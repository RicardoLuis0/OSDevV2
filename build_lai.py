#!/usr/bin/env python3

from buildsystem import *

lai_path="../lai/"

defines_c=[
    "-DDEBUG"
]

flags_c=[
    "-std=c18","-g","-I\""+lai_path+"include\"","-I\"include/stdc\"","--target=i686-pc-none-elf",
    "-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-Wall","-Werror=undef"
]


flags_asm=[
    "-f elf"
]

get_files(
    #source folders
    [lai_path+"core",lai_path+"helpers"],
    #obj output folder
    lai_path+"build/obj",
    #build temp folder
    lai_path+"build/tmp",
    #extension <-> compiler association
    {
        ".asm":nasm(flags_asm),
        ".c":clang_c(defines_c,flags_c),
    },
    #files not to link automatically
    [],
    #linker
    linker_ar(
        #flags
        ["rc"],
        #link before other files
        [],
        #link after other files
        []
    )
).compile("lib/liblai_i386.a")

