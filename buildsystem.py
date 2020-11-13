#!/usr/bin/env python3

import os
import subprocess

clang_path=""#you can leave this empty if llvm is in path

ld_lld_path=""#you can leave this empty if llvm is in path

ar_path=""#you can leave this empty if ar is in path

nasm_path=""#you can leave this empty if nasm is in path

do_rebuild=False

verbose=False

silent=False

class compiler: #base class for compiler

    def needs_to_run(self,src,src_file,src_base,src_path,out,tmp):
        return True

    def calc_extraflags(self,src,src_file,src_base,src_path,out,tmp):
        return []

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
        if not silent:print("running "+cmd)
        if(subprocess.Popen(cmd,shell=True).wait()):
            if silent:print(" '"+cmd+"'")
            raise "linking failed"

class linker_ld_lld(linker): #llvm linker on windows, might need to use plain ld on linux instead of this? not sure

    def __init__(self,flags,libs,start_files,end_files):
        super(linker_ld_lld,self).__init__(start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list

    def get_cmd(self,out): #get command to run
        cmd="ld.lld" if not ld_lld_path else "\""+ld_lld_path+"ld.lld\""
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

class clang_base(compiler): #clang compiler
    def __init__(self,defines,flags,incremental,cpp):
        self.defines=defines
        self.flags=defines+flags #clang flags, list
        self.cpp=cpp #whether to run clang or clang++
        self.incremental=incremental

    def calc_extraflags(self,src,src_file,src_base,src_path,out,tmp):
        out_r=os.path.relpath(src_path,src_base)
        if out_r==".":
            out_r=""
        out_f=tmp+"/"+out_r
        out_d=out_f+"/"+src_file+".d"
        try:
            os.makedirs(out_f)
        except FileExistsError:
            pass
        return ["-MD -MF \""+out_d+"\""]

    def needs_to_run(self,src,src_file,src_base,src_path,out,tmp):
        if do_rebuild or not self.incremental:
            return True
        out_d=tmp+"/"+os.path.relpath(src,src_base)+".d"
        try:
            ctime=os.path.getmtime(out)
            flist=[]
            with open(out_d,"r") as df:
                str=" ".join([ line.rstrip("\\") for line in df.read().splitlines(False) ])
                flist=str[str.find(":")+1:].split()
            for f in flist:
                ftime=os.path.getmtime(f)
                if ftime>ctime :return True
            return False
        except (OSError,IOError): #couldn't open/read file(s)
            return True
        
    def get_cmd(self,src,out,extraflags): #get command to run
        compiler="clang++" if self.cpp else "clang"
        cmd=compiler if not clang_path else "\""+clang_path+compiler+"\""
        args=" -c "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" \""+src+"\" -o \""+out+"\""
        return cmd+" "+args;

class clang_cpp(clang_base):
    def __init__(self,defines,flags,incremental=True):
        clang_base.__init__(self,defines,flags,incremental,True)

class clang_c(clang_base):
    def __init__(self,defines,flags,incremental=True):
        clang_base.__init__(self,defines,flags,incremental,False)

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
        if not silent:print("running "+cmd)
        if (subprocess.Popen(cmd,shell=True).wait()):
            if silent:print(" '"+cmd+"'")
            raise "compile of file "+self.get_filename()+" failed"
        elif not self.nolink:
            self.linker.add_file(self.out)

class compiler_runner_linkonly:
    def __init__(self,linker,out):
        self.linker=linker #linker to add file to
        self.out=out #output obj file
    
    def run(self): #run compiler
        self.linker.add_file(self.out)
    

class compile_list:
    def __init__(self,linker):
        self.runners=[]
        self.linker=linker

    def add(self,compiler,linker,src,out,extraflags,nolink):
        self.runners.append(compiler_runner(compiler,linker,src,out,extraflags,nolink))

    def add_linkonly(self,linker,out):
        self.runners.append(compiler_runner_linkonly(linker,out))

    def append(self,other):
        self.runners+=other.runners

    def compile(self,out):
        for runner in self.runners:
            runner.run()
        self.linker.run(out)

def get_files(folder,out_folder,tmp_folder,compiler_exts,nolink,linker):#must be relative folders, compiler_exts = dict of file extensions to compilers, ex: {".c":clang_c_default,".cpp":clang_cpp_default,".asm":nasm}
    complist=compile_list(linker)
    if type(folder)!=list:
        folder=[folder]
    for f in folder:
        for path,dirs,files in os.walk(f) :
            for file in files:
                split=os.path.splitext(file)
                ext=split[1]
                if ext in compiler_exts: #if extension has a compiler
                    nolink_b=split[0] in nolink
                    compiler=compiler_exts[ext];
                    src=path+"/"+file
                    out_dir=os.path.relpath(path,f)
                    if out_dir == ".":
                        out_dir=out_folder
                    else:
                        out_dir=out_folder+"/"+out_dir
                    if not os.path.isdir(out_dir):
                        os.makedirs(out_dir)
                    out=(out_folder if nolink_b else out_dir)+"/"+(split[0] if nolink_b else file)+".o"
                    if compiler.needs_to_run(src,file,f,path,out,tmp_folder):#src,src_file,src_base,src_path,out,tmp
                        if verbose and not silent:print("preparing '"+ext+"' file "+file)
                        complist.add(compiler,linker,src,out,compiler.calc_extraflags(src,file,f,path,out,tmp_folder),nolink_b)
                    elif not nolink_b:
                        if verbose and not silent:print("skipping '"+ext+"' file "+file)
                        complist.add_linkonly(linker,out)
                else:
                    if verbose and not silent:print("skipping '"+ext+"' file "+file)
    return complist
    
def set_rebuild(new_rebuild):
    global do_rebuild
    do_rebuild=new_rebuild

def set_silent_verbose(new_silent,new_verbose):
    global silent
    global verbose
    silent=new_silent
    verbose=new_verbose
    



