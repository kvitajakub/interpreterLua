/*******************************************************************************
*        VUTBR - FIT - IFJ - Interpret jazyka IFJ11
*
*   Autori:  xkvita01   Jakub Kvita
*            xcienc02   Ondrej Cienciala
*            xpecse00   Robert Pecserke
*            xkripp00   Martin Krippel
*            xpyszk02   Petr Pyzsko
*
*   Datum dokonceni:   11.12.2011
*
*   Podrobnejsi info o aktualnim rocniku projektu:
*            https://www.fit.vutbr.cz/study/courses/IFJ/public/project/
*
********************************************************************************
*   Soubor:  scanner.h
*   Popis:   Rozhrani lexikalniho analyzatoru - scanneru.
*
*******************************************************************************/

 
#ifndef __SCANNER_H__
  #define __SCANNER_H__
 
/*======= Hlavickove soubory =================================================*/
#include <stdio.h>        // Vstupne/vystupni operace
#include <stdlib.h>       // Obecne funkce, pamet
#include <ctype.h>        // Makra na rozpoznani znaku
#include <string.h>       // Prace s retezci
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "udma.h"
/*===========================================================================*/


/*
 * Typy tokenu
 */
#define NONE        -1            //  Implicitni hodnota
#define END_OF_FILE  0            //  Konec souboru
#define ID           1            //  Identifikator
#define NUMBER       2            //  Ciselny literal
#define BOOL         3            //  Boolean
#define STRING       4            //  Retezcovy literal
// Aritmeticke operatory
#define PLUS         10           //  +
#define MINUS        11           //  -
#define MUL          12           //  *
#define DIV          13           //  /
#define POW          14           //  ^
#define MOD          15           //  mod
// Prirazeni
#define ASSIGN       20           //  =
// Relacni operatory
#define EQ           30           //  ==
#define NE           31           //  ~=
#define LT           32           //  <
#define GT           33           //  >
#define LE           34           //  <=
#define GE           35           //  >=
// Zavorky
#define L_BRACKET    40           //  (
#define R_BRACKET    41           //  )
// Strednik
#define SEMICOLON    50           //  ;
// Konkatenace
#define CONC         60           //  ..
// Carka
#define COMMA        65           //  ,
// Klicova slova
#define DO           70
#define ELSE         71
#define END          72
#define FUNCTION     73
#define IF           74
#define LOCAL        75
#define NIL          76
#define READ         77
#define RETURN       78
#define THEN         79
#define WHILE        80
#define WRITE        81
// Rezervovana slova
#define RESERVED     100
/*
#define AND          100
#define BREAK        101
#define ELSEIF       102
#define FOR          103
#define IN           104
#define NOT          105
#define OR           106
#define REPEAT       107
#define UNTIL        108
*/


/*
 * Struktura tokenu
 */
typedef struct ttoken {
  int type;        // Typ tokenu
  unsigned line;   // Radek, na kterem se token vyskytl
  char *attr;      // Ukazatel na retezec, ve kterem je ulozen atribut
                   // Nikdy neni NULL, pamet je vzdy alokovana
} Ttoken;




/*===========================================================================*/
/*
 *  Prototypy globalnich funkci
 */

/*---------------------------------------------------------------------------
 * Nastaveni zdrojoveho souboru do globalni promenne
 * Nutne zavolat ve funkci, ktera pozdeji pouziva funkci get_token()
 */
void source_file_set(FILE *f);


/*---------------------------------------------------------------------------
 * Uvolni pamet pro atribut tokenu
 * Token bude ve stejnem stavu, jako po inicializaci
 *
 * Neuvolňuje primo strukturu tokenu, protoze se predpoklada, ze ta je alokovana
 * staticky
 */
void token_UDMA_free(Ttoken *token);


/*---------------------------------------------------------------------------
 * Scanner - lexikalni analyzator
 * Prochazi zdrojovym kodem a vraci token
 */
Ttoken get_token();
/*===========================================================================*/


#endif

/***** konec souboru****/
