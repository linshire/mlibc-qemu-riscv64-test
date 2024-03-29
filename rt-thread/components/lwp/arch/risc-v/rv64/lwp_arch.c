/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     Jesven       first version
 * 2021-02-03     lizhirui     port to riscv64
 * 2021-02-06     lizhirui     add thread filter
 * 2021-02-19     lizhirui     port to new version of rt-smart
 * 2021-03-02     lizhirui     add a auxillary function for interrupt
 * 2021-03-04     lizhirui     delete thread filter
 * 2021-03-04     lizhirui     modify for new version of rt-smart
 * 2021-11-22     JasonHu      add lwp_set_thread_context
 * 2021-11-30     JasonHu      add clone/fork support
 */
#include <rthw.h>
#include <rtthread.h>

#include <stddef.h>

#ifdef ARCH_MM_MMU

#include <lwp.h>
#include <lwp_arch.h>
#include <lwp_user_mm.h>
#include <page.h>

#include <cpuport.h>
#include <encoding.h>
#include <stack.h>

extern rt_ubase_t MMUTable[];

void *lwp_copy_return_code_to_user_stack()
{
    void lwp_thread_return();
    void lwp_thread_return_end();
    rt_thread_t tid = rt_thread_self();

    if (tid->user_stack != RT_NULL)
    {
        rt_size_t size = (rt_size_t)lwp_thread_return_end - (rt_size_t)lwp_thread_return;
        rt_size_t userstack = (rt_size_t)tid->user_stack + tid->user_stack_size - size;
        memcpy((void *)userstack, lwp_thread_return, size);
        return (void *)userstack;
    }

    return RT_NULL;
}

rt_ubase_t lwp_fix_sp(rt_ubase_t cursp)
{
    void lwp_thread_return();
    void lwp_thread_return_end();

    if (cursp == 0)
    {
        return 0;
    }

    return cursp - ((rt_size_t)lwp_thread_return_end - (rt_size_t)lwp_thread_return);
}

rt_thread_t rt_thread_sp_to_thread(void *spmember_addr)
{
    return (rt_thread_t)(((rt_ubase_t)spmember_addr) - (offsetof(struct rt_thread, sp)));
}

void *get_thread_kernel_stack_top(rt_thread_t thread)
{
    return (void *)(((rt_size_t)thread->stack_addr) + ((rt_size_t)thread->stack_size));
}

void *arch_get_user_sp(void)
{
    /* user sp saved in interrupt context */
    rt_thread_t self = rt_thread_self();
    rt_uint8_t *stack_top = (rt_uint8_t *)self->stack_addr + self->stack_size;
    struct rt_hw_stack_frame *frame = (struct rt_hw_stack_frame *)(stack_top - sizeof(struct rt_hw_stack_frame));

    return (void *)frame->user_sp_exc_stack;
}

int arch_user_space_init(struct rt_lwp *lwp)
{
    rt_ubase_t *mmu_table;

    mmu_table = (rt_ubase_t *)rt_pages_alloc(0);
    if (!mmu_table)
    {
        return -1;
    }

    lwp->end_heap = USER_HEAP_VADDR;

    memcpy(mmu_table, MMUTable, ARCH_PAGE_SIZE);
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, mmu_table, ARCH_PAGE_SIZE);
    lwp->aspace = rt_aspace_create(
        (void *)USER_VADDR_START, USER_VADDR_TOP - USER_VADDR_START, mmu_table);
    if (!lwp->aspace)
    {
        return -1;
    }

    return 0;
}

void *arch_kernel_mmu_table_get(void)
{
    return (void *)((char *)MMUTable);
}

void arch_user_space_vtable_free(struct rt_lwp *lwp)
{
    if (lwp && lwp->aspace->page_table)
    {
        rt_pages_free(lwp->aspace->page_table, 0);
        lwp->aspace->page_table = NULL;
    }
}

long _sys_clone(void *arg[]);
long sys_clone(void *arg[])
{
    return _sys_clone(arg);
}

long _sys_fork(void);
long sys_fork(void)
{
    return _sys_fork();
}

