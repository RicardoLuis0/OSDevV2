#!/usr/bin/env python3

from buildsystem import *

lai_path="../lai/"

config={
    ".asm":nasm(
        #flags
        ["-f elf"]
    ),
    ".cpp":clang(
        #flags
        ["-std=c++17","-DDEBUG","-g","-Iinclude","-I\"include/stdc\"","-I\""+lai_path+"include\"","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-nostdinc++","-fno-rtti","-fno-exceptions","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],
        
        #c++
        True
    ),
    ".c":clang(
        #flags
        ["-std=c18","-DDEBUG","-g","-Iinclude","-I\"include/stdc\"","-I\""+lai_path+"include\"","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],
        
        #c++
        False
    ),
}

ld_lld=linker_ld_lld(
    #flags
    ["-T linker.ld","-Llib"],
    
    #libraries
    ["-lclang_builtins_i386","-llai_i386"],
    
    #link before other files
    ["\"build/obj/boot.o\"","\"build/obj/crti.o\"","\"misc/crtbegin.o\""],
    
    #link after other files
    ["\"misc/crtend.o\"","\"build/obj/crtn.o\""]
)

runners=get_files(
    #source folder
    "src",

    #obj output folder
    "build/obj",

    #extension <-> compiler association
    config,
    
    #don't add to linker list
    ["crti","crtn","boot"],
    
    #linker
    ld_lld
)

#run compilers
for runner in runners:
    runner.run()

#run linker
ld_lld.run(
    #final output file
    "build/kernel.bin"
)
