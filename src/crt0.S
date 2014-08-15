/*
 * Copyright (C) 2014 by Adrian Danis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Description:
 *  Very small entry point to get us to C. This is because seL4
 *  is a bit unkind and doesn't even give us a stack. So we
 *  create one in the bss, load it, and then get out of assembler
 *  as fast as possible
 */

.global _start
.extern c_start
.text

.align 0x1000
_start:
/* Get the stack */
leal _stack, %esp
/* Boot info frame location was shoved into ebx */
pushl %ebx
/* Get out of here */
call c_start
/* We don't expect to come back */
1: jmp 1b

.bss
.align 16
.space 4096
_stack:
