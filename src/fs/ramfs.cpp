#include "fs/ramfs.h"

#include "klib.h"

namespace FS {
    
    RamFS::FileHandle::FileHandle(RamFS * f,const Util::String &n):FS::FileHandle(f,n,FSHANDLE_FILE){
    }
    
    void RamFS::FileHandle::lock(){
        spinlock.lock();
    }
    
    bool RamFS::FileHandle::try_lock(){
        return spinlock.try_lock();
    }
    
    void RamFS::FileHandle::unlock(){
        spinlock.release();
    }
    
    size_t RamFS::FileHandle::read(void * buf,size_t elem_size,size_t elem_count,size_t offset){
        uint32_t to_read=min(elem_size*elem_count,data.size()-offset);
        memcpy(buf,data.get()+offset,to_read);
        return to_read;
    }
    
    size_t RamFS::FileHandle::write(void * buf,size_t elem_size,size_t elem_count,size_t offset){
        uint32_t to_write=elem_size*elem_count;
        uint32_t needed=to_write+offset;
        if(needed<data.size()){
            data.resize(needed);
        }
        memcpy(data.get()+offset,buf,to_write);
        return to_write;
    }
    
    Util::Vector<Util::String> RamFS::FolderHandle::listFiles(){
        Util::Vector<Util::String> file_names;
        files.foreach_k([&file_names](char * k){
            file_names.push(k);
        });
        return file_names;
    }
    
    Util::Vector<Util::String> RamFS::FolderHandle::listFolders(){
        Util::Vector<Util::String> folder_names;
        folders.foreach_k([&folder_names](char * k){
            folder_names.push(k);
        });
        return folder_names;
    }
    
    RamFS::FolderHandle::FolderHandle(RamFS * f):FS::FolderHandle(f,"/",FSHANDLE_FOLDER),parent(nullptr){
    }
    
    RamFS::FolderHandle::FolderHandle(RamFS * f,const Util::String &n,FolderHandle * p):FS::FolderHandle(f,n,FSHANDLE_FOLDER),parent(p){
    }
    
    FS::FolderHandle * RamFS::FolderHandle::parentFolder(){
        return parent;
    }
    
    FS::FolderHandle * RamFS::FolderHandle::openFolder(const Util::String & name,bool create){
        auto p=folders.find(name.c_str());
        return p?p->get():create?createFolder(name):nullptr;
    }
    
    RamFS::FolderHandle * RamFS::FolderHandle::createFolder(const Util::String & name){
        if(folders.has(name.c_str())){
            return nullptr;
        }
        RamFS::FolderHandle * f=new RamFS::FolderHandle((RamFS*)fs,name,this);
        folders.set(name.c_str(),f);
        return f;
    }
    
    FS::FileHandle * RamFS::FolderHandle::openFile(const Util::String & name,bool create){
        auto p=files.find(name.c_str());
        return p?p->get():create?createFile(name):nullptr;
    }
    
    RamFS::FileHandle * RamFS::FolderHandle::createFile(const Util::String & name){
        if(files.has(name.c_str())){
            return nullptr;
        }
        RamFS::FileHandle * f=new RamFS::FileHandle((RamFS*)fs,name);
        files.set(name.c_str(),f);
        return f;
    }
    
    bool RamFS::FolderHandle::removeFolder(const Util::String & name){
        return folders.unset(name.c_str());
    }
    
    bool RamFS::FolderHandle::removeFile(const Util::String & name){
        auto p=files.find(name.c_str());
        if(!p){
            return false;
        }
        (*p)->lock();
        Util::UniquePtr<FS::FileHandle> temp(TMP::move(*p));
        files.unset(name.c_str());
        temp->unlock();
        return true;
    }
    
    bool RamFS::FolderHandle::renameFolder(const Util::String & name_old,const Util::String & name_new){
        auto p=folders.find(name_old.c_str());
        if(!p||folders.has(name_new.c_str())){
            return false;
        }
        Util::UniquePtr<FS::FolderHandle> temp(TMP::move(*p));
        folders.unset(name_old.c_str());
        folders.set(name_new.c_str(),TMP::move(temp));
        return true;
    }
    
    bool RamFS::FolderHandle::renameFile(const Util::String & name_old,const Util::String & name_new){
        auto p=files.find(name_old.c_str());
        if(!p||files.has(name_new.c_str())){
            return false;
        }
        auto * fp=p->get();
        fp->lock();
        Util::UniquePtr<FS::FileHandle> temp(TMP::move(*p));
        files.unset(name_old.c_str());
        files.set(name_new.c_str(),TMP::move(temp));
        fp->unlock();
        return true;
    }
    
    RamFS::RamFS():root(this){
    }
    
    RamFS::FolderHandle * RamFS::getRoot(){
        return &root;
    }
    
}
