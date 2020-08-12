#include "TextEngine.h"
#include "Util.h"
#include <ctype.h>

#include <stdio.h>

namespace TEdit::TextEngine {
    
    int16_t x,y;
    
    int16_t lpos;
    
    int16_t x_min,x_max,y_min,y_max;
    
    int16_t x_mmin,x_mmax,y_mmin,y_mmax;
    
    uint32_t view_x,view_y;
    
    bool virtual_whitespace=false;
    
    Text * data;
    
    static void redraw_line(size_t i){
        IOLayer::moveCursor(0,i);
        IOLayer::fillLine(i,1,' ',IOLayer::WHITE,IOLayer::BLACK);
        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
        size_t max=data->size();
        if((i+view_y)>max)return;
        auto &l=data->get(i+view_y);
        if(view_x<l.len()){
            IOLayer::writeChars(l.get()+view_x,l.len()-view_x);
        }
        IOLayer::moveCursor(x,y);
    }
    
    void redraw_full(){//FULL REDRAW, SLOW
        IOLayer::fillLine(0,y_mmax,' ',IOLayer::WHITE,IOLayer::BLACK);
        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
        size_t max=data->size();
        for(int i=0;i<y_mmax;i++){
            if((i+view_y)>=max)break;
            auto &l=data->get(i+view_y);
            if(view_x<l.len()){
                IOLayer::moveCursor(0,i);
                IOLayer::writeChars(l.get()+view_x,l.len()-view_x);
            }
        }
        IOLayer::moveCursor(x,y);
    }
    
    static void x_line(bool always_redraw=false){
        if(data->size()>(view_y+y)){
            size_t pos=min<size_t>(data->get(view_y+y).len(),lpos);
            auto view_x_old=view_x;
            while(!(pos>=view_x)&&pos<(view_x+x_max)){
                if(pos>=view_x){
                    view_x+=x_max;
                }else{
                    view_x-=x_max;
                }
            }
            x=pos-view_x;
            if(always_redraw||view_x_old!=view_x) redraw_full();
        }else{
            x=0;
            if(always_redraw) redraw_full();
        }
    }
    
    static void y_plus(){
        if(y<y_max){
            if(virtual_whitespace||(y+view_y)<(data->size()-1)){
                y++;
                x_line();
            }
        }else{
            view_y++;
            x_line(true);
        }
        IOLayer::moveCursor(x,y);
    }
    
    static void y_minus(){
        if(y>y_min){
            y--;
            x_line();
        }else if(view_y>0){
            view_y--;
            x_line(true);
        }
        IOLayer::moveCursor(x,y);
    }
    
    static void x_plus(){
        if(data->size()>(view_y+y)&&(data->get(view_y+y).len()>(x+view_x))){
            x++;
        }else{
            x=0;
            view_x=0;
            if(y<y_max){
                y++;
            }else{
                view_y++;
                redraw_full();
            }
        }
        lpos=x+view_x;
        IOLayer::moveCursor(x,y);
    }
    
    static void x_minus(){
        if(x>x_min){
            x--;
        }else if(y>y_min){
            y--;
            if(data->size()>(view_y+y))lpos=data->get(view_y+y).len();
            x_line();
        }
        lpos=x+view_x;
        IOLayer::moveCursor(x,y);
    }
    
    static void write(char c){
        auto &l=data->get(y+view_y);
        if((x+view_x)<(l.len())){
            l.insert(c,x+view_x);
            x_plus();
            redraw_line(y);
        }else{
            l.insert(c,x+view_x);
            IOLayer::writeChar(c);
            x_plus();
        }
    }
    
    static void erase(){
        if(x==0){
            if(y>0){
                if((view_y+y)<data->size()){
                    auto & a=data->get(view_y+y-1);
                    lpos=a.len();
                    auto & b=data->get(view_y+y);
                    a.append(b);
                    data->erase(view_y+y);
                    y_minus();
                    x_line(true);
                }else{
                    lpos=data->get(view_y+y-1).len();
                    y_minus();
                    x_line();
                }
            }
        }else{
            x_minus();
            auto & l=data->get(view_y+y);
            l.erase(view_x+x);
            redraw_line(y);
        }
    }
    
    void newline(){
        if(!virtual_whitespace||(y+view_y)<data->size()){
            data->insert(y+view_y+1,data->get(y+view_y).split(x+view_x));
        }
        lpos=0;
        y_plus();
        redraw_full();
    }
    
    void init(int16_t off,Text * t_data){
        x_min=0;
        x_mmin=-1;
        x=0;
        x_max=79;
        x_mmax=80;
        y_min=0;
        y_mmin=-1;
        y=0;
        y_max=24-off;
        y_mmax=25-off;
        view_x=0;
        view_y=0;
        lpos=0;
        IOLayer::moveCursor(x,y);
        data=t_data;
    }
    
    void set_offset(int16_t off){
        y_max=24-off;
        y_mmax=25-off;
        if(y==y_mmax)y_minus();
    }
    
    void handle_input(IOLayer::keypress key){
        switch(key.type){
            case IOLayer::ARROW_UP:
                y_minus();
                break;
            case IOLayer::ARROW_DOWN:
                y_plus();
                break;
            case IOLayer::ARROW_LEFT:
                x_minus();
                break;
            case IOLayer::ARROW_RIGHT:
                x_plus();
                break;
            case IOLayer::KEY:
                while(key.count--){
                    switch(key.key){
                    case ' ':
                        write(' ');
                        break;
                    case '\b':
                        erase();
                        break;
                    case '\n':
                    case '\r':
                        newline();
                        break;
                    case '\t':
                        do{
                            write(' ');
                        }while((view_x+x)%4);
                        break;
                    default:
                        if(isgraph(key.key)){
                            write(key.key);
                        }
                    }
                }
                break;
        }
    }
    
    int save(const char * filename){
        FILE * f=fopen(filename,"w");
        if(!f)return 0;
        for(size_t i=0;i<data->size();i++){
            fputs(data->get(i).get(),f);
            fputc('\n',f);
        }
        fclose(f);
        return 1;
    }
    
    void update_cursor(){
        IOLayer::moveCursor(x,y);
    }
    
}
