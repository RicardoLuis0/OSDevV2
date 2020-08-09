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
    
    FileHandle * VFSRoot::resolveFilePathCreate(FolderHandle * location,const Util::String &path_str,bool create_path){
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
        return location->createFile(path[n]);
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
        return location->openFolder(path[n],create);
    }
    
    FolderHandle * VFSRoot::resolveFolderPathCreate(FolderHandle * location,const Util::String &path_str,bool create_path){
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
        return location->createFolder(path[n]);
    }
    
    FolderHandle * VFSRoot::getRoot(){
        if(!vfs_root)return nullptr;
        return (*vfs_root)->getRoot();
    }
    
    bool VFSRoot::removeFile(FolderHandle * location,const Util::String &path_str){
        if(!vfs_root)return false;
        bool first_sep,last_sep;
        auto path=path_str.explode('/',first_sep,last_sep);
        if(first_sep||location==nullptr){//ABSOLUTE
            location=getRoot();
        }
        size_t n=path.size()-1;
        for(size_t i=0;i<n;i++){
            location=location->openFolder(path[i],false);
            if(!location)return false;
        }
        return location->removeFile(path[n]);
    }
    
    bool VFSRoot::removeFolder(FolderHandle * location,const Util::String &path_str){
        if(!vfs_root)return false;
        bool first_sep,last_sep;
        auto path=path_str.explode('/',first_sep,last_sep);
        if(first_sep||location==nullptr){//ABSOLUTE
            location=getRoot();
        }
        size_t n=path.size()-1;
        for(size_t i=0;i<n;i++){
            location=location->openFolder(path[i],false);
            if(!location)return false;
        }
        return location->removeFolder(path[n]);
    }
    
    bool VFSRoot::renameFile(FolderHandle * location,const Util::String &path_str,const Util::String &new_name){
        if(!vfs_root)return false;
        bool first_sep,last_sep;
        auto path=path_str.explode('/',first_sep,last_sep);
        if(first_sep||location==nullptr){//ABSOLUTE
            location=getRoot();
        }
        size_t n=path.size()-1;
        for(size_t i=0;i<n;i++){
            location=location->openFolder(path[i],false);
            if(!location)return false;
        }
        return location->renameFile(path[n],new_name);
    }
    
    bool VFSRoot::renameFolder(FolderHandle * location,const Util::String &path_str,const Util::String &new_name){
        if(!vfs_root)return false;
        bool first_sep,last_sep;
        auto path=path_str.explode('/',first_sep,last_sep);
        if(first_sep||location==nullptr){//ABSOLUTE
            location=getRoot();
        }
        size_t n=path.size()-1;
        for(size_t i=0;i<n;i++){
            location=location->openFolder(path[i],false);
            if(!location)return false;
        }
        return location->renameFolder(path[n],new_name);
    }
    
}
