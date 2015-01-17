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
*   Soubor:  parser.h
*   Popis:   Rozhrani syntaktickeho a semantickeho analyzatoru
*
*******************************************************************************/
#include <stdbool.h>
#include "scanner.h"
#include "instruction_list.h"
#include "ial.h"

#ifndef __PARSER_H__
  #define __PARSER_H__
  
#include "udma.h"


Ttoken token; // globalna premenna v ktorej je nacitany aktualny token  
  
void loadToken();  
bool prog(FTable *fTable, t_table *tLit);


#endif

/***** konec souboru****/