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
*   Soubor:  express_parser.h
*   Popis:   Funkce provadejici syntaktickou analyzu vyrazu(zdola nahoru).
*
*******************************************************************************/

#ifndef __EXPRESS_PARSER_H__
  #define __EXPRESS_PARSER_H__

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>
#include <stdlib.h>        // Vstupne/vystupni operace
#include "ial.h"          //tabulka symbolu
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "parser.h"       //kvuli globalni promenne token
#include "scanner.h"      //kvuli dostavani tokenu
#include "instruction_list.h"  //kvuli seznamu instrukci
#include "udma.h"
/*===========================================================================*/
  
  
  // Ttoken token; // globalna premenna v ktorej je nacitany aktualny token  
  
  // Tprom - tabulka symbolu pro promenne s pocitadlem indexu promennych
  // Tlit - tabulka symbolu pro literaly s pocitadlem indexu literalu
  // instruction - ukazatel na ukazatel na seznam - musim ho zmenit takze je to dulezite dostavat adresu
  //             - pri chybe to bude null
  //index - promenna v tabulce symbolu ve ktere je navratova hodnota
bool expres_parser(t_table *Tprom, t_table *Tlit, instruction **instr, int *index);
  
  
#endif

/***** konec souboru****/