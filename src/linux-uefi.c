/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * Linux-compatible wrapper around UEFI
 * Copyright (C) 2023 Nils Sauer <tuxifan@posteo.de>
 */

#ifndef LINUX_UEFI_USE_INTERNAL_INTS
#	define LINUX_UEFI_USE_INTERNAL_INTS
#endif /* LINUX_UEFI_USE_INTERNAL_INTS */
#include "linux-uefi.h"
#include "linux-uefi-syscalls.h"

#include <efi.h>
#include <efilib.h>


/* Macros */
#define MAX_FDS 16
#define VIRTUAL_FD_BASE 4
#define TO_REAL_FD(fd) (fd-VIRTUAL_FD_BASE)
#define TO_VIRTUAL_FD(fd) (fd+VIRTUAL_FD_BASE)

#define EMU_PID 2
#define EMU_UID 1000
#define EMU_GID 1000


/* Global variables */
char *environ[] = {NULL};
const unsigned long _auxv[] = {0};

/* startup code and handles */
static EFI_HANDLE Image;
static EFI_FILE_HANDLE Volume;
static EFI_FILE_HANDLE fds[MAX_FDS];

extern
int main(int argc, char **argv);

static
EFI_FILE_HANDLE GetVolume(EFI_HANDLE image);

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);

	Image = ImageHandle;
	Volume = GetVolume(ImageHandle);

	for (unsigned it = 0; it != MAX_FDS; it++) {
		fds[it] = (EFI_FILE_HANDLE)0;
	}

	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"Initialized! Calling main()...\r\n");

	/* Stub argv, argc for now */
	char *argv[1] = {"main.efi"};

	return main(1, argv)?EFI_END_OF_FILE/*?*/:EFI_SUCCESS;
}


/* Tons of tiny little helper functions */
static
EFI_FILE_HANDLE GetVolume(EFI_HANDLE image) {
  EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
  EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
  EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

  /* get the loaded image protocol interface for our "image" */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
  /* get the volume handle */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
  return Volume;
}

static
CHAR16 *bstr_to_wstr(const char *input) {
	static char fres[1024];
	unsigned it;
	for (it = 0; input[it] && it != sizeof(fres)/2 - 2; it++) {
		fres[it*2] = input[it];
		fres[it*2+1] = '\0';
	}
	fres[it*2] = '\0';
	fres[it*2+1] = '\0';

	return (CHAR16 *)fres;
}

static
int efi_status_to_errno(EFI_STATUS status) {
	switch((int)(status & 0xffff)) {
		case EFI_WRITE_PROTECTED & 0xffff: return EROFS;
		case EFI_ACCESS_DENIED & 0xffff: return EACCES;
		case EFI_VOLUME_FULL & 0xffff: return ENOSPC;
		case EFI_NOT_FOUND & 0xffff: return ENOENT;
		case EFI_INVALID_PARAMETER & 0xffff: return EINVAL;
		default: return EIO;
	}
}


/* "Process" handling */
void proc_exit(int status) {
	uefi_call_wrapper(BS->Exit, 4, Image, status?EFI_END_OF_FILE/*?*/:EFI_SUCCESS, 0, NULL);
}

void proc_abort() {
	uefi_call_wrapper(BS->Exit, 4, Image, EFI_ABORTED, 0, NULL);
}

int proc_kill(pid_t pid, int sig) {
	if (pid == EMU_PID - 1)
		return -EPERM;

	if (pid != EMU_PID)
		return -ESRCH;

	if (sig >= 30)
		return -EINVAL;

	/* TODO: Signal handling! */

	proc_abort();

	while (1); /* Don't want the compiler to complain */
}


