void print(const char * c){
    asm(
        "push %%ax\ncall asm_print"
        :
        :"a" (c)
    );
}

void cmain(){
    print("C is working!\r\n");
}

