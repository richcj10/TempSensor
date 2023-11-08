#include "Com.h"
#include <Arduino.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_CMD_NBR 16 //Max Commands in List

struct CmdType //CMD structure to hold info, make it easy to access and easier to read
{
    char* Name;
    char* Discription;
    char Show;
    void (*func)();
};

struct Cmds // Holds the CMD array, This is static, pros/cons to this
{
    CmdType the_array[MAX_CMD_NBR];
    unsigned char  ComandCount = 0;
};

Cmds ListOfComands; 
CmdType CmdTemp;

const int BUFFER_SIZE = 25;
char buf[BUFFER_SIZE];  //Serial Input Buffer 

void ComStart(){
    Serial.begin(115200);
}

void BufferClear();  //Clear Buffer
char LoadCmd(char* Name, char* Disc, char show, void (*func)()); //Add a Cmd to the list, make it searchable
void CheckCmd(); // runtime to find commands and respond

bool ConfigArray[5] = {1,1,1,1,0};
char CmdIn[8] = {0,0,0,0,0,0,0,0};

char LoadCmd(char* Name, char* Disc, char show, void (*func)()){
  unsigned char Next = ListOfComands.ComandCount+1;
  if(Next >= MAX_CMD_NBR){
    return 0; //No more spots available
  }
  CmdTemp.Name = Name;
  CmdTemp.Discription = Disc;
  CmdTemp.func = {func};
  CmdTemp.Show = show;
  ListOfComands.the_array[Next-1] = CmdTemp;
  ListOfComands.ComandCount = Next;
  return 1;
}

void CheckCmd(){
  char Found = 0; 
  unsigned char k = 0;
  if(Serial.available() > 0){
    BufferClear();
    // for(int i = 0; i < 25; i++){
    //    Serial.print(buf[i]);
    // }
    int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
    //prints the received data
    //Serial.clear();
    // Serial.print("I received: ");
    // for(int i = 0; i < rlen+5; i++){
    //    Serial.print(buf[i]);
    // }
    delay(10); // added stability..idk why  
    for(k=0;k<ListOfComands.ComandCount;k++){
      //Serial.print(int(k));
      //Serial.print(" ");
      CmdTemp = ListOfComands.the_array[k];
      int ReturnVar = strcmp(buf,CmdTemp.Name);
      if(ReturnVar == 0){
        //Serial.println("Found it");
        Found = 1;
        break;
      }
    }
    if(Found == 1){
      CmdTemp = ListOfComands.the_array[k];
      CmdTemp.func();
      BufferClear();
    }
    else{
      Serial.println();
      Serial.println("-No CMD Found - Try Help");
      BufferClear();
    }
  }
}

void BufferClear(){
  for(unsigned char p =0;p<BUFFER_SIZE;p++){
    buf[p] = NULL;
  }
}

void Printhelp(){
  char l = 0;
  Serial.println();
  Serial.println("----CMD LIST: ----");
  for(l;l<ListOfComands.ComandCount;l++){
    CmdTemp = ListOfComands.the_array[l];
    if(CmdTemp.Show == 1){ //Is it a CMD we want public to see? 
      Serial.print(int(l+1));
      Serial.print(") ");
      Serial.print(CmdTemp.Name);
      Serial.print(": ");
      Serial.println(CmdTemp.Discription);
      delay(10);
    }
  }
}

void LogSetup(char DebugLevel){
    switch (DebugLevel){
        case ERROR:
            ConfigArray[0] = 1;
            ConfigArray[1] = 0;
            ConfigArray[2] = 0;
            ConfigArray[3] = 0;
            break;
        case LOG:
            ConfigArray[0] = 1;
            ConfigArray[1] = 1;
            ConfigArray[2] = 0;
            ConfigArray[3] = 0;
            break;
        case NOTIFY:
            ConfigArray[0] = 1;
            ConfigArray[1] = 1;
            ConfigArray[2] = 1;
            ConfigArray[3] = 0;
            break;
        case DEBUG:
            ConfigArray[0] = 1;
            ConfigArray[1] = 1;
            ConfigArray[2] = 1;
            ConfigArray[3] = 1;
            break;
    }
}

char Log(char level,const char* format, ...){
    switch (level){
        case ERROR:
            if(ConfigArray[0] == 1){
                Serial.print("ERR>");
            }
            else{
                return 0; 
            }
            break;
        case LOG:
            if(ConfigArray[1] == 1){
                Serial.print("LOG>");
            }
            else{
                return 0; 
            }
            break;
        case NOTIFY:
            if(ConfigArray[2] == 1){
                Serial.print("NOTFY>");
            }
            else{
                return 0; 
            }
            break;
        case DEBUG:
            if(ConfigArray[3] == 1){
                Serial.print("DEBUG>");
            }
            else{
                return 0; 
            }
            break;
        default:
            Serial.print("ALT>");
            break;
    }
    static char loc_buf[64];
    char * temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            //return 0;
        }
    }
    
    vsnprintf(temp, len+1, format, arg);

    Serial.write(temp);
    va_end(arg);
    if(len >= sizeof(loc_buf)){
        free(temp);
    }
    return 0;
}