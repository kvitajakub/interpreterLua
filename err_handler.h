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
*   Soubor:  err_handler.h
*   Popis:   Rozhrani err_handleru, deklarace chybovych konstant.
*
*******************************************************************************/

#ifndef __ERR_HANDLER_H__
  #define __ERR_HANDLER_H__

#include "udma.h"

/*
 * Promenna priznaku chyby
 */
extern int err_flg;


/*
 * Chybove priznaky
 */
#define OK          0       // V poradku
#define ERR_LEX     1       // Lexikalni chyba
#define ERR_SYN     2       // Syntakticka chyba
#define ERR_SEM     3       // Semanticka chyba
#define ERR_ITP     4       // Interpretacni chyba
#define ERR_INT     5       // Vnitrni chyba

void print_err();

#endif
