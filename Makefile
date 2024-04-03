all: link
compile: obj/clibc-user-impl-c.o obj/linux-uefi-c.o obj/main-c.o obj/atof-c.o obj/heap-c.o obj/math-c.o obj/stdio-c.o obj/string-c.o obj/strtod-c.o obj/printf-c.o obj/cppbase-cpp.o
link: compile
	ld -o bin/efitest -m elf_x86_64 obj/clibc-user-impl-c.o obj/linux-uefi-c.o obj/main-c.o obj/atof-c.o obj/heap-c.o obj/math-c.o obj/stdio-c.o obj/string-c.o obj/strtod-c.o obj/printf-c.o obj/cppbase-cpp.o -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o -lefi -lgnuefi -nostdlib
clean:
	rm -f obj/clibc-user-impl-c.o obj/linux-uefi-c.o obj/main-c.o obj/atof-c.o obj/heap-c.o obj/math-c.o obj/stdio-c.o obj/string-c.o obj/strtod-c.o obj/printf-c.o obj/cppbase-cpp.o bin/efitest


obj/clibc-user-impl-c.o: src/clibc-user-impl.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c src/clibc-user-impl.c -o obj/clibc-user-impl-c.o

obj/linux-uefi-c.o: src/linux-uefi.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c src/linux-uefi.c -o obj/linux-uefi-c.o

obj/main-c.o: src/main.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c src/main.c -o obj/main-c.o

obj/atof-c.o: crosslibc/atof.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/atof.c -o obj/atof-c.o

obj/heap-c.o: crosslibc/heap.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/heap.c -o obj/heap-c.o

obj/math-c.o: crosslibc/math.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/math.c -o obj/math-c.o

obj/stdio-c.o: crosslibc/stdio.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/stdio.c -o obj/stdio-c.o

obj/string-c.o: crosslibc/string.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/string.c -o obj/string-c.o

obj/strtod-c.o: crosslibc/strtod.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/strtod.c -o obj/strtod-c.o

obj/printf-c.o: crosslibc/printf/printf.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/printf/printf.c -o obj/printf-c.o

obj/cppbase-cpp.o: crosslibc/cppbase.cpp
	gcc -std=c++17 -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch -fno-rtti -nostdinc++  -Iinclude  -Iinclude/compat  -Inolibc  -Icrosslibc  -Icrosslibc/STL  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c crosslibc/cppbase.cpp -o obj/cppbase-cpp.o

efi: bin/efitest.efi

bin/efitest.efi: all
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 bin/efitest bin/efitest.efi

test: bin/efitest.efi
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -kernel bin/efitest.efi -m 128M
