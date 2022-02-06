##include <Arduino.h>
#include <struct.h>
#include <stdarg.h>
#include <string.h>
#include <cstdio>

#include "UartRemote.h"


#define RXD1 18
#define TXD1 19

char cmd[32]; // global temporary storage for command names

UartRemote uartremote;


void led(Arguments args) {
    int r,g,b,n;
    unpack(args,&r,&g,&b,&n);
    Serial.printf("LED on: %d, %d, %d, %d\n", r, g, b, n);
    uartremote.send("ledack","B",0);
}

void add(Arguments args) {
    int a,b;
    unpack(args,&a,&b);
    Serial.printf("sum on: %d, %d\n", a, b);
    int c=a+b;
    uartremote.send("imuack","i",c);
}

unsigned char buf[BUFSIZ] = {'\0',};
  
void setup() {


  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  // debug 
  Serial.begin(9600);
  // put your setup code here, to run once:

  uartremote.add_command("led", &led);
  uartremote.add_command("add", &add);
  
  Serial.printf("\n\n\n");
  uartremote.testsend("led",buf,"4i",10,11,12,13);
  int l=buf[1];
  for (int i=0; i<l+2; i++) { Serial.printf("%02x ",buf[i]);}
  Serial.println();
    
  Arguments args = uartremote.testreceive(cmd,buf);
  Serial.println("executing received command");
  uartremote.command(cmd,args);

   
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  Arguments args = uartremote.receive(cmd);
  Serial.println("Command received: "+String(cmd));
  uartremote.command(cmd,args);
 
}

