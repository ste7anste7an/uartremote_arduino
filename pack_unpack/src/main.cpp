#include <Arduino.h>
#include <struct.h>
#include <stdarg.h>
#include <string.h>
#include <cstdio>

#include "UartRemote.h"

// #include <Bluepad32.h>

#define RXD1 18
#define TXD1 19

char cmd[32]; // global temporary storage for command names

UartRemote uartremote;


void led(Arguments args) {
    int r,g,b,n;
    unpack(args,&r,&g,&b,&n);
    Serial.printf("LED on: %d, %d, %d, %d\n", r, g, b, n);
    uartremote.send_command("ledack","B",0);
}

void add(Arguments args) {
    int a,b;
    unpack(args,&a,&b);
    Serial.printf("sum on: %d, %d\n", a, b);
    int c=a+b;
    uartremote.send_command("imuack","i",c);
}

unsigned char buf[BUFSIZ] = {'\0',};
  
void setup() {


  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  // debug 
  Serial.begin(9600);
  // put your setup code here, to run once:

//  Serial.println("Bluepdad32 firmware check.");
//   Serial.println();

//   // Print firmware version on the module
//   String fv = BP32.firmwareVersion();
//   String latestFv;
//   Serial.print("Firmware version installed: ");
//   Serial.println(fv);

//   // Print required firmware version
//   Serial.print("Latest firmware version available : ");
//   Serial.println(BLUEPAD32_LATEST_FIRMWARE_VERSION);


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

  uartremote.flush(); 
  // put your setup code here, to run once:
}

int i=0;
int s=0;
Arguments args;
void loop() {
  i+=1;
  if (i>100) {i=0;}
  // put your main code here, to run repeatedly:
  // Arguments args = uartremote.receive(cmd);
  // Serial.println("Command received: "+String(cmd));
  // uartremote.command(cmd,args);
 uartremote.send_command("led","4i",i+1,i+2,i+3,i+4);
 args=uartremote.receive_command(cmd);
 if (strcmp(cmd,"ledack")!=0) {uartremote.flush();}
 else {
  printf("received ack: %s\n",cmd);
 }
 delay(10000);
 uartremote.send_command("add","2i",i+1,i+2);
 args=uartremote.receive_command(cmd);
  if (strcmp(cmd,"addack")!=0) {uartremote.flush();}
 else {
   unpack(args,&s);
   printf("Received sumack: %s, sum=%d\n",cmd,s);
 }
 delay(10000);
}

