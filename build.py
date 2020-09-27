#!/usr/bin/env python3

from buildsystem import *
import sys

if "--rebuild" in sys.argv or "-r" in sys.argv:
    set_rebuild(True)

if "--silent" in sys.argv or "-s" in sys.argv:
    set_silent_verbose(True,False)

if "--verbose" in sys.argv or "-v" in sys.argv:
    set_silent_verbose(False,True)

lai_path="../lai/"

defines_c_cpp=[
    "-DDEBUG"
]

flags_c_cpp=[
    "-g","-Iinclude","-I\"include/stdc\"","-I\""+lai_path+"include\"",
    "--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib",
    "-nostdinc","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"
]

flags_c=flags_c_cpp+[
    "-std=c18","-Werror=implicit-function-declaration"
]

flags_cpp=flags_c_cpp+[
    "-std=c++20","-nostdinc++","-fno-rtti","-fno-exceptions","-Wshadow"
]

flags_asm=[
    "-f elf"
]

get_files(
    #source folder
    "src",
    #obj output folder
    "build/obj",
    #build temp folder
    "build/tmp",
    #extension <-> compiler association
    {
        ".asm":nasm(flags_asm),
        ".cpp":clang_cpp(defines_c_cpp,flags_cpp),
        ".c":clang_c(defines_c_cpp,flags_c),
    },
    #files not to link automatically
    ["crti","crtn","boot"],
    #linker
    linker_ld_lld(
        #flags
        ["-T linker.ld","-Llib"],
        #libraries
        ["-lclang_builtins_i386","-llai_i386"],
        #link before other files
        ["\"build/obj/boot.o\"","\"build/obj/crti.o\"","\"misc/crtbegin.o\""],
        #link after other files
        ["\"misc/crtend.o\"","\"build/obj/crtn.o\""]
    )
).compile("build/kernel.bin")