/* "Power" handling */
int power_reboot(int magic1, int magic2, int cmd, void *arg) {
	if (magic1 != LINUX_REBOOT_MAGIC1)
		return -EINVAL;
	if (magic2 != LINUX_REBOOT_MAGIC2 &&
		magic2 != LINUX_REBOOT_MAGIC2A &&
		magic2 != LINUX_REBOOT_MAGIC2B &&
		magic2 != LINUX_REBOOT_MAGIC2C)
		return -EINVAL;

	if (cmd == LINUX_REBOOT_CMD_CAD_OFF ||
		cmd == LINUX_REBOOT_CMD_CAD_ON)
		return -EINVAL; /* TODO: Implement this! */

	if (cmd == LINUX_REBOOT_CMD_HALT) {
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"\r\nSystem halted.");
		while (1) asm("cli; hlt"); /* TODO: Make this more portable */
	}

	if (cmd == LINUX_REBOOT_CMD_KEXEC)
		return -EINVAL; /* TODO: Implement this! */

	if (cmd == LINUX_REBOOT_CMD_SW_SUSPEND)
		return -EINVAL; /* Won't ever be implemented */

	if (cmd == LINUX_REBOOT_CMD_POWER_OFF) {
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"\r\nPower down.");
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 34*2, L"LINUX_REBOOT_CMD_POWER_OFF called");
		return 0;
	}

	if (cmd == LINUX_REBOOT_CMD_RESTART) {
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"\r\nRestarting system.");
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 32, L"LINUX_REBOOT_CMD_RESTART called");
		return 0;
	}

	if (cmd == LINUX_REBOOT_CMD_RESTART2) {
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"\r\nRestarting system with command '");
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, bstr_to_wstr((const char*)arg));
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"'");
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 33, L"LINUX_REBOOT_CMD_RESTART2 called");
		return 0;
	}

	return -EINVAL;
}


/* Timekeeping functions */
static
/* from musl */
uint64_t time_year_to_secs(uint64_t year, int *is_leap) {
	int y, cycles, centuries, leaps, rem;

	if (year-2ULL <= 136) {
		y = (int)year;
		leaps = (y-68)>>2;
		if (!((y-68)&3)) {
			leaps--;
			if (is_leap) *is_leap = 1;
		} else if (is_leap) *is_leap = 0;
		return 31536000ULL*(uint64_t)(y-70) + 86400ULL*(uint64_t)leaps;
	}

	static int static_is_leap = 0;
	if (!is_leap) is_leap = &static_is_leap;
	cycles = (int)((year-100) / 400);
	rem = (year-100) % 400;
	if (rem < 0) {
		cycles--;
		rem += 400;
	}
	if (!rem) {
		*is_leap = 1;
		centuries = 0;
		leaps = 0;
	} else {
		if (rem >= 200) {
			if (rem >= 300) { centuries = 3; rem -= 300; }
			else { centuries = 2; rem -= 200; }
		} else {
			if (rem >= 100) { centuries = 1; rem -= 100; }
			else centuries = 0;
		}
		if (!rem) {
			*is_leap = 0;
			leaps = 0;
		} else {
			leaps = rem / 4;
			rem %= 4;
			*is_leap = !rem;
		}
	}

	leaps += 97*cycles + 24*centuries - *is_leap;

	return (uint64_t)(year-100) * 31536000ULL + (uint64_t)leaps * 86400ULL + 946684800ULL + 86400ULL;
}

static
/* adpoted from posix-uefi */
time_t time_efi_time_to_time_t(EFI_TIME *time) {
	time->Year += /* workaround some buggy firmware*/ time->Year > 2000 ? -1900 : 98;
	time->Month -= 1;

	static const uint64_t secs_through_month[] = {
		0, 31*86400, 59*86400, 90*86400,
		120*86400, 151*86400, 181*86400, 212*86400,
		243*86400, 273*86400, 304*86400, 334*86400 };
	int is_leap;
	uint64_t year = (uint64_t)time->Year, t, adj;
	int month = time->Month;
	if (month >= 12 || month < 0) {
		adj = (uint64_t)month / 12;
		month %= 12;
		if (month < 0) {
			adj--;
			month += 12;
		}
		year += adj;
	}
	t = time_year_to_secs(year, &is_leap);
	t += secs_through_month[month];
	if (is_leap && month >= 2) t += 86400;
	t += 86400ULL * (uint64_t)(time->Day-1);
	t += 3600ULL * (uint64_t)time->Hour;
	t += 60ULL * (uint64_t)time->Minute;
	t += (uint64_t)time->Second;
	return (time_t)t;
}

static
time_t time_cstd_time(time_t *tloc) {
	EFI_TIME efi_time = {0};
	uefi_call_wrapper(ST->RuntimeServices->GetTime, 2, &efi_time, NULL);
	time_t ret = time_efi_time_to_time_t(&efi_time);
	if (tloc)
		*tloc = ret;
	return ret;
}

int time_gettimeofday(struct timeval *tv) {
	tv->tv_sec = time_cstd_time(NULL);
	tv->tv_usec = 0;

	return 0;
}

int time_sleep(struct timeval *duration) {
	EFI_STATUS status = uefi_call_wrapper(BS->Stall, 1, duration->tv_sec * 1000000UL + duration->tv_usec);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return 0;
}


