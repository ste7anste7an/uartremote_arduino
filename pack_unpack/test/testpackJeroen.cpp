#include <iostream>
#include <stdio.h>
using namespace std;
#include "struct.c"
#include "struct_endian.c"

char buf1[BUFSIZ] = {'\0',};
char buf2[BUFSIZ] = {'\0',};
char str[BUFSIZ] = {'\0',};
char fmt[BUFSIZ] = {'\0',};
int val = 42;


int rval;
char rstr[32] = {'\0',};

struct Arguments {
    void* buf;
    char* fmt;
    template<typename... Args> friend void unpack(const Arguments& a, Args... args) {
        struct_unpack(a.buf,a.fmt, args...);
    }
};

void led(Arguments args) {
    int r,g,b,n;
    unpack(args,&r,&g,&b,&n);
    cout << "LED on: " << r << ' '<< g << ' ' << b << ' ' << n << '\n';
}



int main() {
    struct_pack(buf1, "4i", val, val+1,val+2,val+3);
    Arguments a = {buf1, "4i"};
    led(a);
    strcpy(str, "test");
    snprintf(fmt, sizeof(fmt), "%ds", strlen(str));

    struct_pack(buf2, fmt, str);
    // unpack(a, &rval);
    struct_unpack(buf2, fmt, rstr);
    printf("val=%d\n",rval);
    printf("fmt=%s, rstr=%s\n",fmt,rstr);
}