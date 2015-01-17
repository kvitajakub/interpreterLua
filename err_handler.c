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
*   Soubor:  err_handler.c
*   Popis:   V tomoto souboru jsou implementovany funkce osetrujici chybove
*            stavy.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>
#include "err_handler.h"  // Vlastni hlavickovy soubor, chybove konstanty
/*============================================================================*/


/*
 * Promenna priznaku chyby
 */ 
int err_flg = OK;


char *errs[]={
  "Vse v poradku.\n",
  "Lexikalni chyba.\n",
  "Syntakticka chyba.\n",
  "Semanticka chyba.\n",
  "Interpretacni chyba.\n",
  "Vnitrni chyba.\n"
};

void print_err(){

  fprintf(stderr,"%s",errs[err_flg]);
}
