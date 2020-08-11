#include "MenuEngine.h"
#include "TextEngine.h"
#include "IOLayer.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX(x,y)((x)>(y)?(x):(y))
#define MIN(x,y)((x)<(y)?(x):(y))

namespace TEdit {
    extern bool do_loop;
}

namespace TEdit::MenuEngine {
    
    int16_t line=24;
    int16_t height=1;
    
    char * filename=nullptr;
    size_t filename_len=0;
    constexpr size_t filename_max=40;
    
    void setfile(const char * s){
        strncpy(filename,s,filename_max);
        filename[filename_max]='\0';
        filename_len=strlen(filename);
    }
    
    bool fullscreen=false;
    
    enum{
        STATE_NONE,
        STATE_SAVING,
        STATE_QUITCONFIRM,
    } state=STATE_NONE;
    
    void init(){
        filename=(char*)calloc(filename_max+1,1);
    }
    
    void quit(){
        free(filename);
    }
    
    int16_t getHeight(){
        return height;
    }
    
    void draw(bool focused){
        if(focused){
            IOLayer::fillLine(line,height,' ',IOLayer::BLACK,IOLayer::WHITE);
            IOLayer::setColor(IOLayer::DARK_GREY,IOLayer::WHITE);
            IOLayer::moveCursor(0,line);
            switch(state){
            case STATE_NONE:
                IOLayer::writeStr("S - Save, Q - Quit, F - Fullscreen");
                break;
            case STATE_SAVING:
                IOLayer::writeStr("Save As: ");
                IOLayer::writeChars(filename,filename_len);
                return;
            case STATE_QUITCONFIRM:
                IOLayer::writeStr("Quit? (Y/N)");
                return;
            }
        }else{
            IOLayer::fillLine(line,height,' ',IOLayer::BLACK,IOLayer::DARK_GREY);
            IOLayer::setColor(IOLayer::WHITE,IOLayer::DARK_GREY);
            IOLayer::moveCursor(0,line);
            IOLayer::writeStr("File: ");
            if(filename_len>0){
                IOLayer::writeChars(filename,filename_len);
            }else{
                IOLayer::writeStr("...");
            }
        }
        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
        TextEngine::update_cursor();
    }
    
    bool handle_input(IOLayer::keypress key){
        switch(state){
        case STATE_SAVING:
            if(key.key=='\n'||key.key=='\r'){//confirm
                if(TextEngine::save(filename)){
                    state=STATE_NONE;
                    IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                    TextEngine::update_cursor();
                    return false;
                }
                return true;
            }else if(key.key=='\e'){//cancel
                state=STATE_NONE;
                IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                TextEngine::update_cursor();
                return false;
            }else if(key.key=='\b'&&filename_len>0){
                filename_len--;
                filename[filename_len]='\0';
            }else if((isgraph(key.key)||key.key==' ')&&filename_len<filename_max){
                filename[filename_len]=key.key;
                filename_len++;
                filename[filename_len]='\0';
            }
            return true;
        case STATE_QUITCONFIRM:
            if(key.key=='y'||key.key=='Y'||key.key=='\n'||key.key=='\r'){
                do_loop=false;
                return false;
            }else if(key.key=='n'||key.key=='N'||key.key=='\e'){
                state=STATE_NONE;
                IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                TextEngine::update_cursor();
                return false;
            }
            return true;
        case STATE_NONE:
            if(key.type==IOLayer::ALT){
                return false;
            }else if(key.key=='s'||key.key=='S'){
                state=STATE_SAVING;
            }else if(key.key=='q'||key.key=='Q'){
                state=STATE_QUITCONFIRM;
            }else if(key.key=='f'||key.key=='F'){
                fullscreen=!fullscreen;
                if(fullscreen){
                    TextEngine::set_offset(0);
                }else{
                    TextEngine::set_offset(height);
                }
                return false;
            }
            return true;
        default:
            return true;
        }
    }
    
    bool is_fullscreen(){
        return fullscreen;
    }
    
}
