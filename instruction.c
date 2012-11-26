/*!
 * \file instruction.c
 * \brief Description du jeu d'instruction.
 */

#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//! Chaines de caracteres correspondant aux codes des operations
const char* cop_names[] = { "ILLOP", "NOP", "LOAD", "STORE", "ADD", "SUB", "BRANCH", "CALL", "RET", "PUSH", "POP", "HALT" };

//! Chaines de caracteres correspondant aux codes conditions
const char* condition_names[] = { "NC", "EQ", "NE", "GT", "GE", "LT", "LE" };

//! Impression du registre d'une instruction sous forme lisible.
//! Affiche le registre sous la forme R0i si 0 < i <= 9 ou Ri si i > 9.
/*!
 * \param instr l'instruction à imprimer
 */
void print_register(Instruction instr) {
	printf("R%02d, ", (int) instr.instr_generic._regcond);
}

//! Impression du code condition d'une instruction sous forme lisible.
/*!
 * \param instr l'instruction à imprimer
 */
void print_condition(Instruction instr) {
	printf("%s ", condition_names[instr.instr_generic._regcond]);
}

//! Impression des opérandes d'une instruction sous forme lisible.
/*!
 * \param instr l'instruction à imprimer
 */
void print_op(Instruction instr) {
	if (instr.instr_generic._immediate) { // Si I = 1 : Immediat
		printf("#%d", instr.instr_immediate._value);
	} else {				
		if (instr.instr_generic._indexed) { // Si I = 0 et X = 1 : Adressage indexe
			printf("%+d[", (int) instr.instr_indexed._offset); // Offset sous la forme +/-offset
			printf("R%02d]", (int) instr.instr_indexed._rindex); // Registre pour l'adressage indirect [R..]
		} else { // Si I = 0 et X = 0 : Adressage direct
			printf("@%04x", (int) instr.instr_absolute._address);	
		}
	}
}

//! Impression d'une instruction sous forme lisible (désassemblage)
/*!
 * \param instr l'instruction à imprimer
 * \param addr son adresse
 */
void print_instruction(Instruction instr, unsigned addr) {
	printf("%s ", cop_names[instr.instr_generic._cop]);
	switch (instr.instr_generic._cop) {
		case ILLOP:
		case NOP:
		case RET:
		case HALT:
			break;
		case LOAD:
		case STORE:
		case ADD:
		case SUB:
			print_register(instr); 
			print_op(instr);
			break;
		case BRANCH:
		case CALL:
			print_condition(instr);
			print_op(instr);
			break;
		case PUSH:
		case POP:
			print_op(instr);
			break;
	}
}
