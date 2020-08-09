from buildsystem import *

lai_path="../lai/"

config_debug={
    ".asm":nasm(
        #flags
        ["-f elf"]
    ),
    ".c":clang(
        #flags
        [
        #defines
        "-DDEBUG",
        #other flags
        "-std=c18","-g","-I\""+lai_path+"include\"","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib",
        #"-nostdinc",
        "-Wall","-Werror=undef"],
        #c++
        False
    ),
}

ar=linker_ar(
    #flags
    ["rc"],
    
    #link before other files
    [],
    
    #link after other files
    []
)

runners=get_files([lai_path+"core",lai_path+"helpers"],lai_path+"obj",config_debug,[],ar)

#run compilers
for runner in runners:
    runner.run()

#run linker
ar.run(
    #final output file
    "lib/liblai_i386.a"
)
