import os

# toolchains options
ARCH        ='risc-v'
CPU         ='virt64'
CROSS_TOOL  ='gcc'

RTT_ROOT = os.getenv('RTT_ROOT') or os.path.join(os.getcwd(), '..', '..')

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'/home/linshire/toolchains/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-linux-ubuntu14/bin'
else:
    print('Please make sure your toolchains is GNU GCC!')
    exit(0)


CFLAGS_MLIBC_TEST =  " -nostdinc" +" -nostdinc " + " -ffreestanding " #+ " -Wl,-wrap=memset "

BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX  = os.getenv('RTT_CC_PREFIX') or 'riscv64-unknown-elf-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'

    DEVICE  = ' -mcmodel=medany -march=rv64imafdc -mabi=lp64 '
    CFLAGS  = DEVICE + '-ffreestanding -flax-vector-conversions -Wno-cpp -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -fdiagnostics-color=always'
    AFLAGS  = ' -c' + DEVICE + ' -x assembler-with-cpp -D__ASSEMBLY__ '
    LFLAGS  = DEVICE + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,_start -T link.lds' + ' -lsupc++ -lgcc -static ' + CFLAGS_MLIBC_TEST

    
    CPATH   = ''
    LPATH   = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -ggdb -fvar-tracking '
        AFLAGS += ' -ggdb'
    else:
        CFLAGS += ' -O2 -Os'

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtthread.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
