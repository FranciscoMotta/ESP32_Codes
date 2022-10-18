#include <stdio.h>
#include <string.h>

typedef struct Persona_struct
{
	char nombre[20];
	char apellido[20];
	int edad;
} Persona;

void main (void)
{
	Persona persona;
	strcpy(persona.nombre, "Julio");
	strcpy(persona.apellido, "Motta");
	persona.edad = 22;
	printf("Nombre: %s Apellidos: %s Edad: %s\n", persona.nombre, persona.apellido, persona.edad);
}
