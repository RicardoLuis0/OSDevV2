#ifndef STRING_H
#define STRING_H

#include "util/smart_ptr.h"

namespace Util {
    class String {
            UniquePtr<char> data;//null terminated string
            size_t len;
            String(UniquePtr<char> && data,size_t len);
        public:
            String();
            String(const char * other);
            String(const String & other);
            String(String && other);
            virtual ~String();
            
            size_t size();
            
            char& operator[](size_t);
            char operator[](size_t) const;
            
            String operator+(const char * other) const;
            String operator+(const String & other) const;
            
            String& operator+=(const char * other);
            String& operator+=(const String & other);
            
            String& operator=(const char * other);
            String& operator=(const String & other);
            String& operator=(String && other);
        protected:
        private:
    };
}

#endif // STRING_H
