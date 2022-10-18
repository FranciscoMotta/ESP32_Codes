#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct Person_struct
{
    char nombre[20];
    char apellido[20];
    int edad;
} Person;


void main (void)
{
    Person person;
    strcpy(person.nombre, "Julio");
    strcpy(person.apellido, "Motta");
    person.edad = 22;
    printf("Persona: %s %s tiene %s años de edad\n", person.nombre, person.apellido, person.edad);
}