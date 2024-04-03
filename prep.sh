#! /bin/bash

# Paths
SOURCE_PATHS_C="src/*.c crosslibc/*.c crosslibc/printf/printf.c"
SOURCE_PATHS_CXX="src/*.cpp crosslibc/*.cpp"
SOURCE_PATHS_ASM="src/*.s"
INCLUDE_PATHS="include  include/compat nolibc crosslibc crosslibc/STL  /usr/include/efi /usr/include/efi/x86_64 /usr/include/efi/protocol"

# Config
ARCH="x86_64" # Warning: Also specified in Makefile_cst
COMPILER="gcc"
COMPILER_FLAGS="-DEFI_FUNCTION_WRAPPER -DLINUX_UEFI_USE_INTERNAL_INTS -DCLIBC_NO_MEMCPY -DCLIBC_NO_MEMSET -DCLIBC_NO_LL_HEAP -mno-red-zone -fno-stack-protector -fpic -fshort-wchar -Wno-builtin-declaration-mismatch"
COMPILER_FLAGS_CXX="-fno-rtti -nostdinc++"
ASSEMBLER="nasm"
ASSEMBLER_FLAGS=""
ASSEMBLER_TARGET="elf64"
LINKER="ld"
LINKER_FLAGS="-znocombreloc -T /usr/lib/elf_${ARCH}_efi.lds -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-${ARCH}.o -lefi -lgnuefi -nostdlib"
LINKER_TARGET="elf_x86_64"

# Library functions
function file_continue {
    test -f "$1" || echo continue
}
function getoutfile {
    echo obj/"$(basename "${1%.*}")-"${1##*.}"".o
}
function makeentry_init {
    srcfile="$1"
    outfile="$(getoutfile $srcfile)"
    outfiles+=("$outfile")
    echo " - ${srcfile} produces ${outfile}..." > /dev/stderr
}

# Templates
function makeentry_cfile {
    makeentry_init "$1"
    echo "
${outfile}: ${srcfile}
	${COMPILER} ${COMPILER_FLAGS} ${COMPILER_INCLUEDIRS} -c ${srcfile} -o ${outfile}"
}
function makeentry_cppfile {
    makeentry_init "$1"
    echo "
${outfile}: ${srcfile}
	${COMPILER} -std=c++17 ${COMPILER_FLAGS} ${COMPILER_FLAGS_CXX} ${COMPILER_INCLUEDIRS} -c ${srcfile} -o ${outfile}"
}
function makeentry_sfile {
    makeentry_init "$1"
    echo "
${outfile}: ${srcfile}
	${ASSEMBLER} ${ASSEMBLER_FLAGS} -f ${ASSEMBLER_TARGET} ${srcfile} -o ${outfile}"
}
function makeentry_misc {
    echo "all: link"
    echo "compile: ${outfiles[@]}"
    echo "link: compile
	${LINKER} -o bin/$proj_name -m ${LINKER_TARGET} ${outfiles[@]} ${LINKER_FLAGS}"
    echo "clean:
	rm -f ${outfiles[@]} bin/$proj_name"
    echo ""
}

# Create variables
declare -a outfiles
proj_name="efitest"

# Prepare structure
mkdir -p obj bin

# Generate Makefile
for path in $INCLUDE_PATHS; do
    COMPILER_INCLUEDIRS="$COMPILER_INCLUEDIRS -I${path} "
done
for filename in $SOURCE_PATHS_C; do
    $(file_continue "$filename")
    makeentry_cfile "$filename" >> Makefile_body
done
for filename in $SOURCE_PATHS_CXX; do
    $(file_continue "$filename")
    makeentry_cppfile "$filename" >> Makefile_body
done
for filename in $SOURCE_PATHS_ASM; do
    $(file_continue "$filename")
    makeentry_sfile "$filename" >> Makefile_body
done
makeentry_misc > Makefile
cat Makefile_body >> Makefile
if [ -f "Makefile_cst" ] ; then
    echo >> Makefile
    cat Makefile_cst >> Makefile
fi
rm -f Makefile_body
