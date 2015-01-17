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
*   Soubor:  ial.h
*   Popis:   Rozhrani funkci:
*             - Boyer-Mooreuv algoritmus - vyhledavani podretezce v retezci
*             - Merge sort - razeni
*             - dalsi vestavene funkce jazyka IFJ11
*
*******************************************************************************/

#ifndef __IAL_H__
  #define __IAL_H__

//___________________ VESTAVENE FUNKCE _______________________// 
//                                                            //
//_____________________ FIND__________________________________//
int find_c(char *text,char* vzor); // vyhleda vzor v textu
//______________________SORT__________________________________//
void sort_c(char *s);  // seradi posloupnost znaku dle ascii
//_____________________SUBSTR_________________________________//
char * substr_c(char *s,int start , int end); 
// vyreze z retezce s
// podretezec od indexu start az po index end
//_____________________________________________________________//

//___________________FUNKCE PRO TABULKU SYMBOLU________________//
#include<stdbool.h>
#include "udma.h"

#define TABLE_SIZE 7 // velikost tabulky

typedef struct htable  // tabulka hashovaci
{

    unsigned int size;
    // velikost pole ukazatelu

    int count; // pocitadlo
    
    struct symbol** tab;
    // pole ukazatelu
}t_table;

// TYPY PROMENNYCH
#define T_ZARAZKA 666 // pro oddeleni

#ifndef STRING      // vyjmuto se scaneru  
    #define NUMBER       2            //  Ciselny literal
    #define BOOL         3            //  Boolean
    #define STRING       4            //  Retezcovy literal
    #define NIL          76    
#endif
///////////////
// unie pro hodnotu narveme do ni co budeme chtit
// vytvorit staticky a mrdnout do funkce pridej_symbol
// nebo pridej_literal
typedef union value  
{
    char *str;
    double num;
    bool flag;
}t_value;

// struktura predstavujici symbol v TS
typedef struct symbol
{

    char *jmeno;
    int typ;
    int index;
    union  value hodnota;
    struct symbol *next;

}t_symbol;

/////////////////////////////////////////////////////////
///________________FUNKCE____________________________//
///////////////////////////////////////////////////////

//_______________ PRINT TABLE _______________________//
// vytiskne tabulku symbolu
void print_ts(struct htable* tabulka);
//___________________ TYPE__________________//
// vrati nam integer s hodnotou daneho typu dle
// #define v scanneru
char *type_c(int poradove_cislo,struct htable* TS_prom,struct htable* TS_liter);
// _____________PRIDEJ SYSTEMOVOU_____________//
// prida systemovou promennou do TS a vrati jeji index
int pridej_systemovou(struct htable * tabulka);
//___________ PRIDEJ LOCAL____________________//
//  urceno pro instrukci LOCAL x
//  prida nam do TS promenou s danym cislem x do TS
void pridej_local(int cislo, struct htable* tabulka);
//_________________NAHREJ HODNOTU____________//
// prida hodnotu x do promenne s cislem cislo
// urceno pro instrukce modifikujici promenne
void nahrej_hodnotu(t_value x,int cislo,int typ_hodnoty,struct htable *tabulka);
//______________ PRIDEJ LITERAL_____________________//
// specialni funkce pro pridani literalu
// staci ji jen predhodit tabulku hodnotu v unii a typ (dle tokenu)
// vrati nam index - poradove zaporne cislo literalu
int pridej_literal(struct htable *tabulka,t_value hodnota,int typ);
//______________PRIDEJ PROMENNOU_________________//
//
// obdobna funkce jako literal
// pripadne vlozte prazdnou hodnotu
int pridej_promennou(struct htable *tabulka,char *jmeno,t_value hodnota,int typ);
// hashovaci funkce ///////////////    -HASH-      ////////////////////
// z IJC
unsigned int hash_function(const char *str, unsigned int htable_size);
//// MUJ ITOA (na linuxu neni funkce itoa asi)
char * my_itoa(int hodnota);
/////////////////////    VYTVORENI  TABULKY   //////////////////////////////////
// vytvoreni a inicializace tabulky o zadane velikosti
// navraci ukazatel na tabulku
struct htable* htable_init();
//////////////////////////   CLEAR ////////////////////////////////
// vymaze vsechny symboly z tabulky a zbyde nam jen samotna
// tabulka symbolu
void htable_clear(struct htable* tabulka);
////////////////////////////   UDMA_freeE   ///////////////////////////////////////
//   zruseni tabulky symbolu nejprve smaze vsechny symboly
//   a pak celou tabulku symbolu posle do pekelnych bran
void htable_UDMA_free(struct htable* tabulka);
//___________________ VYHLEDEJ PROMENNOU________________//
//  vyhleda podle jmena promennou az po zarazku nebo konec seznamu
// vrati ukazatel na polozku, nebo null
t_symbol* najdi_symbol(struct htable* tabulka,char* jmeno);
//_______________ PRIDEJ PROMENNOU___________________________//
// prida promennou s danym typem a hodnotou do tabulky
// pokud jiz existuje takova prommenna v dane urovni tak se hodnota prepise
// => pri deklaraci kontrolovat jestli již existuje s danym jmenem promenna
// danou urovni se rozumi v oblasti pred zarazkou
// v pripade spatne alokace vraci NULL
t_symbol * pridej_symbol(struct htable* tabulka,char* jmeno,int typ,union value hodnota);
//_________________ PRIDEJ ZARAZKY______________________________//
// prida zarazky pro oddeleni urovni
// v pripade problemu s pameti modifukuje err_flg
void pridej_zarazky(struct htable *tabulka);
//________________ UVOLNI ZARAZKY ________________//
// uvolni nam nejvyssi uroven zarazek
// tj. vsechny symboly po zarazky
// !! nevolat pokud nejsou vytvoreny zarazky
void uvolni_zarazky(struct htable *tabulka);
//____________________________________________________//
#endif
