#include "fs/vfs.h"
#include "util/tmp.h"

using namespace FS;

FSHandle::FSHandle(VirtualFileSystem * f,const Util::String & p,FSHandleType_t t):fs(f),path(p),type(t){
    
}

FSHandle::~FSHandle(){
    
}


