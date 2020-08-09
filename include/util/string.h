#ifndef STRING_H
#define STRING_H

#include "util/smart_ptr.h"
#include "util/vector.h"

namespace Util {
    class String {
            UniquePtr<char> data;//null terminated string
            size_t len;
            String(UniquePtr<char> && data,size_t len);
        public:
            String();
            String(const char * other);
            String(const char * other,size_t n);
            String(const String & other);
            String(String && other);
            virtual ~String();
            
            size_t size() const;
            
            char& operator[](size_t);
            char operator[](size_t) const;
            
            String operator+(const char * other) const;
            String operator+(const String & other) const;
            
            String& operator+=(const char * other);
            String& operator+=(const String & other);
            
            String& operator=(const char * other);
            String& operator=(const String & other);
            String& operator=(String && other);
            
            const char * c_str() const;
            
            char * release();
            
            Vector<String> explode(char separator,bool &has_first_separator,bool &has_last_separator) const;
            
            String substr(size_t start,size_t len=-1) const;
            
        protected:
        private:
    };
}

#endif // STRING_H
