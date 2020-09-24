#include "fs/ramfs.h"

#include "klib.h"

#include <stdio.h>

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
        if(offset>data.size())return 0;
        uint32_t to_read=min(elem_size*elem_count,data.size()-offset);
        memcpy(buf,data.get()+offset,to_read);
        return to_read;
    }
    
    size_t RamFS::FileHandle::write(const void * buf,size_t elem_size,size_t elem_count,size_t offset){
        uint32_t to_write=elem_size*elem_count;
        uint32_t needed=to_write+offset;
        if(needed>data.size()){
            data.resize(needed);
        }
        memcpy(data.get()+offset,buf,to_write);
        return to_write;
    }
    
    size_t RamFS::FileHandle::putc(char c,size_t offset){
        return write(&c,1,1,offset);
    }
    
    char RamFS::FileHandle::getc(size_t offset){
        if(offset>data.size())return EOF;
        return *(char*)(data.get()+offset);
    }
    
    uint8_t RamFS::FileHandle::getu(size_t offset){
        if(offset>data.size())return EOF;
        return data[offset];
    }
    
    size_t RamFS::FileHandle::size(){
        return data.size();
    }
    
    void RamFS::FileHandle::clear(){
        data.resize(0);
    }
    
    Util::Vector<Util::String> RamFS::FolderHandle::listFiles(){
        Util::Vector<Util::String> file_names;
        files.foreach_k([&file_names](const char * k){
            file_names.push_back(k);
        });
        return file_names;
    }
    
    Util::Vector<Util::String> RamFS::FolderHandle::listFolders(){
        Util::Vector<Util::String> folder_names;
        folders.foreach_k([&folder_names](const char * k){
            folder_names.push_back(k);
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
        auto p=folders.find(name);
        return p?p->get():create?createFolder(name):nullptr;
    }
    
    RamFS::FolderHandle * RamFS::FolderHandle::createFolder(const Util::String & name){
        if(folders.has(name)){
            return nullptr;
        }
        RamFS::FolderHandle * f=new RamFS::FolderHandle((RamFS*)fs,name,this);
        folders.set(name,f);
        return f;
    }
    
    FS::FileHandle * RamFS::FolderHandle::openFile(const Util::String & name,bool create){
        auto p=files.find(name);
        return p?p->get():create?createFile(name):nullptr;
    }
    
    RamFS::FileHandle * RamFS::FolderHandle::createFile(const Util::String & name){
        if(files.has(name)){
            return nullptr;
        }
        RamFS::FileHandle * f=new RamFS::FileHandle((RamFS*)fs,name);
        files.set(name,f);
        return f;
    }
    
    bool RamFS::FolderHandle::removeFolder(const Util::String & name){
        return folders.unset(name);
    }
    
    bool RamFS::FolderHandle::removeFile(const Util::String & name){
        auto p=files.find(name);
        if(!p){
            return false;
        }
        (*p)->lock();
        Util::UniquePtr<FS::FileHandle> temp(TMP::move(*p));
        files.unset(name);
        temp->unlock();
        return true;
    }
    
    bool RamFS::FolderHandle::renameFolder(const Util::String & name_old,const Util::String & name_new){
        auto p=folders.find(name_old);
        if(!p||folders.has(name_new)){
            return false;
        }
        Util::UniquePtr<FS::FolderHandle> temp(TMP::move(*p));
        folders.unset(name_old);
        folders.set(name_new,TMP::move(temp));
        return true;
    }
    
    bool RamFS::FolderHandle::renameFile(const Util::String & name_old,const Util::String & name_new){
        auto p=files.find(name_old);
        if(!p||files.has(name_new)){
            return false;
        }
        auto * fp=p->get();
        fp->lock();
        Util::UniquePtr<FS::FileHandle> temp(TMP::move(*p));
        files.unset(name_old);
        files.set(name_new,TMP::move(temp));
        fp->unlock();
        return true;
    }
    
    Util::UniquePtr<FS::FileHandle> RamFS::FolderHandle::releaseFile(const Util::String & name){
        auto p=files.find(name);
        if(!p)return nullptr;
        (*p)->lock();
        Util::UniquePtr<FS::FileHandle> tmp(p->release());
        files.unset(name);
        tmp->unlock();
        return tmp;
    }
    
    bool RamFS::FolderHandle::insertFile(Util::UniquePtr<FS::FileHandle> file,const Util::String & name){
        if(file->fs!=fs||files.has(name))return false;
        files.set(name,TMP::move(file));
        return true;
    }
    
    Util::UniquePtr<FS::FolderHandle> RamFS::FolderHandle::releaseFolder(const Util::String & name){
        auto p=folders.find(name);
        if(!p)return nullptr;
        Util::UniquePtr<FS::FolderHandle> tmp(p->release());
        folders.unset(name);
        return tmp;
    }
    
    bool RamFS::FolderHandle::insertFolder(Util::UniquePtr<FS::FolderHandle> folder,const Util::String & name){
        if(folder->fs!=fs||folders.has(name))return false;
        folders.set(name,TMP::move(folder));
        return true;
    }
    
    RamFS::RamFS():root(this){
    }
    
    RamFS::FolderHandle * RamFS::getRoot(){
        return &root;
    }
    
}
