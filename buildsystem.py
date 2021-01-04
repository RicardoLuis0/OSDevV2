#!/usr/bin/env python3

import os
import sys
import subprocess
import json

gcc_path=""#you can leave this empty if gcc is in path

clang_path=""#you can leave this empty if clang is in path

ld_path=""#you can leave this empty if ld is in path

ld_lld_path=""#you can leave this empty if ld.lld is in path

ar_path=""#you can leave this empty if ar is in path

nasm_path=""#you can leave this empty if nasm is in path

do_rebuild=False

verbose=False

silent=False

def __mkdirs_allow_exist(dir):
    if not os.path.isdir(dir):
        os.makedirs(dir)

class compiler: #base class for compiler

    def needs_to_run(self,src,src_file,src_base,src_path,out,tmp):
        return True

    def calc_extraflags(self,src,src_file,src_base,src_path,out,tmp):
        return []

    def get_cmd(self,src,out,extraflags): #get command to run
        raise NotImplementedError

class linker: #linker base class

    def __init__(self,link_before,link_after,start_files,end_files):
        self.link_before=link_before
        self.link_after=link_after
        self.start_files=start_files #files to link before others, list
        self.end_files=end_files #files to link after others, list
        self.files=[] #files to link normally, list
    
    def add_file(self,file): #add file to linking list
        name=os.path.splitext(os.path.splitext(os.path.split(file)[1])[0])[0]
        if name in self.link_before:
            self.start_files=[file]+self.start_files
        elif name in self.link_after:
            self.end_files.append(file)
        else:
            self.files.append(file)
    
    def get_files(self):
        return self.start_files+self.files+self.end_files #get files to link
    
    def get_cmd(self,out): #get command to run
        raise NotImplementedError
    
    def run(self,out): #run linker
        cmd=self.get_cmd(out)
        if not silent:print("running "+cmd)
        if(subprocess.Popen(cmd,shell=True).wait()):
            if silent:print(" '"+cmd+"'")
            sys.exit("linking failed")

class linker_ld(linker):

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_ld,self).__init__(link_before,link_after,start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list

    def get_cmd(self,out): #get command to run
        cmd="ld" if not ld_path else "\""+ld_path+"ld\""
        args=(" ".join(self.flags))+" "+(" ".join(self.libs))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))
        return cmd+" "+args

class linker_ld_lld(linker): #llvm linker

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_ld_lld,self).__init__(link_before,link_after,start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list

    def get_cmd(self,out): #get command to run
        cmd="ld.lld" if not ld_lld_path else "\""+ld_lld_path+"ld.lld\""
        args=(" ".join(self.flags))+" "+(" ".join(self.libs))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))
        return cmd+" "+args

class linker_gcc_clang_base(linker):

    def __init__(self,compiler,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_gcc_clang_base,self).__init__(link_before,link_after,start_files,end_files)
        self.flags=flags #linker flags, list
        self.libs=libs #linker libraries, list
        self.compiler=compiler

    def get_cmd(self,out): #get command to run
        cmd=self.compiler
        args=(" ".join(self.flags))+" -o \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files))+" "+(" ".join(self.libs))
        return cmd+" "+args

class linker_gcc_c(linker_gcc_clang_base):

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_gcc_c,self).__init__("gcc" if not gcc_path else "\""+gcc_path+"gcc\"",flags,libs,link_before,link_after,start_files,end_files)

class linker_gcc_cpp(linker_gcc_clang_base):

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_gcc_cpp,self).__init__("g++" if not gcc_path else "\""+gcc_path+"g++\"",flags,libs,link_before,link_after,start_files,end_files)

class linker_clang_c(linker_gcc_clang_base):

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_clang_c,self).__init__("clang" if not gcc_path else "\""+gcc_path+"clang\"",flags,libs,link_before,link_after,start_files,end_files)

class linker_clang_cpp(linker_gcc_clang_base):

    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_clang_cpp,self).__init__("clang++" if not gcc_path else "\""+gcc_path+"clang++\"",flags,libs,link_before,link_after,start_files,end_files)


class linker_ar(linker): #just packages object files together
    def __init__(self,flags,libs,link_before,link_after,start_files,end_files):
        super(linker_ar,self).__init__(link_before,link_after,start_files,end_files)
        self.flags=flags #linker flags, list

    def get_cmd(self,out): #get command to run
        cmd="ar" if not ar_path else "\""+ar_path+"ar\""
        args=(" ".join(self.flags))+" \""+out+"\" "+(" ".join(self.start_files))+" "+(" ".join(self.files))+" "+(" ".join(self.end_files));
        return cmd+" "+args

class compiler_gcc_clang_base(compiler): #gcc compiler
    def __init__(self,defines,flags,incremental,compiler):
        self.flags=defines+flags #flags, list
        self.compiler=compiler
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
        cmd=self.compiler
        args=" -c "+(" ".join(self.flags))+" "+(" ".join(extraflags))+" \""+src+"\" -o \""+out+"\""
        return cmd+" "+args;

