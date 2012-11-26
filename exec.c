/*!
 * \file exec.c
 * \brief Exécution d'une instruction.
 */

#include "exec.h"
#include "error.h"
#include <stdio.h>
 
//! Met à jour cc (code condition) selon la valeur de reg.
/*!
 * \param pmach machine en cours d'exécution
 * \param reg numéro de registre
 */
void refresh_cc(Machine *pmach, unsigned int reg) 
{
	if (reg < 0)
	        pmach->_cc = CC_N;
    	else if (reg > 0)
        	pmach->_cc = CC_P;
    	else
        	pmach->_cc = CC_Z;
}

//! Vérifie que le Stack Pointer (SP) ne dépasse pas la zone dédiée à la pile.
//! Il ne faut pas par exemple qu'avec des branchements successifs, on efface les données existantes.
/*!
 * \param pmach machine en cours d'exécution
 * \param addr adresse de l'instruction
 */
void check_stack(Machine *pmach, unsigned addr) 
{
	if (pmach->_sp < pmach->_dataend || pmach->_sp >= pmach->_datasize)
		error(ERR_SEGSTACK,addr);
}


//! Vérifie que l'instruction n'est pas immédiate.
/*!
 * \param instr instruction en cours
 * \param addr adresse de l'instruction
 */
void check_immediate(Instruction instr, unsigned addr) 
{
	if (instr.instr_generic._immediate)
		error(ERR_IMMEDIATE,addr);
}

//! Vérifie si la condition de branchement est respectée.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 */
bool allowed_condition(Machine *pmach, Instruction instr, unsigned addr) 
{
	switch (instr.instr_generic._regcond) {
	case NC: // Pas de condition
		return true;
	case EQ: // Egal à 0
		return (pmach->_cc == CC_Z);
	case NE: // Different de zero
		return (pmach->_cc != CC_Z);
	case GT: // Strictement positif
		return (pmach->_cc == CC_P);
	case GE: // Positif ou nul
		return (pmach->_cc == CC_P || pmach->_cc == CC_Z);
	case LT: // Strictement négatif
		return (pmach->_cc == CC_N);
	case LE: // Négatif ou nul
		return (pmach->_cc == CC_N || pmach->_cc == CC_Z);
	default:
		error(ERR_CONDITION, addr);
	}
}

//! Récupère l'adresse réelle, à partir d'un adressage indexé ou absolu.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 */
unsigned int get_address(Machine *pmach, Instruction instr) 
{	
	if (instr.instr_generic._indexed) {
		return pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset;
	} 
	return instr.instr_absolute._address;
}

//! Vérifie qu'il n'y a pas d'erreur de segmentation sur le tableau des données.
/*!
 * \param pmach machine en cours d'exécution
 * \param data_addr adresse réelle
 * \param addr adresse de l'instruction en cours
 */
void check_data_addr(Machine *pmach, unsigned int data_addr, unsigned addr) 
{
	if (data_addr > pmach->_datasize)
		error(ERR_SEGDATA, addr);
}

