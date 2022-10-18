#include <stdio.h>
#include <string.h>

typedef struct Persona_struct
{
	char nombre[20];
	char apellido[20];
	int edad;
}Persona;

void update_Person (Persona *persona);

int main(int argc, char** argv) {
	printf("Hello world!\n");
	Persona	persona;
	update_Person(&persona);
	printf("Persona: %s %s tiene %s\n", persona.nombre, persona.apellido, persona.edad);
	return 0;
}

void update_Person (Persona *persona)
{
	strcpy(persona->nombre, "Julio");
	strcpy(persona->nombre, "Motta");
	persona->edad = 22;
}
