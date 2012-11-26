/*!
 * \file error.c
 * \brief Messages et codes d'erreurs.
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//! Affichage d'une erreur et fin du simulateur
/*!
 * \note Toutes les erreurs étant fatales on ne revient jamais de cette
 * fonction. L'attribut \a noreturn est une extension (non standard) de GNU C
 * qui indique ce fait.
 * 
 * \param err code de l'erreur
 * \param addr adresse de l'erreur
 */
void error(Error err, unsigned addr){
		printf("ERROR: ");
		switch (err) {
		case ERR_NOERROR:
			printf("No error");
			printf("\tat 0x%08x\n",addr);
			break;
		case ERR_UNKNOWN:
			printf("Unknown instruction");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_ILLEGAL:
			printf("Illegal instruction");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_CONDITION:
			printf("Illegal condition");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_IMMEDIATE:
			printf("Immediate value forbidden");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_SEGTEXT:
			printf("Text index out of bounds");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_SEGDATA:
			printf("Data index out of bounds");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		case ERR_SEGSTACK:
			printf("Stack index out of bounds");
			printf("\tat 0x%08x\n",addr);
			exit(1);
		default:
			exit(0);
		}
		
}

//! Affichage d'un avertissement
/*!
 * \param warn code de l'avertissement
 * \param addr adresse de l'erreur
 */
void warning(Warning warn, unsigned addr){
	printf("WARNING: Program correctly ended by HALT\tat 0x%08x\n",addr);
}