/* Basic I/O */
static
int fd_real_find_free() {
	for (unsigned it = 0; it != MAX_FDS; it++) {
	   if (fds[it])
		   return it;
	}

	return -1;
}

static
int fd_real_for_handle(EFI_FILE_HANDLE FileHandle) {
	for (unsigned it = 0; it != MAX_FDS; it++) {
	   if (fds[it] == FileHandle)
		   return it;
	}
	return 0;
}

inline static
void fd_real_reset(int fd) {
	fds[fd] = (EFI_FILE_HANDLE)0;
}

static
EFI_STATUS fd_efi_get_file_info(EFI_FILE_HANDLE FileHandle, EFI_FILE_INFO *info) {
	EFI_GUID infoGuid = EFI_FILE_INFO_ID;
	UINTN fsiz = sizeof(EFI_FILE_INFO);

	return uefi_call_wrapper(FileHandle->GetInfo, 4, FileHandle, &infoGuid, &fsiz, info);
}
static
EFI_STATUS fd_efi_set_file_info(EFI_FILE_HANDLE FileHandle, const EFI_FILE_INFO *info) {
	EFI_GUID infoGuid = EFI_FILE_INFO_ID;
	UINTN fsiz = sizeof(EFI_FILE_INFO);

	return uefi_call_wrapper(FileHandle->SetInfo, 4, FileHandle, &infoGuid, fsiz, &info);
}

int fd_open(const char *pathname, int flags) {
	int fd = fd_real_find_free();

	if (fd < 0)
		return -EMFILE;

	EFI_FILE_HANDLE FileHandle;

	/* Always read mode to avoid firmware bugs */
	UINT64 OpenMode = EFI_FILE_MODE_READ;
	if (flags & O_APPEND)
		OpenMode |= EFI_FILE_MODE_WRITE;
	if (flags & O_CREAT)
		OpenMode |= EFI_FILE_MODE_CREATE;
	if (flags & O_WRONLY || flags & O_RDWR)
		OpenMode |= EFI_FILE_MODE_WRITE;

	EFI_STATUS status = uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, bstr_to_wstr(pathname), OpenMode, (flags & O_DIRECTORY)?EFI_FILE_DIRECTORY:0);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	EFI_FILE_INFO info;
	status = fd_efi_get_file_info(FileHandle, &info);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	if (flags & O_APPEND) {
		uefi_call_wrapper(FileHandle->SetPosition, 2, FileHandle, 0xFFFFFFFFFFFFFFFF);
	} else {
		if (OpenMode & EFI_FILE_MODE_WRITE) {
			info.FileSize = 0;
			fd_efi_set_file_info(FileHandle, &info);
		}
	}
	if (flags & O_DIRECTORY) {
		if (!(info.Attribute & EFI_FILE_DIRECTORY))
			return -ENOTDIR;
	} else {
		if (info.Attribute & EFI_FILE_DIRECTORY)
			return -EISDIR;
	}

	return TO_VIRTUAL_FD(fd);
}

int fd_close(int fd) {
	if (fd > MAX_FDS)
		return -EBADF;
	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	fd_real_reset(TO_REAL_FD(fd));

	if (!fd_real_for_handle(FileHandle)) {
		EFI_STATUS status = uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
		if (EFI_ERROR(status))
			return -efi_status_to_errno(status);
	}

	return 0;
}

int fd_fsync(int fd) {
	if (fd > MAX_FDS)
		return -EBADF;
	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	EFI_STATUS status = uefi_call_wrapper(FileHandle->Flush, 1, FileHandle);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return 0;
}

