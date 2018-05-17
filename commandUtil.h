/*
* Autores:
*	Santiago Hernandez Arias Codigo 1631281
*   Alejandro Palacios Hurtado Codigo 1630531
* Nombre: commandUtil.h
* Descripción: Archivo de cabecera para funciones auxiliares
* Fecha de creación: 4 / 05 / 2018
* Fecha de modificación: 16 / 05 / 2018
*/

#ifndef __SHELL__
#define __SHELL__
int countWords(char input[]);
char* removeExtraSpaces(char input[]);
void separarArgumentos(char *args[], char input[]);
int checkred(char *args[]);
int checkpip(char *args[], char *args2[]);
int ejecutr(char *args1[]);
int ejecutaPipe(char *args1[],char  *args2[]);
int verificaArch(const char *fname);
#endif
