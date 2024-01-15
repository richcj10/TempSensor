#ifndef COM_H
#define COM_H

#include <string.h>

#define ERROR 1
#define LOG 2
#define NOTIFY 3
#define DEBUG 4

char LoadCmd(char* Name, char* Disc, char show, void (*func)());
void CheckCmd();
void Printhelp();

void LogSetup(char DebugLevel);
char Log(char level,const char* format, ...);

#endif