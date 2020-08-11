#include "IOLayer.h"
#include "TextEngine.h"
#include "MenuEngine.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
namespace TEdit {
    
    bool do_loop;
    
    int tedit_main(int argc,char ** argv) {
        do_loop=true;
        IOLayer::init();
        Text data;
        MenuEngine::init();
        if(argc==2){
            if(strlen(argv[1])>40){
                IOLayer::writeStr("\nFilename Too long\n");
                return 1;
            }else{
                MenuEngine::setfile(argv[1]);
                FILE * f=fopen(argv[1],"r");
                if(f){
                    int c;
                    size_t i=0;
                    while((c=fgetc(f))!=EOF){//reading per-character is slow but should be ~fine~
                        TextLine &line=data.get(i);
                        if(c=='\n'){
                            i++;
                        }else if(c=='\t'){
                            do{
                                line.insert(c,line.len());
                            }while(line.len()%4);
                        }else if(isgraph(c)||c==' '){
                            line.insert(c,line.len());
                        }
                    }
                    fclose(f);
                }
            }
        }else if(argc>2){
            IOLayer::writeStr("\nInvalid Number of Arguments, must be zero or 1\n");
            return 1;
        }
        IOLayer::fillLine(0,25,' ',IOLayer::WHITE,IOLayer::BLACK);
        IOLayer::moveCursor(0,0);
        TextEngine::init(MenuEngine::getHeight(),&data);
        TextEngine::redraw_full();
        MenuEngine::draw(false);
        bool in_menu=false;
        while(do_loop){
            auto key=IOLayer::get_key();
            if(in_menu){
                in_menu=MenuEngine::handle_input(key);
                if(!in_menu&&MenuEngine::is_fullscreen()){
                    TextEngine::redraw_full();
                }
            }else if(key.type==IOLayer::ALT){
                in_menu=true;
            }else{
                TextEngine::handle_input(key);
            }
            if(in_menu||!MenuEngine::is_fullscreen()){
                MenuEngine::draw(in_menu);
            }
        }
        MenuEngine::quit();
        IOLayer::fillLine(0,25,' ',IOLayer::WHITE,IOLayer::BLACK);
        IOLayer::moveCursor(0,0);
        return 0;
    }
    
}
