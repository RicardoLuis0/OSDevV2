#pragma once

#include "TextLine.h"
namespace TEdit {
    class Text {
            Text(const Text& other)=delete;
            Text& operator=(const Text& other)=delete;
        public:
            Text();
            ~Text();
            Text(Text && other);
            Text& operator=(Text && other);
            TextLine& get(size_t x);
            size_t size();
            void insert(size_t at,TextLine * line);
            void erase(size_t at);
        private:
            void grow(size_t to);
            void grow(size_t to,size_t n,TextLine * p);
            TextLine ** lines;
            size_t _alloc;
    };
}
