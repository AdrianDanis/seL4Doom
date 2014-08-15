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
 *  Init and general functions for interfaction with seL4. This
 *  essentially the 'OS' that runs doom
 */

#include <autoconf.h>
#include <simple/simple.h>
#include <simple-stable/simple-stable.h>
#include <vka/vka.h>
#include <allocman/allocman.h>
#include <allocman/vka.h>
#include <allocman/bootstrap.h>
#include <vspace/vspace.h>
#include <sel4utils/vspace.h>
#include <syscall_stubs_sel4.h>
#include <sel4platsupport/platsupport.h>
#include <sel4utils/stack.h>

static void (*next_function)(void) = NULL;

allocman_t *allocman;
vka_t global_vka;
simple_t global_simple;
vspace_t global_vspace;
static char initial_mem_pool[8 * 1024];
static sel4utils_alloc_data_t vspace_alloc_data;
extern vspace_t *muslc_this_vspace;
extern reservation_t *muslc_brk_reservation;
static reservation_t brk_reservation;
extern void *muslc_brk_reservation_start;

/* this pool is virtual, so can make it big */
#define ALLOCMAN_POOL_SIZE (16 * 1024 * 1024)

/* maximal size of the heap */
#define HEAP_SIZE (64 * 1024 * 1024)

static void sel4_init_continued(void) {
    reservation_t *reservation;
    void *vaddr;
    int error;
    /* Set our syscall table. We sort of prayed no one did this earlier
     * but we couldn't set it on our other stack */
    SET_MUSLC_SYSCALL_TABLE;
    /* Get printf working */
    platsupport_serial_setup_simple(&global_vspace, &global_simple, &global_vka);
    /* Test it */
    printf("Initializing seL4Doom OS...\n");
    /* configure the heap */
    muslc_this_vspace = &global_vspace;
    muslc_brk_reservation = &brk_reservation;
    error = sel4utils_reserve_range_no_alloc(&global_vspace, &brk_reservation, HEAP_SIZE, seL4_AllRights, 1, &muslc_brk_reservation_start);
    assert(!error);
    /* Give allocman a small virtual memory pool for putting book keeping in */
    reservation = vspace_reserve_range(&global_vspace, ALLOCMAN_POOL_SIZE, seL4_AllRights, 1, &vaddr);
    assert(reservation);
    bootstrap_configure_virtual_pool(allocman, vaddr, ALLOCMAN_POOL_SIZE, simple_get_pd(&global_simple));
    printf("Completed seL4Doom OS initialization. Running doom...\n");
    /* go run whatever we were told to */
    next_function();
}

void sel4_init(void (*next)(void)) {
    seL4_BootInfo *bootinfo;
    /* Get bootinfo and init simple so we can forget about bootinfo */
    bootinfo = seL4_GetBootInfo();
    simple_stable_init_bootinfo(&global_simple, bootinfo);
    /* Create allocator */
    allocman = bootstrap_new_2level_simple(&global_simple, 8, 8, sizeof(initial_mem_pool), initial_mem_pool);
    assert(allocman);
    /* Always need a vka */
    allocman_make_vka(&global_vka, allocman);
    /* Create vspace */
    sel4utils_bootstrap_vspace_with_bootinfo(&global_vspace, &vspace_alloc_data, simple_get_pd(&global_simple), &global_vka, bootinfo, NULL, NULL);
    /* We have enough to get the hell of whatever piece of shit stack we are on */
    next_function = next;
    sel4utils_run_on_stack(&global_vspace, sel4_init_continued);
}
