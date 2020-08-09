import os
import subprocess

clang_path=""#you can leave this empty if llvm is in path

ld_lld_path=""#you can leave this empty if llvm is in path

ar_path=""#you can leave this empty if ar is in path

nasm_path=""#you can leave this empty if nasm is in path

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
    
    def run(self,out): #run linker
        cmd=self.get_cmd(out)
        print("running "+cmd)
        if(subprocess.Popen(cmd,shell=True).wait()):
            raise "linking failed"

class linker_ld_lld(linker): #llvm linker on windows, might need to use plain ld on linux instead of this? not sure

    def __init__(self,flags,libs,start_files,end_files):
        super(linker_ld_lld,self).__init__(start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list

    def get_cmd(self,out): #get command to run
        cmd="ld.lld" if not ar_path else "\""+ar_path+"ld.lld\""
        args=(" ".join(self.flags))+" "+(" ".join(self.libs))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))
        return cmd+" "+args

class linker_ar(linker): #just packages object files together
    def __init__(self,flags,start_files,end_files):
        super(linker_ar,self).__init__(start_files,end_files)
        self.flags=flags #linker flags, list

    def get_cmd(self,out): #get command to run
        cmd="ar" if not ar_path else "\""+ar_path+"ar\""
        args=(" ".join(self.flags))+" \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files));
        return cmd+" "+args

class clang(compiler): #clang compiler
    def __init__(self,flags,cpp):
        self.flags=flags #clang flags, list
        self.cpp=cpp #whether to run clang or clang++

    def get_cmd(self,src,out,extraflags): #get command to run
        compiler="clang++" if self.cpp else "clang"
        cmd=compiler if not clang_path else "\""+clang_path+compiler+"\""
        args=" -c "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" \""+src+"\" -o \""+out+"\""
        return cmd+" "+args;

class nasm(compiler): #nasm compiler
    def __init__(self,flags):
        self.flags=flags #nasm flags

    def get_cmd(self,src,out,extraflags): #get command to run
        cmd="nasm" if not nasm_path else "\""+nasm_path+"nasm\""
        args=(" ".join(self.flags))+" "+(" ".join(extraflags))+" "+src+" -o "+out
        return cmd+" "+args
    

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
    if(type(folder)==list):
        for f in folder :
            compiler_runners+=get_files(f,out_folder,compiler_exts,nolink,linker)
    else:
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
                                "\""+path+"/"+file+"\"",
                                
                                #output file
                                "\""+out_folder+"/"+split[0]+".o\"", #don't add extension to .o if in nolink
                                
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
                                "\""+path+"/"+file+"\"",
                                
                                #output file
                                "\""+out_folder+"/"+file+".o\"",
                                
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
