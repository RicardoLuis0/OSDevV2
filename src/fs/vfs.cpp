#include "fs/vfs.h"
#include "util/tmp.h"

namespace FS {
    
    VirtualFileSystem::~VirtualFileSystem(){
    }
    
    Handle::Handle(const VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):name(n),fs(f),type(t){
        
    }
    
    Handle::~Handle(){
        
    }
    
    FolderHandle::FolderHandle(const VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):Handle(f,n,t){
        
    }
    
    FileHandle::FileHandle(const VirtualFileSystem * f,const Util::String & n,FSHandleType_t t):Handle(f,n,t){
        
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
    
    static void findFolder(const Util::Vector<Util::String> & path,FolderHandle * &folder){
        size_t n=path.size()-1;
        for(size_t i=0;i<n;i++){
            if(path[i]==".."){
                folder=folder->parentFolder();
            }else if(path[i]!="."){
                folder=folder->openFolder(path[i],false);
            }
            if(!folder)break;
        }
    }
    
    static bool moveFile_impl(FolderHandle * from,FolderHandle * to,const Util::String &oldname,const Util::String &newname){
        if(from&&to){
            if(from==to){
                return from->renameFile(oldname,newname);
            }else{
                if(from->fs==to->fs){
                    Util::UniquePtr<FileHandle> f=from->releaseFile(oldname);
                    to->insertFile(TMP::move(f),newname);
                    return true;
                }else{
                    //TODO cross-filesystem move, would be copy+remove, but no copy yet
                }
            }
        }
        return false;
    }
    
    bool VFSRoot::moveFile(FolderHandle * location,const Util::String &path_str,const Util::String &new_path_str){
        if(!vfs_root)return false;
        
        bool first_sep,last_sep;
        
        auto path_lhs=path_str.explode('/',first_sep,last_sep);
        FolderHandle * location_lhs=(first_sep||location==nullptr)?getRoot():location;
        
        auto path_rhs=new_path_str.explode('/',first_sep,last_sep);
        FolderHandle * location_rhs=(first_sep||location==nullptr)?getRoot():location;;
        
        findFolder(path_lhs,location_lhs);
        findFolder(path_rhs,location_rhs);
        
        return moveFile_impl(location_lhs,location_rhs,path_lhs[path_lhs.size()-1],path_rhs[path_rhs.size()-1]);
    }
    
    static bool moveFolder_impl(FolderHandle * from,FolderHandle * to,const Util::String &oldname,const Util::String &newname){
        if(from&&to){
            if(from==to){
                return from->renameFolder(oldname,newname);
            }else{
                if(from->fs==to->fs){
                    Util::UniquePtr<FolderHandle> f=from->releaseFolder(oldname);
                    to->insertFolder(TMP::move(f),newname);
                    return true;
                }else{
                    //TODO cross-filesystem move, would be copy+remove, but no copy yet
                }
            }
        }
        return false;
    }
    
    bool VFSRoot::moveFolder(FolderHandle * location,const Util::String &path_str,const Util::String &new_path_str){
        if(!vfs_root)return false;
        
        bool first_sep,last_sep;
        
        auto path_lhs=path_str.explode('/',first_sep,last_sep);
        FolderHandle * location_lhs=(first_sep||location==nullptr)?getRoot():location;
        
        auto path_rhs=new_path_str.explode('/',first_sep,last_sep);
        FolderHandle * location_rhs=(first_sep||location==nullptr)?getRoot():location;;
        
        findFolder(path_lhs,location_lhs);
        findFolder(path_rhs,location_rhs);
        
        return moveFolder_impl(location_lhs,location_rhs,path_lhs[path_lhs.size()-1],path_rhs[path_rhs.size()-1]);
    }
    
}
