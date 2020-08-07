#ifndef RAMFS_H_INCLUDED
#define RAMFS_H_INCLUDED

#include "fs/vfs.h"
#include <util/hash_table.h>

namespace FS {
    
    class RamFS : public VirtualFileSystem {
        
    public:
        class FolderHandle;
        
        class FileHandle : public FS::FileHandle {
        public:
            FileHandle(RamFS * fs,const Util::String &name);
            virtual void lock() override;
            virtual bool try_lock() override;
            virtual void unlock() override;
            virtual size_t read(void * buf,size_t elem_size,size_t elem_count,size_t offset) override;
            virtual size_t write(void * data,size_t elem_size,size_t elem_count,size_t offset) override;
        private:
            Util::Vector<uint8_t> data;
            Util::Spinlock spinlock;
        };
        
        class FolderHandle : public FS::FolderHandle {
        public:
            FolderHandle(RamFS * fs);//root constructor
            FolderHandle(RamFS * fs,const Util::String &name,FolderHandle * parent);//parent MUST NOT BE NULL
            virtual Util::Vector<Util::String> listFiles() override;
            virtual Util::Vector<Util::String> listFolders() override;
            virtual FS::FolderHandle * parentFolder() override;//parent is only null on the filesystem root
            virtual FS::FolderHandle * openFolder(const Util::String & name,bool create) override;//returns null if folder doesn't exist
            virtual RamFS::FolderHandle * createFolder(const Util::String & name) override;//if folder already exists returns null
            virtual FS::FileHandle * openFile(const Util::String & name,bool create) override;//if file doesn't exist and create is false returns null, if create is true, creates the file and returns it
            virtual RamFS::FileHandle * createFile(const Util::String & name) override;//if file already exists returns null
            virtual bool removeFolder(const Util::String & name) override;//if folder doesn't exist returns false
            virtual bool removeFile(const Util::String & name) override;//if file doesn't exist returns false
            virtual bool renameFolder(const Util::String & name_old,const Util::String & name_new) override;//if folder doesn't exist returns false
            virtual bool renameFile(const Util::String & name_old,const Util::String & name_new) override;//if file doesn't exist returns false
        private:
            friend class RamFS::FileHandle;
            Util::HashTable<Util::UniquePtr<FS::FolderHandle>> folders;
            Util::HashTable<Util::UniquePtr<FS::FileHandle>> files;
            FolderHandle * parent;
        };
        
        RamFS();
        
        virtual RamFS::FolderHandle * getRoot() override;
        
    private:
        RamFS::FolderHandle root;
        
    };
    
}


#endif // RAMFS_H_INCLUDED