//! Décode et exécute l'instruction LOAD.
//! LOAD accepte l'adressage immédiat, absolu et indexé pour la source.
//! Il faut indiquer un registre de destination.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool load(Machine *pmach, Instruction instr, unsigned addr) 
{
	if (instr.instr_generic._immediate) { // Si I = 1 : Immediat
		pmach->_registers[instr.instr_generic._regcond] = instr.instr_immediate._value;
	} else {
		unsigned int address = get_address(pmach, instr);
		check_data_addr(pmach, address, addr);
		pmach->_registers[instr.instr_generic._regcond] = pmach->_data[address];
	}
	refresh_cc(pmach, pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décode et exécute l'instruction STORE.
//! STORE accepte l'adressage absolu et indexé pour la destination.
//! Il faut indiquer un registre pour la source.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool store(Machine *pmach, Instruction instr, unsigned addr) 
{
	check_immediate(instr, addr);		
	unsigned int address = get_address(pmach, instr);
	check_data_addr(pmach, address, addr);
	pmach->_data[address] = pmach->_registers[instr.instr_generic._regcond];
	
	return true;
}

//! Décode et exécute l'instruction ADD.
//! ADD accepte l'adressage immédiat, absolu et indexé pour la source.
//! Il faut indiquer un registre pour la destination.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool add(Machine *pmach, Instruction instr, unsigned addr) 
{
	if (instr.instr_generic._immediate) { // Immediat
		pmach->_registers[instr.instr_generic._regcond] += instr.instr_immediate._value;
	} else {				
		unsigned int address = get_address(pmach, instr);
		check_data_addr(pmach, address, addr);
		pmach->_registers[instr.instr_generic._regcond] += pmach->_data[address];
	}
	refresh_cc(pmach,pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décode et exécute l'instruction SUB.
//! SUB accepte l'adressage immédiat, absolu et indexé pour la source.
//! Il faut indiquer un registre pour la destination.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool sub(Machine *pmach, Instruction instr,unsigned addr) 
{
	if (instr.instr_generic._immediate) { // Immediat
		pmach->_registers[instr.instr_generic._regcond] -= instr.instr_immediate._value;
	} else {				
		unsigned int address = get_address(pmach, instr);
		check_data_addr(pmach, address, addr);
		pmach->_registers[instr.instr_generic._regcond] -= pmach->_data[address];
	}
	refresh_cc(pmach,pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décode et exécute l'instruction BRANCH.
//! BRANCH accepte l'adressage absolu et indexé pour l'adresse de l'instruction à exécuter.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool branch(Machine *pmach, Instruction instr, unsigned addr) 
{
	check_immediate(instr, addr);	
	if (allowed_condition(pmach, instr, addr)) {
		unsigned int address = get_address(pmach, instr);
		pmach->_pc = address;
	}
	return true;
}

//! Décode et exécute l'instruction CALL.
//! CALL accepte l'adressage absolu et indexé pour l'adresse du sous-programme.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool call(Machine *pmach, Instruction instr, unsigned addr) 
{
	check_immediate(instr, addr);	
	check_stack(pmach, addr);
	
	if (allowed_condition(pmach, instr, addr)) {
		pmach->_data[pmach->_sp--] = pmach->_pc;
		unsigned int address = get_address(pmach, instr);
		pmach->_pc = address;
	}
	return true;
}

//! Décode et exécute l'instruction RET.
//! RET est employé seul.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool ret(Machine *pmach, Instruction instr, unsigned addr) {
	++pmach->_sp;
	check_stack(pmach, addr);
	pmach->_pc = pmach->_data[pmach->_sp];
	return true;
}

//! Décode et exécute l'instruction PUSH.
//! PUSH supporte l'immédiat, l'adressage indexé et absolu.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool push(Machine *pmach, Instruction instr, unsigned addr) 
{
	check_stack(pmach, addr);
	if (instr.instr_generic._immediate) { // Immediat
		pmach->_data[pmach->_sp--] = instr.instr_immediate._value;
	} else {
		unsigned int address = get_address(pmach, instr);
		check_data_addr(pmach, address, addr);
		pmach->_data[pmach->_sp--] = pmach->_data[address];	
	}
	
	return true;
}

//! Décode et exécute l'instruction POP.
//! POP ne supporte pas les valeurs immédiates mais doit indiquer une adresse (absolue ou indexée).
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool pop(Machine *pmach, Instruction instr, unsigned addr) 
{
	check_immediate(instr,addr);	
	unsigned int address = get_address(pmach, instr);
	check_data_addr(pmach, address, addr);
	++pmach->_sp;
	check_stack(pmach, addr);
	pmach->_data[address] = pmach->_data[pmach->_sp];	
	return true;
}

//! Décode et exécute une instruction.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 */
bool decode_execute(Machine *pmach, Instruction instr) 
{
	unsigned addr = pmach->_pc - 1;
	switch (instr.instr_generic._cop) {
	case LOAD:
		return load(pmach, instr, addr);
	case STORE:
		return store(pmach, instr, addr);
	case ADD:
		return add(pmach, instr, addr);
	case SUB:
		return sub(pmach, instr, addr);
	case BRANCH:
		return branch(pmach, instr, addr);
	case CALL:
		return call(pmach, instr, addr);
	case RET:
		return ret(pmach, instr, addr);
	case PUSH:
		return push(pmach, instr, addr);
	case POP:
		return pop(pmach, instr, addr);
	case HALT:
		warning(WARN_HALT, addr);
		return false;
	case NOP:
		return true;
	case ILLOP:
		error(ERR_ILLEGAL, addr);
	default:
		error(ERR_UNKNOWN, addr);
	}
}

//! Affiche la trace d'une instruction.
/*!
 * \param msg message à afficher
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours d'exécution
 */
void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr) 
{
	printf("TRACE: %s: 0x%04x: ",msg, addr);
	print_instruction(instr, addr);
	printf("\n");
}
