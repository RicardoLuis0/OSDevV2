#include "Text.h"

#include <stdlib.h>
#include <string.h>

using namespace TEdit;

Text::Text(Text && other):lines(other.lines),_alloc(other._alloc){
    other.lines=nullptr;
    other._alloc=0;
}

Text& Text::operator=(Text && other){
    lines=other.lines;
    _alloc=other._alloc;
    other.lines=nullptr;
    other._alloc=0;
    return *this;
}

Text::Text() {
    //lines is a c-allocated pointer array to c++-allocated pointers
    _alloc=0;
    lines=(TextLine**)calloc(0,sizeof(TextLine*));
}

Text::~Text() {
    for(size_t i=0;i<_alloc;i++){
        delete lines[i];
    }
    free(lines);
}

TextLine& Text::get(size_t x){
    if(x>=_alloc){
        grow(x+1);
    }
    return *lines[x];
}

void Text::insert(size_t x,TextLine * line){
    if(x>=_alloc){
        grow(x+1,x,line);
    }else{
        lines=(TextLine**)realloc(lines,sizeof(TextLine*)*(_alloc+1));
        memmove(lines+x+1,lines+x,sizeof(TextLine*)*(_alloc-x));
        _alloc++;
        lines[x]=line;
    }
}

void Text::erase(size_t x){
    if(x<_alloc){
        delete lines[x];
        memmove(lines+x,lines+x+1,sizeof(TextLine*)*((_alloc-x)-1));
        lines=(TextLine**)realloc(lines,sizeof(TextLine*)*(_alloc-1));
        _alloc--;
    }
}

void Text::grow(size_t to,size_t n,TextLine * p){
    if(to<=_alloc)return;
    lines=(TextLine**)realloc(lines,sizeof(TextLine*)*to);
    for(size_t i=_alloc;i<to;i++){
        if(i==n){
            lines[i]=p;
        }else{
            lines[i]=new TextLine();
        }
    }
    _alloc=to;
}

void Text::grow(size_t to){
    if(to<=_alloc)return;
    lines=(TextLine**)realloc(lines,sizeof(TextLine*)*to);
    for(size_t i=_alloc;i<to;i++){
        lines[i]=new TextLine();//not particularly performant but _should_ befine
    }
    _alloc=to;
}

size_t Text::size(){
    return _alloc;
}

