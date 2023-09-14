# UEFI Linux
No, this projects aim is *not* booting Linux from UEFI (which is already possible), but instead to <u>emulate</u> basic Linux system calls within the UEFI environment.

## Why???
For fun. Nothing else. It this project would, in theory, eventually allow you to use any Linux C library in the UEFI environment.

## Status

Implemented:
 - Console I/O
 - File I/O
 - Directory I/O
 - Basic memory management (mmap/munmap)
 - Time keeping (gettimeofday/select)
 - Exit/Reboot/Poweroff
 - Basic signals (`getpid()` returns a PID that can be `kill()`ed)
 - System call emulation through function call

Planned:
 - Networking

Won't happen:
 - Multhreading
 - `syscall` instruction emulation

## Misc
This is purely a showcase, not a library meant for production use. I will give no support of any kind, but I'm happy to answer any development related questions you may have.