int fd_dup(int old_fd) {
	if (old_fd > MAX_FDS)
		return -EBADF;
	if (old_fd < VIRTUAL_FD_BASE) /* can't dup stdandard I/O! */
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(old_fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	int new_fd = fd_real_find_free();

	if (new_fd < 0)
		return -EMFILE;

	fds[new_fd] = FileHandle;

	return TO_VIRTUAL_FD(new_fd);
}

int fd_dup2(int old_fd, int new_fd) {
	if (old_fd > MAX_FDS)
		return -EBADF;
	if (old_fd < VIRTUAL_FD_BASE) /* can't dup stdandard I/O! */
		return -EACCES;

	if (fds[TO_REAL_FD(new_fd)]) {
		int res = fd_close(new_fd);
		if (res < 0)
			return res;
	}

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(old_fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	if (new_fd < 0)
		return -EMFILE;

	fds[new_fd] = FileHandle;

	return TO_VIRTUAL_FD(new_fd);
}

off_t fd_lseek(int fd, off_t offset, int whence) {
	if (fd > MAX_FDS)
		return -EBADF;
	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	EFI_STATUS status;
	UINT64 offset64;
	switch (whence) {
		case SEEK_SET: {
			offset64 = offset;
			status = uefi_call_wrapper(FileHandle->SetPosition, 2, FileHandle, offset64);
		} break;
		case SEEK_END: {
			status = uefi_call_wrapper(FileHandle->SetPosition, 2, FileHandle, 0xFFFFFFFFFFFFFFFF);
			if (EFI_ERROR(status))
				break;
		} /* fallthrough */
		case SEEK_CUR: {
			status = uefi_call_wrapper(FileHandle->GetPosition, 2, FileHandle, &offset64);
			if (EFI_ERROR(status))
				break;
			offset64 += offset;
			status = uefi_call_wrapper(FileHandle->SetPosition, 2, FileHandle, offset64);
		} break;
	}

	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return offset64;
}

ssize_t fd_read(int fd, void *buf, size_t count) {
	if (fd > MAX_FDS)
		return -EBADF;

	/* Standard I/O handling */
	if (fd == 0) {
		unsigned it;
		for (it = 0; it != count; it++) {
			/* delay 1ms */
			uefi_call_wrapper(BS->Stall, 1, 1000);
			/* Wait for key */
			EFI_EVENT Events[1] = {ST->ConIn->WaitForKey};
			UINTN index;
			uefi_call_wrapper(BS->WaitForEvent, 3, 1, Events, &index);
			/* Read key */
			EFI_INPUT_KEY key;
			uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
			/* Get key as character */
			char c = ((char *)&key.UnicodeChar)[0];
			/* Avoid backspace if no input */
			if (it == 0 && c == '\b')
				continue;
			/* Print key */
			CHAR16 print_buf[2] = {key.UnicodeChar, L'\0'};
			uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, print_buf);
			if (c == '\r') {
				print_buf[0] = '\n';
				uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, print_buf);
			}
			/* Translate line feed to newline */
			if (c == '\r')
				c = '\n';
			/* Add key to buffer */
			((char *)buf)[it] = c;
			/* Stop if linebreak */
			if (c == '\n')
				break;
		}

		return it + 1;
	}

	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	EFI_STATUS status = uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &count, buf);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return count;
}

ssize_t fd_write(int fd, const void *buf, size_t count) {
	if (fd > MAX_FDS)
		return -EBADF;

	/* Standard I/O handling */
	if (fd == 1 || fd == 2) {
		const char *src_buf = (const char *)buf;
		CHAR16 str_buf[count*2];
		unsigned it;
		unsigned str_it;
		for (it = 0, str_it = 0; it != count; it++, str_it++) {
			if (src_buf[it] == '\0') {
				str_it--;
				continue;
			}
			if (src_buf[it] == '\n') {
				str_buf[str_it] = L'\r';
				str_buf[++str_it] = L'\n';
				continue;
			}

			((char*)(str_buf + str_it))[0] = (CHAR16)(src_buf[it]);
			((char*)(str_buf + str_it))[1] = '\0';
		}
		str_buf[str_it] = L'\0';

		EFI_STATUS status = uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, str_buf);
		if (EFI_ERROR(status))
			return -efi_status_to_errno(status);

		return count;
	}

	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	EFI_STATUS status = uefi_call_wrapper(FileHandle->Write, 3, FileHandle, &count, buf);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return count;
}

int fd_getdents64(int fd, struct linux_dirent64 *dirp, int count) {
	if (fd > MAX_FDS)
		return -EBADF;
	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	size_t fres = 0;

	count = count / sizeof(struct linux_dirent64);
	for (unsigned it = 0; it != count; it++) {
		EFI_FILE_INFO info[16]; /* Some cheap padding to avoid heap use, x16 should be plently */
		UINTN infoSize = sizeof(info);
		EFI_STATUS status = uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &infoSize, info);
		if (EFI_ERROR(status))
			return -efi_status_to_errno(status);

		struct linux_dirent64 *dire = dirp + count;

		unsigned it;
		for (it = 0; info->FileName[it]; it++) {
			char *c = (char*)&info->FileName[it];
			dire->d_name[it] = *c;
		}
		dire->d_name[it] = '\0';

		dire->d_ino = 0; /* UEFI isn't giving us that information */
		dire->d_off = dire->d_reclen = sizeof(uint64_t) + sizeof(int64_t) + sizeof(unsigned short) + sizeof(unsigned char) + it + 1;

		fres += dire->d_reclen;
	}

	return fres;
}

