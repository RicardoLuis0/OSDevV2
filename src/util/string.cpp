#include "util/string.h"
#include "util/tmp.h"
#include <string.h>
#include <ctype.h>

using namespace Util;


String::String(UniquePtr<char> && d,size_t l){
    data=TMP::move(d);
    len=l;
}

String::String() {
    data=strdup("");
    len=0;
}

String::String(const char * other){
    data=strdup(other);
    len=strlen(data);
}

String::String(const char * other,size_t n){
    data=strndup(other,n);
    len=strlen(data);
}

String::String(const String& other){
    data=strdup(other.data);
    len=other.len;
}

String::String(String && other){
    data=strdup("");//free previous buffer
    data.swap(other.data);//leave other as empty string
    len=other.len;
    other.len=0;
}

String::~String() {
}

size_t String::size() const{
    return len;
}

String String::operator+(const char * other) const{
    size_t l2=strlen(other);
    char * tmp=reinterpret_cast<char*>(calloc(len+l2+1,sizeof(char)));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other,l2);
    return String(tmp,len+l2);
}

String String::operator+(const String & other) const{
    UniquePtr<char> tmp=reinterpret_cast<char*>(calloc(len+other.len+1,sizeof(char)));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other.data,other.len);
    return String(TMP::move(tmp),len+other.len);
}

String String::operator+(char c) const {
    UniquePtr<char> tmp=reinterpret_cast<char*>(calloc(len+2,sizeof(char)));
    memcpy(tmp,data,len);
    tmp[len]=c;
    tmp[len+1]='\0';
    return String(TMP::move(tmp),len+1);
}

String& String::operator+=(const char * other){
    size_t l2=strlen(other);
    char * tmp=reinterpret_cast<char*>(calloc(len+l2+1,sizeof(char)));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other,l2);
    len=len+l2;
    data=tmp;
    return *this;
}

String& String::operator+=(char c) {
    char * tmp=data.release();
    tmp=reinterpret_cast<char*>(realloc(tmp,len+2));
    tmp[len]=c;
    tmp[len+1]='\0';
    data=tmp;
    len++;
    return *this;
}

String& String::operator+=(const String & other){
    char * tmp=reinterpret_cast<char*>(calloc(len+other.len+1,sizeof(char)));
    memcpy(tmp,data,len);
    memcpy(tmp+len,other.data,other.len);
    len=len+other.len;
    data=tmp;
    return *this;
}

char& String::operator[](size_t i){
    if(i<=len){//include null terminator
        return data[i];
    }else{
        k_abort_s("accessing invalid string offset");
    }
}

char String::operator[](size_t i) const{
    if(i<=len){//include null terminator
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
    data=strdup("");//free previous buffer
    data.swap(other.data);//leave other as empty string
    len=other.len;
    other.len=0;
    return *this;
}


bool String::operator==(const char * other) const{
    return k_strcmp_bool(data,other);
}

bool String::operator==(const String & other) const{
    return k_strcmp_bool(data,other.data);
}

String::operator const char *() const{
    return data;
}

const char * String::c_str() const{
    return data.get();
}

Vector<String> String::explode(char separator,bool &has_first_separator,bool &has_last_separator) const{
    Vector<String> vec;
    if(len==0){
        has_first_separator=false;
        has_last_separator=false;
        return vec;
    }
    size_t start=0;
    enum{
        READING_START,
        READING_END,
    }state=READING_START;
    has_first_separator=(data[0]==separator);
    has_last_separator=(data[len-1]==separator);
    for(size_t i=0;i<len;i++){
        switch(state){
        case READING_START:
            if(data[i]!=separator){
                start=i;
                state=READING_END;
                [[fallthrough]];
            }else{
                break;
            }
        case READING_END:
            if(data[i]==separator){
                vec.push_back(substr(start,i-start));
                state=READING_START;
            }
            break;
        }
    }
    if(state==READING_END){
        vec.push_back(substr(start,len-start));
    }
    return vec;
}

String String::substr(size_t start,size_t n) const{
    if(start>len)return String();
    return String(data.get(),min(len-start,n));
}

char * String::release(){
    char * tmp=data.release();
    data=strdup("");
    len=0;
    return tmp;
}

static void string_format_hex_rec(String &s,uint64_t h){
    if(h>15){
        string_format_hex_rec(s,h/16);
        h%=16;
    }
    s+=((h<10)?(h+'0'):((h-10)+'A'));
}

String String::format_hex(uint64_t h){
    String temp("0x");
    string_format_hex_rec(temp,h);
    return temp;
}


