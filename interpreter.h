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
*   Soubor:  interpreter.h
*   Popis:   Funkce interpretujici seznam instrukci vznikly v parseru.
*
*******************************************************************************/

#ifndef __INTERPRETER_H__
  #define __INTERPRETER_H__

/*======= Hlavickove soubory =================================================*/
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "scanner.h"
#include "ial.h"
#include "instruction_list.h"
#include "udma.h"
/*============================================================================*/
void interpret(FTable *fTable, t_table *tLit); //funkce pro spusteni interpretu


#endif

/***** konec souboru****/
