/*
* Autores:
*	Santiago Hernandez Arias Codigo 1631281
*   Alejandro Palacios Hurtado Codigo 1630531
* Nombre: commandUtil.c
* Descripción: Archivo de implementaciones de funciones auxiliares para el shell basico
* Fecha de creación: 4 / 05 / 2018
* Fecha de modificación: 16 / 05 / 2018
*/

#include "commandUtil.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


// Funcion que recibe un string y devuelve una copia en forma "normalizada", eliminando espacios innecesarios
// EJ: "   ls     -la  > 1.txt   " -> "ls -la > 1.txt"
char *removeExtraSpaces(char input[]) {
	int wordCounter = countWords(input);
	int originalLength = strlen(input);

	char *output = input;

	int outputIndexCounter = 0;
	int availableSpacesCounter = wordCounter - 1;
	int canAddSpace = 0;
	int wordStarted = 0;

	if (input[originalLength - 1] == '\n') {
		input[originalLength - 1] = '\0';
	}

	for (int i = 0; i < originalLength; i++) {
		if (input[i] != ' ') {
			canAddSpace = 1;
			wordStarted = 1;
			output[outputIndexCounter] = input[i];
			outputIndexCounter++;
		} else {
			if (canAddSpace == 1 && availableSpacesCounter > 0) {
				output[outputIndexCounter] = input[i];
				outputIndexCounter++;
				availableSpacesCounter--;
				canAddSpace = 0;
			}
		}
	}
	output[outputIndexCounter] = '\0';

	return output;
}


// Funcion que separa los "argumentos" del string input, de acuerdo al caracter " " (espacio), y los introduce
// en el arreglo bidimensional que separa los argu
// Ej: "ls -la" -> [[ls], [-la]]
void separarArgumentos(char *args[], char input[]) {
	char *arg;
	arg = strtok(input, " ");

	int i = 0;
	while (arg != NULL) {
		args[i] = arg;
		arg = strtok(NULL, " ");
		i++;
	}
	args[i] = NULL;
}


// Funcion que cuenta los "argumentos" validos en un string, aquellos separados por espacios " "
// Ej: "   ls        -la >   1.txt" -> 4
int countWords(char input[]) {
	int originalLength = strlen(input);

	int wordStarted = 0;
	int wordCounter = 0;

	for (int i = 0; i < originalLength; i++) {
		if (input[i] != ' ' && wordStarted == 0) {
			wordStarted = 1;
			wordCounter++;
		}

		if (wordStarted == 1 && input[i + 1] == ' ') {
			wordStarted = 0;
		}
	}
	return wordCounter;
}


// Funcion que determina la posicion del caracter de redireccion ">" en el arreglo de argumentos
// Ej: [[ls], [-la], [>], [1.txt]] -> 2
int checkred(char *args[]) {
	int originalLength = strlen(*args);


	int i = 0;

	while (1) {
		if (args[i] == NULL) {
			break;
		} else if (*args[i] == '>') {
			return i;
		}
		i++;
	}
	return 0;
}


// Funcion que determina la posicion del caracter pipe "|" en el arreglo de argumentos char *args[], además,
// en caso de haber pipe, char *args[] guardara aquello a la izquierda del pipe, y char *args2[] guardara aquello
// que hay a la derecha.
// Ej: [[grep], [world], [file.txt], [|], [sort]], [[]] -> [[grep], [world], [file.txt]], [[sort]]
int checkpip(char *args[], char *args2[]) {
	int originalLength = strlen(*args);


	int i = 0;

	while (1) {
		if (args[i] == NULL) {
			break;
		} else if (*args[i] == '|') {
			args[i] = NULL;
			int j = i + 1;
			int m = 0;
			while (1) {
				if (args[j] == NULL) {
					break;
				} else {
					args2[m] = strdup(args[j]);
					//~ args2[m] = args[j];
				}
				m++;
				j++;
				args2[m] = NULL;
				args2[m + 1] = NULL;
			}
			return i;
		}
		i++;
	}
	return 0;
}


// Funcion que ejecuta comandos cuando no existe pipe
int ejecutr(char *args1[]) {
	int red = checkred(args1);

	if (red){
				if(!verificaArch(args1[red + 1])){
					args1[red] = NULL;
					close(STDOUT_FILENO);
					int fd = open(args1[red + 1], O_CREAT | O_RDWR, S_IRUSR);
					}else{
						printf("El archivo ya existe\n");
						return 0;
						}	
			}

	int ls = execvp(args1[0], args1);

	if(ls){
		printf("Ocurrio un error, por favor verifique los comandos\n");
		exit(0);
		}

	return 1;
}


// Funcion que ejecuta comandos cuando existe pipe, recibe en char *args1[] aquello a la izquierda del pipe,
// en char *args2[] aquello a la derecha. Luego hace un fork, el hijo ejecuta la parte izquierda y el padre
// la derecha, usando las funciones de c pipe() para pasar el resultado del hijo al padre como comando.
// La funcion pipe crea una tuberia (una especie de buffer) con dos descriptores de archivo, se le pasa un
// arreglo de dos posiciones en cuya primer posicion va el read y en la segunda posicion el write.
// HIJO: Con dup2, se copia el descriptor de archivo del primer argumento y el segundo descriptor de archivo queda
// apuntando al valor del primero. Se copia el write del buffer, es decir el descriptor de archivo 1 queda apuntando
// al write. Se cierran el read y el write del pipe pues ya no se necesitan y se ejecuta el comando de la izq.
// PADRE: el padre ejecuta el comando de la derecha.
// IMPORTANTE: en el hijo se redirije el STDOUT, salida estandar, de manera que ahora esta se dirige al STDIN
// (entrada estandar), gracias a la tuberia creada por el padre, el padre recibe su input del hijo.
int ejecutaPipe(char *args1[], char *args2[]) {
	int red;
	int pipes[2];

	int i = 0;

	pipe(pipes);
	pid_t pid = fork();

	if (pid == 0) {
			red = checkred(args1);
		
      		dup2(pipes[1],1); 
      		
			close(pipes[0]);	
			
			close(pipes[1]);			
			
				int red = checkred(args1);

			if (red) {
				if(!verificaArch(args1[red + 1])){
					args2[red] = NULL;
					close(STDOUT_FILENO);
					// Verificar si el archivo ya existe y detener la ejecucion de existir este
					int fd = open(args1[red + 1], O_CREAT | O_RDWR, S_IRUSR);
					}else{
						printf("El archivo ya existe\n");
						return 0;
						}	
			}

	int ls = execvp(args1[0], args1);

	if(ls){
		printf("Ocurrio un error, por favor verifique los comandos\n");
		exit(0);
		}

	} else {
			red = checkred(args2);
			
			dup2(pipes[0],0);	
			
			close(pipes[1]);
					
			close(pipes[0]);				
			
			if (red) {
				if(!verificaArch(args2[red + 1])){
					args2[red] = NULL;
					close(STDOUT_FILENO);
					int fd = open(args2[red + 1], O_CREAT | O_RDWR, S_IRUSR);
					}else{
						printf("El archivo ya existe\n");
						return 1;
						}	
			}

	int ls = execvp(args2[0], args2);

	if(ls){
		printf("Ocurrio un error, por favor verifique los comandos\n");
		exit(0);
		}

					  
		}	

	return 1;
}

// Funcion que verifica si el archivo con nombre const char *fname existe en el directorio del proceso.
int verificaArch(const char *fname){
	FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}
