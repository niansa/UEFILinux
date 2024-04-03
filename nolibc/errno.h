/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * Minimal errno definitions for NOLIBC
 * Copyright (C) 2017-2022 Willy Tarreau <w@1wt.eu>
 */

#ifndef _NOLIBC_ERRNO_H
#define _NOLIBC_ERRNO_H

#include <stdio.h>
#include <asm/errno.h>

#define SET_ERRNO(v) do { errno = (v); } while (0)
int errno __attribute__((weak));


/* errno codes all ensure that they will not conflict with a valid pointer
 * because they all correspond to the highest addressable memory page.
 */
#define MAX_ERRNO 4095


static __attribute__((unused))
void perror(const char *msg)
{
	fprintf(stderr, "%s%serrno=%d\n", (msg && *msg) ? msg : "", (msg && *msg) ? ": " : "", errno);
}

#endif /* _NOLIBC_ERRNO_H */
