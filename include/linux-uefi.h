#ifndef _LINUX_UEFI_H
#define _LINUX_UEFI_H

#include <stdint.h>
#include "linux-uefi-int.h"


/* Types */
struct timeval {
	time_t		tv_sec;		/* seconds */
	long long	tv_usec;	/* microseconds */
};

struct timespec {
	time_t	tv_sec;		/* seconds */
	time_t	tv_nsec;	/* nanoseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

struct linux_dirent64 {
	uint64_t       d_ino;
	int64_t        d_off;
	unsigned short d_reclen;
	unsigned char  d_type;
	char           d_name[];
};

struct stat {
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
	union { time_t st_atime; struct timespec st_atim; };
	union { time_t st_mtime; struct timespec st_mtim; };
	union { time_t st_ctime; struct timespec st_ctim; };
	long          __unused[3];
};


/* Macros */
#define LINUX_REBOOT_MAGIC1 0xfee1dead
#define LINUX_REBOOT_MAGIC2 0x28121969
#define LINUX_REBOOT_MAGIC2A 0x05121996
#define LINUX_REBOOT_MAGIC2B 0x16041998
#define LINUX_REBOOT_MAGIC2C 0x20112000

#define LINUX_REBOOT_CMD_CAD_OFF 0
#define LINUX_REBOOT_CMD_CAD_ON 0x89abcdef
#define LINUX_REBOOT_CMD_HALT 0xcdef0123
#define LINUX_REBOOT_CMD_KEXEC 0x45584543
#define LINUX_REBOOT_CMD_POWER_OFF 0x4321fedc
#define LINUX_REBOOT_CMD_RESTART 0x1234567
#define LINUX_REBOOT_CMD_RESTART2 0xa1b2c3d4
#define LINUX_REBOOT_CMD_SW_SUSPEND 0xd000fce1

#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define EBADF            9      /* Bad file number */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EEXIST          17      /* File exists */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define ENAMETOOLONG    36      /* File name too long */
#define ENOSYS          38      /* Invalid system call number */
#define ENOTSUP         95      /* Operation not supported */

#define SIGABRT   6 /* Abort */

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002

#define O_CREAT		00000100	/* not fcntl */
#define O_APPEND	00002000
#define O_DIRECTORY	00200000	/* must be a directory */

#define SEEK_SET	0	/* seek relative to beginning of file */
#define SEEK_CUR	1	/* seek relative to current file position */
#define SEEK_END	2	/* seek relative to end of file */

#endif /* _LINUX_UEFI_H */
