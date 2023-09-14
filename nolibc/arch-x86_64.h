/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * x86_64 specific definitions for NOLIBC
 * Copyright (C) 2017-2022 Willy Tarreau <w@1wt.eu>
 */

#ifndef _NOLIBC_ARCH_X86_64_H
#define _NOLIBC_ARCH_X86_64_H

#include "compiler.h"
#include "linux-uefi-syscalls.h"

/* The struct returned by the stat() syscall, equivalent to stat64(). The
 * syscall returns 116 bytes and stops in the middle of __unused.
 */
struct sys_stat_struct {
	unsigned long st_dev;
	unsigned long st_ino;
	unsigned long st_nlink;
	unsigned int  st_mode;
	unsigned int  st_uid;

	unsigned int  st_gid;
	unsigned int  __pad0;
	unsigned long st_rdev;
	long          st_size;
	long          st_blksize;

	long          st_blocks;
	unsigned long st_atime;
	unsigned long st_atime_nsec;
	unsigned long st_mtime;

	unsigned long st_mtime_nsec;
	unsigned long st_ctime;
	unsigned long st_ctime_nsec;
	long          __unused[3];
};

/* Syscalls for x86_64 :
 *   - registers are 64-bit
 *   - syscall number is passed in rax
 *   - arguments are in rdi, rsi, rdx, r10, r8, r9 respectively
 *   - the system call is performed by calling the syscall instruction
 *   - syscall return comes in rax
 *   - rcx and r11 are clobbered, others are preserved.
 *   - the arguments are cast to long and assigned into the target registers
 *     which are then simply passed as registers to the asm code, so that we
 *     don't have to experience issues with register constraints.
 *   - the syscall number is always specified last in order to allow to force
 *     some registers before (gcc refuses a %-register at the last position).
 *   - see also x86-64 ABI section A.2 AMD64 Linux Kernel Conventions, A.2.1
 *     Calling Conventions.
 *
 * Link x86-64 ABI: https://gitlab.com/x86-psABIs/x86-64-ABI/-/wikis/home
 *
 */

#define my_syscall0(num)                                                      \
({                                                                            \
	uint64_t args[] = {0};                                                    \
	long _ret = syscall_emu_x86_64(num, args);                                \
	_ret;                                                                     \
})

#define my_syscall1(num, ...)                                                 \
({                                                                            \
	uint64_t args[] = {__VA_ARGS__};                                          \
	long _ret = syscall_emu_x86_64(num, args);                                \
	_ret;                                                                     \
})

#define my_syscall2 my_syscall1
#define my_syscall3 my_syscall1
#define my_syscall4 my_syscall1
#define my_syscall5 my_syscall1
#define my_syscall6 my_syscall1

char **environ __attribute__((weak));
const unsigned long *_auxv __attribute__((weak));

#endif /* _NOLIBC_ARCH_X86_64_H */
