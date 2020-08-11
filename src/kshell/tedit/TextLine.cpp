#include "TextLine.h"
#include "IOLayer.h"

#include <stdlib.h>
#include <string.h>

#include "Util.h"

using namespace TEdit;

TextLine::TextLine() {
    _len=0;
    _alloc=8;//initial allocation is 8 chars
    buf=(char*)calloc(8,sizeof(char));//uses c allocation functions instead of new because of the need to use realloc
    if(buf==nullptr){
        IOLayer::exit_error("Out of memory");
    }
    buf[0]='\0';
}

TextLine::TextLine(char * data,size_t len,size_t alloc):buf(data),_len(len),_alloc(alloc){
}

TextLine::TextLine(TextLine && other):buf(other.buf),_len(other._len),_alloc(other._alloc){
    other.buf=nullptr;
    other._len=0;
    other._alloc=0;
}

TextLine& TextLine::operator=(TextLine && other){
    buf=other.buf;
    _len=other._len;
    _alloc=other._alloc;
    other.buf=nullptr;
    other._len=0;
    other._alloc=0;
    return *this;
}

TextLine::~TextLine() {
    if(buf){
        free(buf);
    }
}

size_t TextLine::len(){
    return _len;
}

TextLine * TextLine::split(size_t at){
    if(at>=_len){
        return new TextLine();
    }
    size_t sz=_len-at;
    char * nbuf=(char*)calloc(sz+1,sizeof(char));
    memcpy(nbuf,buf+at,sz+1);//copy including null terminator
    buf[at]='\0';
    _len=at;
    return new TextLine(nbuf,sz,sz+1);
}

void TextLine::append(const TextLine &other){
    if(other._len!=0){
        grow(_len+other._len);
        memcpy(buf+_len,other.buf,other._len);
        _len+=other._len;
    }
}

void TextLine::insert(char c,size_t at){
    if(at>_len){
        resize(at+1,true);
        buf[at]=c;
        return;
    }
    resize(_len+1,false);
    if(at!=(_len-1)){//if inserting to end of line, no need to move memory
        memmove(buf+at+1,buf+at,_len-at);
    }else{
        buf[at+1]='\0';
    }
    buf[at]=c;
}

void TextLine::erase(size_t at){
    if(at>=_len) return;
    memmove(buf+at,buf+at+1,_len-at);
    resize(_len-1,false);
}

void TextLine::resize(size_t new_size,bool move_terminator){
    if(new_size==_len)return;
    if((new_size+1)>_alloc){
        grow(new_size+1);
    }
    if(move_terminator&&new_size>_len){
        memset(buf+_len,' ',new_size-_len);
    }
    if(move_terminator||new_size<_len){
        buf[new_size]='\0';
    }
    _len=new_size;
}

void TextLine::grow(size_t to){
    if(to<=_alloc)return;//only allow growth
    size_t ns=max(_alloc*2,to);//double or grow to necessary, whichever is largest
    buf=(char*)realloc(buf,sizeof(char)*ns);
    if(!buf){
        IOLayer::exit_error("REALLOC FAILED");
    }
    _alloc=ns;
}

void TextLine::trim(){
    if(_alloc>(_len+1)){
        buf=(char*)realloc(buf,_len+1);
        if(!buf){
            IOLayer::exit_error("REALLOC FAILED");
        }
        _alloc=_len+1;
    }
}

const char * TextLine::get(){
    return buf;
}