class gcc_c(compiler_gcc_clang_base):
    def __init__(self,defines,flags,incremental=True):
        super(gcc_c,self).__init__(defines,flags,incremental,"gcc" if not gcc_path else "\""+gcc_path+"gcc\"")

class gcc_cpp(compiler_gcc_clang_base):
    def __init__(self,defines,flags,incremental=True):
        super(gcc_cpp,self).__init__(defines,flags,incremental,"g++" if not gcc_path else "\""+gcc_path+"g++\"")

class clang_c(compiler_gcc_clang_base):
    def __init__(self,defines,flags,incremental=True):
        super(clang_c,self).__init__(defines,flags,incremental,"clang" if not clang_path else "\""+clang_path+"clang\"")

class clang_cpp(compiler_gcc_clang_base):
    def __init__(self,defines,flags,incremental=True):
        super(clang_cpp,self).__init__(defines,flags,incremental,"clang++" if not clang_path else "\""+clang_path+"clang++\"")

class nasm(compiler): #nasm compiler
    def __init__(self,defines,flags):
        self.flags=defines+flags #nasm flags

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
            sys.exit("compile of file '"+self.src+"' failed")
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

def get_files(folder,out_folder,tmp_folder,compiler_exts,nolink,linker):#must be relative folders, compiler_exts = dict of file extensions to compilers, ex: {".c":gcc_c_default,".cpp":gcc_cpp_default,".asm":nasm}
    __mkdirs_allow_exist(out_folder)
    __mkdirs_allow_exist(tmp_folder)
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
                    out=out_dir+"/"+file+".o"
                    __mkdirs_allow_exist(out_dir)
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
    
def parse_args():
    if "--clean" in sys.argv or "--rebuild" in sys.argv or "-r" in sys.argv:
        set_rebuild(True)

    if "--silent" in sys.argv or "-s" in sys.argv:
        set_silent_verbose(True,False)

    if "--verbose" in sys.argv or "-v" in sys.argv:
        set_silent_verbose(False,True)


compilers_c = {
    "gcc":gcc_c,
    "clang":clang_c
}
compilers_cpp = {
    "gcc":gcc_cpp,
    "clang":clang_cpp
}
compilers_asm = {
    "gcc":gcc_c,
    "clang":clang_c,
    "nasm":nasm
}
compilers_asm = {
    "gcc":gcc_c,
    "clang":clang_c,
    "nasm":nasm
}
linkers = {
    "gcc":linker_gcc_cpp,
    "clang":linker_clang_cpp,
    "ld":linker_ld,
    "ld.lld":linker_ld_lld,
    "ar":linker_ar
}


