#include "fs/vfs.h"
#include "util/tmp.h"

using namespace FS;

VirtualFileSystem::~VirtualFileSystem(){
}

FSHandle::FSHandle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):fs(f),name(n),type(t){
    
}

FolderHandle::FolderHandle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):FSHandle(f,n,t){
    
}

FileHandle::FileHandle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):FSHandle(f,n,t){
    
}

FSHandle::~FSHandle(){
    
}

VFSRoot::VFSRoot(Util::UniquePtr<VirtualFileSystem> && rootfs):root(TMP::move(rootfs)){
}

FileHandle * VFSRoot::resolveFilePath(FolderHandle * location,const Util::String &path_str,bool create,bool create_path){
    bool first_sep,last_sep;
    auto path=path_str.explode('/',first_sep,last_sep);
    if(last_sep)return nullptr;//can't open folder as file
    if(first_sep||location==nullptr){//ABSOLUTE
        location=getRoot();
    }
    size_t n=path.size()-1;
    for(size_t i=0;i<n;i++){
        location=location->openFolder(path[i],create_path);
        if(!location)return nullptr;
    }
    return location->openFile(path[n],create);
}

FolderHandle * VFSRoot::resolveFolderPath(FolderHandle * location,const Util::String &path_str,bool create,bool create_path){
    bool first_sep,last_sep;
    auto path=path_str.explode('/',first_sep,last_sep);
    if(first_sep||location==nullptr){//ABSOLUTE
        location=getRoot();
    }
    size_t n=path.size()-1;
    for(size_t i=0;i<n;i++){
        location=location->openFolder(path[i],create_path);
        if(!location)return nullptr;
    }
    return location->openFolder(path[n],create);;
}

FolderHandle * VFSRoot::getRoot(){
    return root->getRoot();
}