static
int fd_efi_stat(EFI_FILE_HANDLE FileHandle, struct stat *buf) {
	buf->st_dev = (unsigned long)Volume;
	buf->st_ino = 0; /* UEFI won't tell us */
	buf->st_nlink = 1;
	buf->st_mode = 0666;
	buf->st_uid = EMU_UID;
	buf->st_gid = EMU_GID;
	buf->st_blksize = 0;
	buf->st_atim.tv_nsec = 0;
	buf->st_mtim.tv_nsec = 0;
	buf->st_ctim.tv_nsec = 0;

	EFI_FILE_INFO info;
	EFI_STATUS status = fd_efi_get_file_info(FileHandle, &info);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	buf->st_size = info.FileSize;
	buf->st_blocks = info.PhysicalSize;
	buf->st_atime = time_efi_time_to_time_t(&info.LastAccessTime);
	buf->st_mtime = time_efi_time_to_time_t(&info.ModificationTime);
	buf->st_ctime = time_efi_time_to_time_t(&info.CreateTime);

	return 0;
}

int fd_fstat(int fd, struct stat *buf) {
	if (fd > MAX_FDS)
		return -EBADF;
	if (fd < VIRTUAL_FD_BASE)
		return -EACCES;

	EFI_FILE_HANDLE FileHandle = fds[TO_REAL_FD(fd)];
	if (FileHandle == (EFI_FILE_HANDLE)0)
		return -EBADF;

	return fd_efi_stat(FileHandle, buf);
}

int fd_stat(const char *path, struct stat *buf) {
	EFI_FILE_HANDLE FileHandle;
	EFI_STATUS status = uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, bstr_to_wstr(path), EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return fd_efi_stat(FileHandle, buf);
}

int fd_select(int nfds, uint64_t *readfds, uint64_t *writefds, uint64_t *exceptfds, struct timeval *timeout) {
	if (nfds || readfds || writefds || exceptfds)
		return ENOTSUP;

	return time_sleep(timeout);
}


/* Memory management functions */
void *mem_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void)addr;
	(void)prot;
	(void)flags;
	(void)offset;

	if (fd > 0)
		return (void *)(long)-ENOTSUP;

	void *fres;

	EFI_STATUS status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, (UINTN)length, &fres);
	if (EFI_ERROR(status))
		return (void *)(long)-efi_status_to_errno(status);

	return fres;
}

int mem_munmap(void *addr, size_t length) {
	(void)length;

	EFI_STATUS status = uefi_call_wrapper(BS->FreePool, 1, addr);
	if (EFI_ERROR(status))
		return -efi_status_to_errno(status);

	return 0;
}


/* System call emulation */
#pragma GCC diagnostic ignored "-Wint-conversion"
uint64_t syscall_emu_x86_64(uint64_t no, uint64_t *args) {
	switch (no) {
		case 0: return fd_read(args[0], args[1], args[2]);
		case 1: return fd_write(args[0], args[1], args[2]);
		case 2: return fd_open(args[0], args[1]);
		case 3: return fd_close(args[0]);
		case 4: return fd_stat(args[0], args[1]);
		case 5: return fd_fstat(args[0], args[1]);
		case 6: return fd_stat(args[0], args[1]);
		case 8: return fd_lseek(args[0], args[1], args[2]);
		case 9: return mem_mmap(args[0], args[1], args[2], args[3], args[4], args[5]);
		case 11: return mem_munmap(args[0], args[1]);
		case 23: return fd_select(args[0], args[1], args[2], args[3], args[4]);
		case 32: return fd_dup(args[0]);
		case 33: return fd_dup2(args[0], args[1]);
		case 60: proc_exit(args[0]); return 0;
		case 74: return fd_fsync(args[0]);
		case 75: return fd_fsync(args[0]);
		case 62: return proc_kill(args[0], args[1]);
		case 96: return time_gettimeofday(args[0]);
		case 169: return power_reboot(args[0], args[1], args[2], args[3]);
		case 217: return fd_getdents64(args[0], args[1], args[2]);

		case 10: return 0; /* At least pretend we care */
		case 24: return 0; /* There is no multithreading, so nothing to do */
		case 39: return EMU_PID;
		case 102: return EMU_UID;
		case 104: return EMU_GID;
		case 105: return -EPERM;
		case 106: return -EPERM;
		case 107: return EMU_UID;
		case 108: return EMU_GID;
		case 110: return EMU_PID-1;
		case 186: return EMU_PID;

		default: return -ENOSYS;
	}
}
