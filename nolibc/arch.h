/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * Copyright (C) 2017-2022 Willy Tarreau <w@1wt.eu>
 */

/* Below comes the architecture-specific code. For each architecture, we have
 * the syscall declarations and the _start code definition. This is the only
 * global part. On all architectures the kernel puts everything in the stack
 * before jumping to _start just above us, without any return address (_start
 * is not a function but an entry point). So at the stack pointer we find argc.
 * Then argv[] begins, and ends at the first NULL. Then we have envp which
 * starts and ends with a NULL as well. So envp=argv+argc+1.
 */

#ifndef _NOLIBC_ARCH_H
#define _NOLIBC_ARCH_H

#include "arch-x86_64.h"

#endif /* _NOLIBC_ARCH_H */
