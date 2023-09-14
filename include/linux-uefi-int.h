#ifndef _LINUX_UEFI_INT_H
#define _LINUX_UEFI_INT_H

#ifdef LINUX_UEFI_USE_INTERNAL_INTS
#ifndef NULL
#define NULL ((void*)0)
#endif /* NULL */

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif /* __cplusplus */

typedef unsigned long size_t;
typedef signed long ssize_t;
typedef signed long time_t;
typedef signed long off_t;
typedef signed int pid_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned int dev_t;
typedef unsigned long ino_t;
typedef unsigned int mode_t;
typedef unsigned long nlink_t;
typedef signed long blksize_t;
typedef signed long blksize_t;
typedef signed long blkcnt_t;
typedef unsigned long clock_t;
typedef unsigned long nfds_t;
#else
#	include <sys/types.h>
#endif /* LINUX_UEFI_USE_INTERNAL_INTS */

#endif /* _LINUX_UEFI_INT_H */
