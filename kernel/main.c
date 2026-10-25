
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 \xx
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "kernel.h"

#if !defined(__aarch64__)
static_assert(1 == 0, "ARM64 only!");
#endif

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "fgetpriority: " fmt
#endif

#include "vmap_patch.h"

// blank slot
#define __AARCH64_shitcall	42

static uintptr_t sct = 0x0;
static uintptr_t blank_slot = 0x0;

// magic commands
#define gp_magic1		0x11111111	// arg0
#define gp_flag_enable		0x1
#define gp_flag_disable		0x2
#define gp_flag_reset		0x3

#ifdef TIF_SECCOMP
#define is_seccomp_enabled() test_thread_flag(TIF_SECCOMP)
#else
#define is_seccomp_enabled() (!!current->seccomp.mode)
#endif

#include "routine.c"

// common handler
static inline void shitcall(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5)
{
	if (likely(is_seccomp_enabled()))
		return;

	if (likely(!!current_uid().val))
		return;

	pr_info("shitcall: 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",  arg0, arg1, arg2, arg3, arg4, arg5);

	if (arg0 != gp_magic1)
		return;

	switch (arg1) {
	case gp_flag_enable:
		gp_set_package_bit((int)arg2, true);
		break;
	case gp_flag_disable:
		gp_set_package_bit((int)arg2, false);
		break;
	case gp_flag_reset:
		gp_reset_bitmap();
		break;
	default:
		pr_info("wrong input\n");
		break;
	}

	// confirm transmission to user
	!!copy_to_user((void __user *)arg5, &arg5, sizeof(uintptr_t));
}

// syscall wrapper and legacy
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
__attribute__((used))
static int handle_shitcall(const struct pt_regs *regs)
{
	uintptr_t arg0 = (uintptr_t)regs->regs[0];
	uintptr_t arg1 = (uintptr_t)regs->regs[1];
	uintptr_t arg2 = (uintptr_t)regs->regs[2];
	uintptr_t arg3 = (uintptr_t)regs->regs[3];
	uintptr_t arg4 = (uintptr_t)regs->regs[4];
	uintptr_t arg5 = (uintptr_t)regs->regs[5];
	shitcall(arg0, arg1, arg2, arg3, arg4, arg5);
	return -ENOSYS;
}
#else
__attribute__((used))
static int handle_shitcall(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5)
{
	shitcall(arg0, arg1, arg2, arg3, arg4, arg5);
	return -ENOSYS;
}
#endif

static int patch_blank_slot(void)
{
	uintptr_t *table = (uintptr_t *)sct;
	blank_slot = table[__AARCH64_shitcall];

	int ret = write_to_readonly_slot((uintptr_t)&table[__AARCH64_shitcall], (uintptr_t)handle_shitcall);
	pr_info("%s: slot: %d ret: %d\n", __func__, __AARCH64_shitcall, ret);
	return ret;
}

static int restore_blank_slot(void)
{
	uintptr_t *table = (uintptr_t *)sct;
	int ret = write_to_readonly_slot((uintptr_t)&table[__AARCH64_shitcall], blank_slot);
	pr_info("%s: ret: %d\n", __func__, ret);
	return ret;
}


#if 0
static void print_sct(void)
{
#define ptr_per_page (PAGE_SIZE / sizeof(uintptr_t))
	uintptr_t *t = (uintptr_t *)sct;
	uintptr_t last = 0x0;

	for (unsigned int i = 0; i < ptr_per_page; i++) {
		last = t[i];
		pr_info("sys_call_table: entry: %u: 0x%lx\n", i, last);
	}
}
#endif

#ifdef MODULE
static unsigned long sct_param = 0x0;
module_param(sct_param, ulong, 0000);
MODULE_PARM_DESC(sct_param, "sys_call_table symaddr");
#endif

static int __init fgetpriority_init(void) 
{
	pr_info("init: Hello World!\n");

#ifdef MODULE
	sct = (uintptr_t)sct_param;
	if (!sct)
		return -EAGAIN;
	kobject_del(&THIS_MODULE->mkobj.kobj);

	char buf[KSYM_SYMBOL_LEN];
	sprint_symbol_no_offset(buf, sct);

	buf[KSYM_SYMBOL_LEN - 1] = 0;
	char ksym[] = "sys_call_table";
	if (!!strcmp(buf, ksym)) {
		pr_info("init: wrong symbol!? %s found!\n", buf);
		return -EAGAIN;
	}
	pr_info("init: %s: 0x%lx \n", buf, sct);
#else
	sct = (uintptr_t)sys_call_table;
	pr_info("init: sys_call_table: 0x%lx \n", sct);
#endif

	int ret = patch_blank_slot();
	if (ret)
		return ret;

	ret = getpriority_hook_init();
	if (ret)
		return ret;

	return 0;
}

static int lkm_unload(void *)
{
	int ret = getpriority_hook_exit();
	if (ret)
		return 0;

	pr_info("exit: rc: %d \n", ret);
	ret = restore_blank_slot();
	if (ret)
		return 0;

	pr_info("exit: rc: %d \n", ret);
	return 0;
}

static void __exit fgetpriority_exit(void)
{
#ifndef MODULE
	__builtin_trap();
	__builtin_unreachable();
#endif

//	stop_machine(lkm_unload_sm, NULL, NULL);
	lkm_unload(nullptr);
}

module_init(fgetpriority_init);
module_exit(fgetpriority_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xx");
MODULE_DESCRIPTION("fgetpriority");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 13, 0)
MODULE_IMPORT_NS("VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver");
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif
