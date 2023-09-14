all: link
compile: obj/linux-uefi-c.o obj/main-c.o
link: compile
	ld -o bin/efitest -m elf_x86_64 obj/linux-uefi-c.o obj/main-c.o -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o -lefi -lgnuefi -nostdlib
clean:
	rm -f obj/linux-uefi-c.o obj/main-c.o bin/efitest


obj/linux-uefi-c.o: src/linux-uefi.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c src/linux-uefi.c -o obj/linux-uefi-c.o

obj/main-c.o: src/main.c
	gcc -DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch  -Iinclude  -Iinclude/compat  -Inolibc  -I/usr/include/efi  -I/usr/include/efi/x86_64  -I/usr/include/efi/protocol  -c src/main.c -o obj/main-c.o

efi: bin/efitest.efi

bin/efitest.efi: all
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 bin/efitest bin/efitest.efi

test: bin/efitest.efi
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -kernel bin/efitest.efi -m 128M
