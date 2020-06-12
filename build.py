#makesystem

import os;
import subprocess;

llvm_path="C:\\Program Files\\LLVM\\bin\\";
nasm_path="";

class compiler:

    def get_cmd(self,src,out,extraflags):
        raise "call of abstract method"

class linker:
    def __init__(self,start_files,end_files):
        self.start_files=start_files
        self.end_files=end_files
        self.files=[]
    def add_file(self,file):
        self.files.append(file)
    def get_files(self):
        return self.start_files+self.files+self.end_files
    def get_cmd(self,out):
        raise "call of abstract method"

class linker_ld_lld(linker):
    def __init__(self,flags,libs,start_files,end_files):
        super(linker_ld_lld,self).__init__(start_files,end_files)
        self.flags=flags
        self.libs=libs
    def get_cmd(self,out):
        return "\""+llvm_path+"ld.lld\" "+(" ".join(self.flags))+" "+(" ".join(self.libs))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))

class clang(compiler):
    def __init__(self,flags,cpp):
        self.flags=flags
        self.cpp=cpp

    def get_cmd(self,src,out,extraflags):
        cmd=" -c "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" \""+src+"\" -o \""+out+"\""
        if(self.cpp):
            return "\""+llvm_path+"clang++\" "+cmd
        else:
            return "\""+llvm_path+"clang\" "+cmd

class nasm(compiler):
    def __init__(self,flags):
        self.flags=flags

    def get_cmd(self,src,out,extraflags):
        return "\""+nasm_path+"nasm\" "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" "+src+" -o "+out
    

class compiler_runner:
    def __init__(self,compiler,linker,src,out,extraflags,nolink):
        self.compiler=compiler
        self.linker=linker
        self.src=src
        self.out=out
        self.extraflags=extraflags
        self.nolink=nolink
    
    def run(self):
        cmd=self.compiler.get_cmd(self.src,self.out,self.extraflags);
        print("running "+cmd);
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
            if ext in compiler_exts:
                if split[0] in nolink :
                    compiler_runners.append(compiler_runner(compiler_exts[ext],linker,"\""+path+os.path.sep+file+"\"","\""+out_folder+os.path.sep+split[0]+".o\"","",True))
                else:
                    compiler_runners.append(compiler_runner(compiler_exts[ext],linker,"\""+path+os.path.sep+file+"\"","\""+out_folder+os.path.sep+file+".o\"","",False))
                print("loading '"+ext+"' file "+file)
            else:
                print("skipping '"+ext+"' file "+file)
    return compiler_runners

config={
    ".asm":nasm(["-f elf"]),
    ".cpp":clang(["-std=c++17","-DDEBUG","-g","-Iinclude","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-nostdinc++","-fno-rtti","-fno-exceptions","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],True),
    ".c":clang(["-std=c18","-DDEBUG","-g","-Iinclude","--target=i686-pc-none-elf","-march=i686","-ffreestanding","-fno-builtin","-nostdlib","-nostdinc","-Wall","-Werror=implicit-function-declaration","-Werror=return-type"],False),
}

ld=linker_ld_lld(["-T linker.ld","-Llib"],["-lclang_builtins_i386"],["\"build\\obj\\boot.o\"","\"build\\obj\\crti.o\"","\"misc\\crtbegin.o\""],["\"misc\\crtend.o\"","\"build\\obj\\crtn.o\""])

runners=get_files("src","build\\obj",config,["crti","crtn","boot"],ld)
for runner in runners:
    runner.run()
ld_cmd=ld.get_cmd("build/kernel.bin")
print("running "+ld_cmd);
subprocess.Popen(ld_cmd).wait()