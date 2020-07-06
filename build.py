#makesystem

import os
import subprocess

lai_path="..\\lai\\"

llvm_path="C:\\Program Files\\LLVM\\bin\\"  #you can leave this empty if llvm is in path

nasm_path=""                                #you can leave this empty if nasm is in path

class compiler: #base class for compiler

    def get_cmd(self,src,out,extraflags): #get command to run
        raise "call of abstract method"

class linker: #linker base class

    def __init__(self,start_files,end_files):
        self.start_files=start_files #files to link before others, list
        self.end_files=end_files #files to link after others, list
        self.files=[] #files to link normally, list

    def add_file(self,file): #add file to linking list
        self.files.append(file)

    def get_files(self):
        return self.start_files+self.files+self.end_files #get files to link

    def get_cmd(self,out): #get command to run
        raise "call of abstract method"

class linker_ld_lld(linker): #llvm linker on windows, might need to use plain ld on linux instead of this? not sure

    def __init__(self,flags,libs,start_files,end_files):
        super(linker_ld_lld,self).__init__(start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list

    def get_cmd(self,out): #get command to run
        return "\""+llvm_path+"ld.lld\" "+(" ".join(self.flags))+" "+(" ".join(self.libs))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))

class clang(compiler): #clang compiler
    def __init__(self,flags,cpp):
        self.flags=flags #clang flags, list
        self.cpp=cpp #whether to run clang or clang++

    def get_cmd(self,src,out,extraflags): #get command to run
        cmd=" -c "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" \""+src+"\" -o \""+out+"\""
        if(self.cpp):
            return "\""+llvm_path+"clang++\" "+cmd
        else:
            return "\""+llvm_path+"clang\" "+cmd

class nasm(compiler): #nasm compiler
    def __init__(self,flags):
        self.flags=flags #nasm flags

    def get_cmd(self,src,out,extraflags): #get command to run
        return "\""+nasm_path+"nasm\" "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" "+src+" -o "+out
    

class compiler_runner: #runs a compiler for a file
    def __init__(self,compiler,linker,src,out,extraflags,nolink):
        self.compiler=compiler #compiler to run
        self.linker=linker #linker to add file to
        self.src=src #input source file
        self.out=out #output obj file
        self.extraflags=extraflags #extra flags, currently not used?
        self.nolink=nolink #add file to linker or not
    
    def run(self): #run compiler
        cmd=self.compiler.get_cmd(self.src,self.out,self.extraflags)
        print("running "+cmd)
        if (subprocess.Popen(cmd,shell=True).wait()):
            raise "compile of file "+self.get_filename()+" failed"
        elif not self.nolink:
            self.linker.add_file(self.out)

def get_files(folder,out_folder,compiler_exts,nolink,linker):#must be relative folders, compiler_exts = dict of file extensions to compilers, ex: {".c":clang_c_default,".cpp":clang_cpp_default,".asm":nasm}
    compiler_runners=[]
    for path,dirs,files in os.walk(folder) :
        for file in files:
            split=os.path.splitext(file)
            ext=split[1]
            if ext in compiler_exts: #if extension has a compiler
                if split[0] in nolink : #don't link file
                    compiler_runners.append(
                        compiler_runner(
                            #compiler for extension
                            compiler_exts[ext],
                            
                            #linker
                            linker,
                            
                            #source file
                            "\""+path+os.path.sep+file+"\"",
                            
                            #output file
                            "\""+out_folder+os.path.sep+split[0]+".o\"", #don't add extension to .o if in nolink
                            
                            #extra flags
                            [],
                            
                            #nolink
                            True
                        )
                    ) 
                else:
                    compiler_runners.append(
                        compiler_runner(
                            #compiler for extension
                            compiler_exts[ext],
                            
                            #linker
                            linker,
                            
                            #source file
                            "\""+path+os.path.sep+file+"\"",
                            
                            #output file
                            "\""+out_folder+os.path.sep+file+".o\"",
                            
                            #extra flags
                            [],
                            
                            #nolink
                            False
                        )
                    )
                print("preparing '"+ext+"' file "+file)
            else:
                print("skipping '"+ext+"' file "+file)
    return compiler_runners

config={
    ".asm":nasm(
        #flags
        ["-f elf"]
    ),
    ".cpp":clang(
        #flags
        ["-std=c++17","-DDEBUG","-g","-Iinclude","-I\"include/stdc\"","-I\""+lai_path+"\include\"","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-nostdinc++","-fno-rtti","-fno-exceptions","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],
        
        #c++
        True
    ),
    ".c":clang(
        #flags
        ["-std=c18","-DDEBUG","-g","-Iinclude","-I\"include/stdc\"","-I\""+lai_path+"\include\"","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],
        
        #c++
        False
    ),
}

ld=linker_ld_lld(
    #flags
    ["-T linker.ld","-Llib"],
    
    #libraries
    ["-lclang_builtins_i386","-llai"],
    
    #link before other files
    ["\"build\\obj\\boot.o\"","\"build\\obj\\crti.o\"","\"misc\\crtbegin.o\""],
    
    #link after other files
    ["\"misc\\crtend.o\"","\"build\\obj\\crtn.o\""]
)

runners=get_files(
    #source folder
    "src",

    #obj output folder
    "build\\obj",

    #extension <-> compiler association
    config,
    
    #don't add to linker list
    ["crti","crtn","boot"],
    
    #linker
    ld
)

#run compilers
for runner in runners:
    runner.run()

#run linker
ld_cmd=ld.get_cmd(
    #final output file
    "build/kernel.bin"
)
print("running "+ld_cmd)
subprocess.Popen(ld_cmd).wait()
#end!