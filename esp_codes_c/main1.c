/* Inclusión de archivos */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

char mensaje[] = "Hola este es un mensaje";
char vocales[100];
char consonantes[100];
int caracteres_en_mensaje = 0;
int vocales_en_mensaje = 0;
int consonantes_en_mensaje = 0;

/* Declaración de funciones */

void contador_letras(char cadena[]);
void contador_vocales(char cadena[]);
void contador_consonantes(char cadena[]);
void volteador_mensaje (char cadena[]);

/* PRINCIPAL */

int main(int argc, char *argv[]) 
{	
	contador_letras(mensaje);
	contador_consonantes(mensaje);
	contador_vocales(mensaje);
	volteador_mensaje(mensaje);
	return 0;
}

/* Definición de funciones */

void volteador_mensaje (char cadena[])
{
	int index = 0;
	int vol_counter = 0;
	while(cadena[index] != '\0')
	{
		index++;
	}	
	char msj_volteado[index];
	for(vol_counter = 0 ; vol_counter < index ; vol_counter++)
	{
		int retorno = index - vol_counter;
		printf("num = %i\r\n", retorno);
		msj_volteado[vol_counter] = cadena[retorno];
	}
	printf("El mensaje: \"%s\" volteado es: \"%s\" ", cadena, msj_volteado);
}

void contador_consonantes(char cadena[])
{
	int index = 0;
	int counter_con = 0;
	while(cadena[index] != '\0')
	{
		char v = cadena[index];
		if((v != 'a') && (v != 'e') && (v != 'i') && (v != 'o') && (v != 'u') && (v !=' '))
		{
			consonantes[counter_con] = cadena[index];
			counter_con++;
		}
		index++;
	}
	printf("El mensaje: \"%s\" tiene: %i consonantes y son: \"%s\" \n", cadena, counter_con, consonantes);	
}

void contador_vocales (char cadena[])
{
	int index = 0;
	int counter_voc = 0;
	while(cadena[index] != '\0')
	{
		char v = cadena[index];
		if((v == 'a') || (v == 'e') || (v == 'i') || (v == 'o') || (v == 'u'))
		{
			vocales[counter_voc] = cadena[index];
			counter_voc++;
		}
		index++;
	}
	printf("El mensaje: \"%s\" tiene: %i vocales y son: \"%s\" \n", cadena, counter_voc, vocales);
}

void contador_letras(char cadena[])
{
	int index = 0;
	while(cadena[index] != '\0')
	{
		index++;
	}
	printf("El mensaje: \"%s\" tiene: %i letras\n", cadena, index);
}