long _sys_vfork(void);
long sys_vfork(void)
{
    return _sys_fork();
}

/**
 * set exec context for fork/clone.
 */
int arch_set_thread_context(void (*exit)(void), void *new_thread_stack,
                            void *user_stack, void **thread_sp)
{
    RT_ASSERT(exit != RT_NULL);
    RT_ASSERT(user_stack != RT_NULL);
    RT_ASSERT(new_thread_stack != RT_NULL);
    RT_ASSERT(thread_sp != RT_NULL);
    struct rt_hw_stack_frame *syscall_frame;
    struct rt_hw_stack_frame *thread_frame;

    rt_uint8_t *stk;
    rt_uint8_t *syscall_stk;

    stk = (rt_uint8_t *)new_thread_stack;
    /* reserve syscall context, all the registers are copyed from parent */
    stk -= CTX_REG_NR * REGBYTES;
    syscall_stk = stk;

    syscall_frame = (struct rt_hw_stack_frame *)stk;

    /* modify user sp */
    syscall_frame->user_sp_exc_stack = (rt_ubase_t)user_stack;

    /* skip ecall */
    syscall_frame->epc += 4;

    /* child return value is 0 */
    syscall_frame->a0 = 0;
    syscall_frame->a1 = 0;

    /* reset thread area */
    rt_thread_t thread = rt_container_of((unsigned long)thread_sp, struct rt_thread, sp);
    syscall_frame->tp = (rt_ubase_t)thread->thread_idr;

#ifdef ARCH_USING_NEW_CTX_SWITCH
    extern void *_rt_hw_stack_init(rt_ubase_t *sp, rt_ubase_t ra, rt_ubase_t sstatus);
    rt_ubase_t sstatus = read_csr(sstatus) | SSTATUS_SPP;
    sstatus &= ~SSTATUS_SIE;

    /* compatible to RESTORE_CONTEXT */
    stk = (void *)_rt_hw_stack_init((rt_ubase_t *)stk, (rt_ubase_t)exit, sstatus);
#else
    /* build temp thread context */
    stk -= sizeof(struct rt_hw_stack_frame);

    thread_frame = (struct rt_hw_stack_frame *)stk;

    int i;
    for (i = 0; i < sizeof(struct rt_hw_stack_frame) / sizeof(rt_ubase_t); i++)
    {
        ((rt_ubase_t *)thread_frame)[i] = 0xdeadbeaf;
    }

    /* set pc for thread */
    thread_frame->epc     = (rt_ubase_t)exit;

    /* set old exception mode as supervisor, because in kernel */
    thread_frame->sstatus = read_csr(sstatus) | SSTATUS_SPP;
    thread_frame->sstatus &= ~SSTATUS_SIE; /* must disable interrupt */

    /* set stack as syscall stack */
    thread_frame->user_sp_exc_stack = (rt_ubase_t)syscall_stk;

#endif /* ARCH_USING_NEW_CTX_SWITCH */
    /* save new stack top */
    *thread_sp = (void *)stk;

    /**
     * The stack for child thread:
     *
     * +------------------------+ --> kernel stack top
     * | syscall stack          |
     * |                        |
     * | @sp                    | --> `user_stack`
     * | @epc                   | --> user ecall addr + 4 (skip ecall)
     * | @a0&a1                 | --> 0 (for child return 0)
     * |                        |
     * +------------------------+ --> temp thread stack top
     * | temp thread stack      |           ^
     * |                        |           |
     * | @sp                    | ---------/
     * | @epc                   | --> `exit` (arch_clone_exit/arch_fork_exit)
     * |                        |
     * +------------------------+ --> thread sp
     */
}

/**
 * void lwp_exec_user(void *args, void *kernel_stack, void *user_entry)
 */
void lwp_exec_user(void *args, void *kernel_stack, void *user_entry)
{
    arch_start_umode(args, user_entry, (void *)USER_STACK_VEND, kernel_stack);
}

void *arch_get_usp_from_uctx(struct rt_user_context *uctx)
{
    return uctx->sp;
}

#endif /* ARCH_MM_MMU */
