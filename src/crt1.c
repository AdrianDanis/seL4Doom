/*
 * Copyright (C) 2014 by Adrian Danis.
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
 *  System entry point. C portion after the assembly portion has
 *  given us a stack
 */

#include <sel4/sel4.h>
#include <sel4/bootinfo.h>

void sel4_init(void);
void c_start(seL4_BootInfo *bootinfo) {
    /* set GS */
    asm volatile("movw %w0, %%gs" :: "r"(IPCBUF_GDT_SELECTOR));
    /* set bootinfo */
    seL4_InitBootInfo(bootinfo);
    /* perform seL4 rootserver init (init stuff, switch stack, then finally call main) */
    sel4_init();
    main(1, &"seL4Doom");
}
