#ifndef _MACHINE_H_
#define _MACHINE_H_

/*!
 * \file machine.h
 * \brief Description de la structure du processeur et de sa mémoire
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//! Nombre de resitres généraux
#define NREGISTERS 16

//! Code condition
/*! 
 * Le code condition donne le signe du résultat de la dernière instruction
 * arithmétique (addition, soustraction) exécutée par le processeur. 
 *
 * \note Dans un processeur réel, le code condition fait partie du mot
 * d'état du processeur. Il contient d'autres indications que le simple
 * signe du résultat : par exemple le fait qu'une retenue ait été générée
 * ou bien qu'un dépassement de capacité (\e overflow) se soit produit.
 */
typedef enum 
{
    CC_U = 0,	//!< Signe du résultat inconnu
    CC_Z,	//!< Résultat nul
    CC_P,	//!< Résultat positif
    CC_N,	//!< Résultat négatif
} Condition_Code;

//! Dernière valeur possible du code condition
static const unsigned LAST_CC = CC_N;

//! Taille minimale de la pile d'exécution
static const unsigned MINSTACKSIZE = 10;

#include <stdbool.h>
#include <stdint.h>

//! Codes opérations
typedef enum 
{
    ILLOP = 0,	//!< Instruction illégale
    NOP,	//!< Instruction sans effet
    LOAD,	//!< Chargement d'un registre
    STORE,	//!< Rangement du contenu d'un registre 
    ADD,	//!< Addition à un registre 
    SUB,	//!< Soustraction d'un registre 
    BRANCH,	//!< Branchement conditionnel ou non  
    CALL,	//!< Appel de sous-programme 
    RET,	//!< Retour de sous-programme 
    PUSH,	//!< Empilement sur la pile d'exécution 
    POP,	//!< Dépilement de la pile d'exécution
    HALT,	//!< Arrêt (normal) du programme
} Code_Op;

//! Dernière valeur possible du code opération
const static unsigned LAST_COP = HALT;

typedef union Instruction
{ 
  //! Format brut : un mot de 32 bits
  uint32_t _raw;

  //! Format générique : les premiers champs sont communs
  struct 
  { 
    Code_Op _cop : 6; 	//!< Code opération 
    bool _immediate : 1;	//!< Adressage immédiat ?
    bool _indexed : 1;	//!< Adressage indirect ?
    unsigned _regcond : 4;	//!< Numéro de registre ou condition
    unsigned _pad : 20;     //<! Format variable...
  } instr_generic;

  //! Format d'une instruction à adressage absolue
  struct 
  {
    Code_Op _cop : 6; 	//!< Code opération
    bool _immediate : 1;	//!< Adressage immédiat ?
    bool _indexed : 1;	//!< Adressage indirect ?
    unsigned _regcond : 4;	//!< Numéro de registre ou condition
    unsigned _address : 20;	//!< Adresse absolue
  } instr_absolute;

    //! Format d'une instruction à valeur immédiate
  struct 
  {
    Code_Op _cop : 6; 	//!< Code opération
    bool _immediate : 1;	//!< Adressage immédiat ?
    bool _indexed : 1;	//!< Adressage indirect ?
    unsigned _regcond : 4;	//!< Numéro de registre ou condition
    signed int _value : 20;	//!< Valeur immédiate
  } instr_immediate;

  //! Format d'une instruction à adressage indéxé
  struct 
  {
    Code_Op _cop : 6; 	//!< Code opération
    bool _immediate : 1;	//!< Adressage immédiat ?
    bool _indexed : 1;	//!< Adressage indirect ?
    unsigned _regcond : 4;	//!< Numéro de registre ou condition
    unsigned _rindex : 4;   //!< Numéro du registre d'index
    signed int _offset : 16;//!< Déplacement
  } instr_indexed;

} Instruction;

typedef enum 
{
    NC, //!< Pas de condition (nrachement inconditionnel)
    EQ,	//!< Égal à 0
    NE,	//!< Différent de 0
    GT, //!< Strictement positif
    GE,	//!< Positif ou nul
    LT, //!< Strictement négatif
    LE, //!< Négatif ou null
} Condition;

//! Dernière valeur possible d'une condition
static const unsigned LAST_CONDITION = LE;

//! Type d'un mot de donnée
typedef uint32_t Word;

//! Forme imprimable des codes opérations
extern const char *cop_names[];

//! Forme imprimable des conditions
extern const char *condition_names[];

typedef struct
{
    // Segments de mémoire
    Instruction *_text;		//!< Mémoire pour les instructions
    unsigned int _textsize;	//!< Taille utilisée pour les instructions

    Word *_data;		//!< Mémoire de données
    unsigned int _datasize;	//!< Taille utilisée pour les données

    unsigned int _dataend;      //!< Première adresse libre après les données statiques

    // Registres de l'unité centrale
    unsigned _pc;		//!< Compteur ordinal
    Condition_Code _cc;		//!< Code condition : signe de la dernière opération
    Word _registers[NREGISTERS];//!< Registres généraux (accumulateurs)

//! Définition de _sp comme synonyme du registre R15    
#   define _sp _registers[NREGISTERS - 1] 
} Machine;

#endif
