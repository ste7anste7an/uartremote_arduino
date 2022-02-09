 #include "UartRemote.h"


UartRemote::UartRemote() {}



void UartRemote::add_command(const char* cmd, void (*func)(Arguments)){
  cmds[nr_cmds].cmd=cmd;
  cmds[nr_cmds].function=func;
  nr_cmds++;
}


void UartRemote::command(const char* cmd,Arguments args) {
  for (int i=0; i<nr_cmds; i++) {
     //printf("checking cmd %s\n",cmds[i].cmd);
     if (strcmp(cmd,cmds[i].cmd)==0) {
         (*cmds[i].function)(args);
     }
  }
 // printf("return from UartRemote::command\n");
}

unsigned char UartRemote::readserial1() {
  while (UART.available()==0) {
    delay(10);
  }
  return UART.read();
}

int UartRemote::available()
{
   if (UART.available()>0) {
     return 1;
   }
   else {
     return 0;
   }
}

void UartRemote::flush() {
    delay(100);
    while (available()) {
        UART.read();
    }
}

void uartwrite(char c){
    Serial.print(c,HEX);
    Serial.print(' ');
}

Arguments UartRemote::pack( unsigned char* buf, const char* format, ...) {
  va_list args;
  va_start(args,format);
  pack_va_list(buf,0,format,args);  
  va_end(args);
  Arguments a = {buf,format};

  return a;
}

void UartRemote::send_command(const char* cmd, const char* format, ...) {
  va_list args;
  va_start(args,format);
  pack_va_list((unsigned char*)data_buf,0,format,args);  
  va_end(args);
  int size = struct_calcsize(format);
  unsigned char lc = strlen(cmd);
  unsigned char lf = strlen(format);
  unsigned char l=lc+lf+size+2; // +2 for the length fields of cmd and format
  int i;
  UART.write('<');
  UART.write(l);
  UART.write(lc);
  for (i=0; i<lc; i++) UART.write(cmd[i]);
  UART.write(lf);
  for (i=0; i<lf; i++) UART.write(format[i]);
  //printf("p.size= %d\n",p.size);
  for (i=0; i<size; i++) UART.write((unsigned char)data_buf[i]);
  UART.write('>');
}

void UartRemote::testsend(const char* cmd, unsigned char* buf,const char* format, ...) {
  int pt=0;
  va_list args;
  va_start(args,format);
  pack_va_list((unsigned char*)data_buf,0,format,args);  
  va_end(args);
  int size = struct_calcsize(format);
  //Serial.printf("Size calc:%d\n",size);
  unsigned char lc = strlen(cmd);
  unsigned char lf = strlen(format);
  unsigned char l=lc+lf+size+2; // +2 for the length fields of cmd and format
  //Serial.printf("lc=%d, lf=%d, size=%d,len=%d\n",(int)lc,(int)lf,size,(int)l);
  int i;
  buf[pt++]='<';
  buf[pt++]=l;
  buf[pt++]=lc;
  for (i=0; i<lc; i++) buf[pt++]=cmd[i];
  buf[pt++]=lf;
  for (i=0; i<lf; i++) buf[pt++]=format[i];
  //printf("p.size= %d\n",p.size);
  for (i=0; i<size; i++) buf[pt++]=(unsigned char)data_buf[i];
  buf[pt++]='>';
}

Arguments UartRemote::receive_command(char* cmd) {
  char delim=readserial1();
  if (delim!='<') {
    strcpy(cmd,"error");
    flush();
    send_command("error","b",'!');
    return pack((unsigned char*)data_buf,"b","!");
  }
  //printf("left delim %d   %d!=60: %d\n",delim,delim,delim!=60);
  unsigned char l =readserial1();
  unsigned char lc = readserial1();

  for (int i=0; i<lc; i++) {
    cmd[i]=readserial1();
  }
  cmd[lc]=0;
  unsigned char lf = readserial1();
  for (int i=0; i<lf; i++) {
    format[i]=readserial1();
  }
  format[lf]=0;
  int l_data=l-2-lc-lf;
  for (int i=0; i<l_data; i++) {
    data_buf[i]=readserial1();
  }
  delim=readserial1();
  //printf("right delim %c\n",delim);
  if (delim!='>') {
      strcpy(cmd,"error");
      flush();
      send_command("error","b",'!');
      return pack((unsigned char*)data_buf,"b","!");
  }
  Arguments a = {data_buf, format};
  return a;
}

Arguments UartRemote::testreceive(char* cmd, unsigned char* buf) {
  int pt=0;
  char delim=buf[pt++];
  if (delim!='<') {
    strcpy(cmd,"error");
    flush();
    send_command("error","b",'!');
    return pack((unsigned char*)data_buf,"b","!");
  }
  unsigned char l =buf[pt++];
  unsigned char lc = buf[pt++];
  for (int i=0; i<lc; i++) {
    cmd[i]=buf[pt++];
  }
  cmd[lc]=0;
  unsigned char lf = buf[pt++];
  for (int i=0; i<lf; i++) {
    format[i]=buf[pt++];
  }
  format[lf]=0;
  int l_data=l-2-lc-lf;
  for (int i=0; i<l_data; i++) {
    data_buf[i]=buf[pt++];
  }
  delim=buf[pt++];
  //printf("right delim %c\n",delim);
  if (delim!='>') {
      strcpy(cmd,"error");
      flush();
      send_command("error","b",'!');
      return pack((unsigned char*)data_buf,"b","!"); 
  }
  Arguments a = {data_buf, format};
  return a;
}

 
