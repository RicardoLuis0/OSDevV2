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
    };
    
    class FSHandle {
    protected:
        VirtualFileSystem * fs;
        Util::String path;
    public:
        const enum FSHandleType_t{
            FSHANDLE_FOLDER,
            FSHANDLE_FILE,
        }type;
        FSHandle(VirtualFileSystem *,const Util::String &,FSHandleType_t);
        virtual ~FSHandle()=0;
    };
    
    class FileHandle : public FSHandle {
    public:
        virtual size_t read(void * buf,size_t buf_len,size_t offset,size_t count)=0;
    };
    
    class FolderHandle : public FSHandle {
    public:
        virtual Util::Vector<Util::String> listFiles()=0;
        virtual Util::Vector<Util::String> listFolders()=0;
        virtual FolderHandle * parentFolder()=0;
        virtual FolderHandle * openFolder(const Util::String & name)=0;
        virtual FileHandle * openFile(const Util::String & name)=0;
        virtual bool removeFile(const Util::String & name)=0;
        virtual bool removeFolder(const Util::String & name)=0;
        virtual bool renameFile(const Util::String & name_old,const Util::String & name_new)=0;
        virtual bool renameFolder(const Util::String & name_old,const Util::String & name_new)=0;
    };
}


#endif // VFS_H_INCLUDED
