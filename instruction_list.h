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
*   Soubor:  instruction_list.h
*   Popis:   Seznam instrukci a funkce pracujici s nim pro vztvoreni a praci.
*
*******************************************************************************/

#ifndef __INSTRUCTION_LIST_H__
  #define __INSTRUCTION_LIST_H__

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>        // Vstupne/vystupni operace
#include <stdlib.h>       // Obecne funkce, pamet
#include <string.h>       // Prace s retezci
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "scanner.h"
#include "udma.h"
/*===========================================================================*/


/* instrukcie */

//ZAKOMENTOVANE INSTRUKCE EXITUJI ALE UZ jSOU JEJICH NAZVY DEFINOVANE V SCANNERU.H



#define READ_N          330     // [a1,,] nacita number
#define READ_L          331     // [a1,,] nacita string, vsetky cisla do konca riadku
#define READ_A          332     // [a1,,] nacita string, vsetky cisla do EOF
#define READ_NUMB       333     // [a1,poctet_znakov,] nacita string, dany pocet znakov
// #define WRITE           302   // [,,a3]
#define LABEL           303   // [lab,,]
#define IFNOTGOTO          304   // [a1,,lab]    in  -   label
#define GOTO            307   // [,,lab]      -   -   label

#define NOT             305   // [a1,,a3]     in  -   out     (a1 = 0 => a3 = 1; a1 = vsetko okrem 0 => a3 = 0)
#define COPY            306   // [a1,,a3]     in  -   out     (do a3 priradi a1)

// #define PLUS            307   // [a1,a2,a3]   in  in  out     (a3 = a1 + a2)
// #define MINUS           308   // [a1,a2,a3]   in  in  out     (a3 = a1 + a2)
// #define POW             309   // [a1,a2,a3]   in  in  out     (a3 = a1 ^ a2)
// #define MUL             310   // [a1,a2,a3]   in  in  out     (a3 = a1 * a2)
// #define DIV             311   // [a1,a2,a3]   in  in  out     (a3 = a1 / a2)
// #define MOD             312   // [a1,a2,a3]   in  in  out     (a3 = a1 mod a2)  -- mod je operacia modulo
                          // porovnavanie v a3 bude 0 alebo 1 v zavislosti od vysledku operacie, 1 == plati; 0 == neplati
// #define LT              313   // [a1,a2,a3]   in  in  out     (a3 <- a1 < a2)
// #define LE              314   // [a1,a2,a3]   in  in  out     (a3 <- a1 <= a2)
// #define GT              315   // [a1,a2,a3]   in  in  out     (a3 <- a1 > a2)
// #define GE              316   // [a1,a2,a3]   in  in  out     (a3 <- a1 >= a2)
// #define EQ              317   // [a1,a2,a3]   in  in  out     (a3 <- a1 == a2)

#define SUBSTR_FIND     318   // [,,]   -  -  -     (a3 pozicia najdeneho podretazca, a1 retazec, a2 podretazec)
// #define CONC            319   // [a1,a2,a3]   -  -  -     (a3 = a1 + a2; vsetko su to retazce)
#define SORT            320   // [,,]     -  -   -     (a1 neusporiadany retazec, a3 uspoiadany)
#define TYPE            321   // [,,]     -  -   -     (out = string)
#define SUBSTR_CUT      322   // [,,]     -   -   -                      ()

#define CALL_FUN        323   // [a1,,]       in  -   -       (a1 je volana funkcia)
// #define RETURN          324   // [,,a3]       -   -   out     (a3 je navratova hodnota)

#define QUE_UP          325   // [a1,,]       in  -   -       (vlozenie prvku do fronty, a1 == prvok)
#define QUE_DOWN        326   // [,,a3]       -   -   out     (odobratie prvku z fronty a ulozenie prvku na a3)
#define QUE_CLEAN       327   // [,,]         -   -   -       (vycisti frontu)

// #define LOCAL           328   // [,,a3]       -   -   out     (vlozenie premennej do tabulky premennych - interpret)

/* koniec instrukcii */

typedef struct INSTRUCTIONS{
  int type;   //typ instrukce
  int op1;    //prvni operand
  int op2;    //druhy operand
  int op3;    //treti operand
  struct INSTRUCTIONS *next;   //ukazatel na dalsi instrukci
} instruction;

typedef struct FUNCT{
  char *name;  //jmeno funkce tak jak je v programu
  instruction *instr;  //seznam instrukci
} func;

typedef struct FTABLE{
  func **function; //pole ukazatelu na funkci ktere se bude delat UDMA_reallocem
  int count;  //ktery index je posledni alokovany - kam se ma pridavat dalsi fce
              //a kde je main - na pozici FTable[count]
}FTable;


/*======= Prototypy funkci ===================================================*/
int LabelIndex();  //vrati index navesti
void FT_init(FTable *Ftable);   //inicializuje tabulku
int FT_addfunc(FTable *Ftable,char *name);   //prida prvek do pole pro funkci s nazvem name
int FT_findfunc(FTable *Ftable,char *name);  //vrati index na kterem je funkce s nazvem name
instruction *makeinstr(int type,int op1,int op2,int op3);  //alokuje pamet pro instrukci a naplni ji
int FT_concatinstr(instruction *inst1,instruction *inst2); //spoji dva seznamy instrukci/prida instrukci na konec
void instrUDMA_free(instruction *instr); //zrusi cely seznam instrukci
void FT_destroy(FTable *Ftable); //zrcusi celou tabulku

void printinstr(instruction *instr); //vytiskne jednu instrukci
void printinstrlist(instruction *instr); //vytiskne cely seznam instrukci
void printfunc(func *function); //vztiskne jednu funkci
/*============================================================================*/



#endif

/***** konec souboru****/
