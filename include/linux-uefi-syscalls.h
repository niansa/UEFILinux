#ifndef _LINUX_UEFI_SYSCALLS_H
#define _LINUX_UEFI_SYSCALLS_H

#include <stdint.h>
#include "linux-uefi-int.h"


struct timeval;
struct linux_dirent64;
struct stat;

void proc_exit(int status);
void proc_abort();

int power_reboot(int magic1, int magic2, int cmd, void *arg);

int time_gettimeofday(struct timeval *restrict tv);
int time_sleep(struct timeval *duration);

int fd_open(const char *pathname, int flags);
int fd_close(int fd);
int fd_fsync(int fd);
int fd_dup(int old_fd);
int fd_dup2(int old_fd, int new_fd);
off_t fd_lseek(int fd, off_t offset, int whence);
ssize_t fd_read(int fd, void *buf, size_t count);
ssize_t fd_write(int fd, const void *buf, size_t count);
int fd_getdents64(int fd, struct linux_dirent64 *dirp, int count);
int fd_fstat(int fd, struct stat *buf);
int fd_stat(const char *path, struct stat *buf);

void *mem_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int mem_munmap(void *addr, size_t length);

uint64_t syscall_emu_x86_64(uint64_t no, uint64_t *args);

#endif /* _LINUX_UEFI_SYSCALLS_H */
