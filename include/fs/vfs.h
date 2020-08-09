#ifndef VFS_H_INCLUDED
#define VFS_H_INCLUDED

#include "util/string.h"
#include "util/vector.h"

#include "fs/fs.h"

namespace FS {
    
    class FolderHandle;
    
    class VirtualFileSystem {
    public:
        virtual FolderHandle * getRoot()=0;
        virtual ~VirtualFileSystem()=0;
    };
    
    class Handle {
    protected:
        VirtualFileSystem * fs;
        Util::String name;
    public:
        const enum FSHandleType_t{
            FSHANDLE_FOLDER,
            FSHANDLE_FILE,
        }type;
        Handle(VirtualFileSystem *,const Util::String &,FSHandleType_t);
        virtual ~Handle()=0;
    };
    
    class FileHandle : public Handle {
    public:
        FileHandle(VirtualFileSystem *,const Util::String &,FSHandleType_t);
        virtual void lock()=0;
        virtual bool try_lock()=0;
        virtual void unlock()=0;
        virtual size_t read(void * buf,size_t elem_size,size_t elem_count,size_t offset)=0;
        virtual size_t write(void * data,size_t elem_size,size_t elem_count,size_t offset)=0;
        virtual char getc(size_t offset)=0;
        virtual uint8_t getu(size_t offset)=0;
    };
    
    class FolderHandle : public Handle {
    public:
        FolderHandle(VirtualFileSystem *,const Util::String &,FSHandleType_t);
        virtual Util::Vector<Util::String> listFiles()=0;
        virtual Util::Vector<Util::String> listFolders()=0;
        virtual FolderHandle * parentFolder()=0;
        virtual FolderHandle * openFolder(const Util::String & name,bool create)=0;
        virtual FolderHandle * createFolder(const Util::String & name)=0;
        virtual FileHandle * openFile(const Util::String & name,bool create)=0;
        virtual FileHandle * createFile(const Util::String & name)=0;
        virtual bool removeFile(const Util::String & name)=0;
        virtual bool removeFolder(const Util::String & name)=0;
        virtual bool renameFile(const Util::String & name_old,const Util::String & name_new)=0;
        virtual bool renameFolder(const Util::String & name_old,const Util::String & name_new)=0;
    };
    
    namespace VFSRoot {
        void init(Util::UniquePtr<VirtualFileSystem> && rootfs);//most likely a RamFS
        FileHandle * resolveFilePath(FolderHandle * location,const Util::String &path,bool create=true,bool create_path=false);//if location is null, resolve only absolute paths
        FolderHandle * resolveFolderPath(FolderHandle * location,const Util::String &path,bool create=true,bool create_path=false);//if location is null, resolve only absolute paths
        FolderHandle * getRoot();
    };
    
}


#endif // VFS_H_INCLUDED
