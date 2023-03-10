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

#define uart_name  "uart0"

int putchar(int chr)
{
    char*word = "---come from mlibc output---";
    rt_device_t uart_usr = 0;
    uart_usr = rt_device_find(uart_name);

    if(chr == '\n')
        rt_device_write(uart_usr, 0, word, strlen(word));
        rt_device_write(uart_usr, 0, &chr, 1);
    return chr;
};



int main(void)
{
    float a =3.456789;
    printf("%d\n",sizeof(unsigned long long));
    printf("%d\n",sizeof(short int ));
    printf("%d\n",sizeof(long));
    printf("%d\n",sizeof(float));
    printf("%d\n",sizeof(double));

    return 0;
}

