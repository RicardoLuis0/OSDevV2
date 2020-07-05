#include "util/string.h"
#include "util/tmp.h"
#include <string.h>

using namespace Util;


String::String(UniquePtr<char> && d,size_t l){
    data=TMP::move(d);
    len=l;
}

String::String() {
    data=strdup("");
}

String::String(const char * other){
    data=strdup(other);
    len=strlen(other);
}

String::String(const String& other){
    data=strdup(other.data);
    len=other.len;
}

String::String(String && other){
    data=TMP::move(other.data);
    len=other.len;
}

String::~String() {
}

size_t String::size(){
    return len;
}

String String::operator+(const char * other) const{
    size_t l2=strlen(other);
    char * tmp=(char*)calloc(len+l2+1,sizeof(char));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other,l2);
    return String(tmp,len+l2);
}

String String::operator+(const String & other) const{
    char * tmp=(char*)calloc(len+other.len+1,sizeof(char));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other.data,other.len);
    return String(tmp,len+other.len);
}

String& String::operator+=(const char * other){
    size_t l2=strlen(other);
    char * tmp=(char*)calloc(len+l2+1,sizeof(char));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other,l2);
    len=len+l2;
    data=tmp;
    return *this;
}

String& String::operator+=(const String & other){
    char * tmp=(char*)calloc(len+other.len+1,sizeof(char));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other.data,other.len);
    len=len+other.len;
    data=tmp;
    return *this;
}

char& String::operator[](size_t i){
    if(i<len){
        return data[i];
    }else{
        k_abort_s("accessing invalid string offset");
    }
}

char String::operator[](size_t i)const{
    if(i<len){
        return data[i];
    }else{
        k_abort_s("accessing invalid string offset");
    }
}

String& String::operator=(const char * other){
    data=strdup(other);
    len=strlen(other);
    return *this;
}

String& String::operator=(const String & other){
    data=strdup(other.data);
    len=other.len;
    return *this;
}

String& String::operator=(String && other){
    data=strdup(other.data);
    len=other.len;
    return *this;
}


