#include "fs/vfs.h"
#include "util/tmp.h"

namespace FS {
    
    VirtualFileSystem::~VirtualFileSystem(){
    }
    
    Handle::Handle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):fs(f),name(n),type(t){
        
    }
    
    Handle::~Handle(){
        
    }
    
    FolderHandle::FolderHandle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):Handle(f,n,t){
        
    }
    
    FileHandle::FileHandle(VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):Handle(f,n,t){
        
    }
    
    static Util::UniquePtr<VirtualFileSystem> * vfs_root=nullptr;
    
    void VFSRoot::init(Util::UniquePtr<VirtualFileSystem> && root){
        if(vfs_root)delete vfs_root;
        vfs_root=new Util::UniquePtr<VirtualFileSystem>(TMP::move(root));
    }
    
    FileHandle * VFSRoot::resolveFilePath(FolderHandle * location,const Util::String &path_str,bool create,bool create_path){
        if(!vfs_root)return nullptr;
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
        if(!vfs_root)return nullptr;
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
        if(!vfs_root)return nullptr;
        return (*vfs_root)->getRoot();
    }
    
}