class builder:
    def get_compiler(self,comp_map,name):
        if name in comp_map:
            return comp_map[name]
        else:
            raise ValueError("'"+name+"' is not a valid compiler")
    def get_linker(self,link_map,name):
        if name in link_map:
            return link_map[name]
        else:
            raise ValueError("'"+name+"' is not a valid linker")
    
    def __init__(self,build_data):
        
        self.noarch=build_data["noarch"] if "noarch" in build_data else False
        
        working_folder=build_data["working_folder"]
        
        self.build_folder=working_folder if self.noarch else working_folder+"/"+os.name;
        
        if "project_ext" in build_data:
            self.ext=build_data["project_ext"]
        elif self.noarch:
            self.ext=""
        elif os.name == "nt":
            self.ext=".exe"
        else:
            self.ext=""
        
        self.project_name=build_data["project_name"]
        self.project_binary=build_data["project_binary"] if "project_binary" in build_data else self.project_name
        self.targets=build_data["targets"]
        self.binary_folder=build_data["binary_folder"] if "binary_folder" in build_data else self.build_folder
        self.default_target=build_data["default_target"] if "default_target" in build_data else None
        self.compiler_c=self.get_compiler(compilers_c,build_data["compiler_c"]) if "compiler_c" in build_data else gcc_c;
        self.compiler_cpp=self.get_compiler(compilers_cpp,build_data["compiler_cpp"]) if "compiler_cpp" in build_data else gcc_cpp;
        self.compiler_asm=self.get_compiler(compilers_asm,build_data["compiler_asm"]) if "compiler_asm" in build_data else gcc_c;
        self.linker=self.get_linker(linkers,build_data["linker"]) if "linker" in build_data else gcc_cpp;
        
    def run(self):
        

        all_target=False

        target_list=[];

        for arg in sys.argv[1:]:
            if not arg.startswith("--"):
                if arg in self.targets and ( ( not "nocompile" in self.targets[arg] ) or not self.targets[arg]["nocompile"] ) :
                    if not arg in target_list:
                        target_list+=[arg]
                elif arg == "all":
                    all_target=True
                elif arg == "default" and self.default_target:
                    if self.default_target == "all":
                        all_target=True
                    elif not self.default_target in target_list:
                        target_list+=[self.default_target]
                else:
                    sys.exit("\n\nInvalid build target '"+arg+"'\n\n")
            
        def build_target(target):
            print("\n---\n\nBuilding '"+target+"' in "+self.project_name+"\n\n---\n");
            
            sources=[]
            
            defines_all=[]
            defines_asm=[]
            defines_c_cpp=[]
            defines_c=[]
            defines_cpp=[]
            
            flags_all=[]
            flags_asm=[]
            flags_c_cpp=[]
            flags_c=[]
            flags_cpp=[]

            linker_flags=[]
            linker_libs=[]
            link_order_before=[]
            link_order_after=[]
            link_extra_before=[]
            link_extra_after=[]
            
            binary_folder=self.binary_folder+"/"+target
            
            nolink=[]
            
            def prepare_target(target):
                nonlocal sources,defines_all,defines_asm,defines_c_cpp,defines_c,defines_cpp,flags_all,flags_asm,flags_c_cpp,flags_c,flags_cpp,linker_flags,linker_libs,link_order_before,link_order_after,link_extra_before,link_extra_after,binary_folder,nolink
                target_data=self.targets[target]
                if "include" in target_data:
                    for subtarget in target_data["include"]:
                        prepare_target(subtarget)
                if "sources" in target_data:
                    sources+=target_data["sources"]
                    
                if "defines_all" in target_data:
                    defines_all+=target_data["defines_all"]
                if "defines_asm" in target_data:
                    defines_asm+=target_data["defines_asm"]
                if "defines_c_cpp" in target_data:
                    defines_c_cpp+=target_data["defines_c_cpp"]
                if "defines_c" in target_data:
                    defines_c+=target_data["defines_c"]
                if "defines_cpp" in target_data:
                    defines_cpp+=target_data["defines_cpp"]
                    
                if "flags_all" in target_data:
                    flags_all+=target_data["flags_all"]
                if "flags_asm" in target_data:
                    flags_asm+=target_data["flags_asm"]
                if "flags_c_cpp" in target_data:
                    flags_c_cpp+=target_data["flags_c_cpp"]
                if "flags_c" in target_data:
                    flags_c+=target_data["flags_c"]
                if "flags_cpp" in target_data:
                    flags_cpp+=target_data["flags_cpp"]
                    
                if "linker_flags" in target_data:
                    linker_flags+=target_data["linker_flags"]
                if "linker_libs" in target_data:
                    linker_libs+=target_data["linker_libs"]
                
                if "link_order_before" in target_data:
                    link_order_before+=target_data["link_order_before"]
                if "link_order_after" in target_data:
                    link_order_after+=target_data["link_order_after"]
                if "link_extra_before" in target_data:
                    link_extra_before+=target_data["link_extra_before"]
                if "link_extra_after" in target_data:
                    link_extra_after+=target_data["link_extra_after"]
                
                if "binary_folder_override" in target_data:
                    binary_folder=target_data["binary_folder_override"]
                    
                if "nolink" in target_data:
                    nolink+=target_data["nolink"]
            
            prepare_target(target)
            
            c_c=self.compiler_c(defines_all+defines_c_cpp+defines_c,flags_all+flags_c_cpp+flags_c)
            c_cpp=self.compiler_cpp(defines_all+defines_c_cpp+defines_cpp,flags_all+flags_c_cpp+flags_cpp)
            c_asm=self.compiler_asm(defines_all+defines_asm,flags_all+flags_asm)
            
            link=self.linker(
                linker_flags,
                linker_libs,
                link_order_before,
                link_order_after,
                link_extra_before,
                link_extra_after
            )
            
            get_files(
                #source folder
                sources,
                #obj output folder
                self.build_folder+"/"+target+"/obj",
                #build temp folder
                self.build_folder+"/"+target+"/tmp",
                #extension <-> compiler association
                {
                    ".cpp":c_cpp,
                    ".cc":c_cpp,
                    ".c":c_c,
                    ".S":c_asm,
                    ".asm":c_asm,
                },
                nolink,
                #linker
                link
            ).compile(binary_folder+"/"+self.project_binary+self.ext)

        if all_target or ( self.default_target=="all" and not target_list ) :
            for target,data in self.targets.items():
                if ( not "nocompile" in data ) or not data["nocompile"]:
                    build_target(target)
        elif target_list:
            for target in target_list:
                build_target(target)
        elif self.default_target:
            build_target(self.default_target)
        else:
            sys.exit("\n\nNo Build Targets Specified\n\n")

def builder_json(json_filename):
    with open(json_filename) as f:
        build_data=json.load(f)
        return builder(build_data)
