/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <rthw.h>
#include <stdio.h>
#include <string.h>
#include "utest.h"
#define uart_name  "uart0"

int putchar(int chr)
{
    rt_device_t uart_usr = 0;
    uart_usr = rt_device_find(uart_name);

        rt_device_write(uart_usr, 0, &chr, 1);
    return chr;
};



int main(void)
{

    //utest_testcase_run(NULL,NULL);
    while(1){};
    return 0;
}

