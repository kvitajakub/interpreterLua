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
*   Soubor:  main.c
*   Popis:   Spusteni prekladace, nacteni souboru, volani funkci.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>        // Vstupne/vystupni operace
#include <stdlib.h>       // Obecne funkce, pamet
#include "scanner.h"      // Lexikalni analyzator - scanner
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "ial.h"          // funkce z ial
#include "parser.h"
#include "express_parser.h"
#include "instruction_list.h"
#include "interpreter.h"
#include "udma.h"
/*============================================================================*/


int main(int argc, char *argv[]) {

  FILE *f;
  
  if (argc != 2){
    err_flg=ERR_INT;  //nastaveni promenne chyby
  }
  else{
    //spravny pocet parametru
    f = fopen(argv[1],"r");
    if (f == NULL)
      //nepodarilo se otevrit-rychle na konec
      err_flg=ERR_INT;  
      
    else
      source_file_set(f); // Nastaveni zdrojaku pro scanner
  }
  
  if(err_flg == OK){
    
    //spravne zadane parametry a otevreny soubor, muzeme neco delat
    FTable fTable;                  //funkcni tabulka
    FT_init(&fTable);
    t_table *tLit = htable_init();  //instance tabulky symbolu pro literaly
    
    if(prog(&fTable, tLit)){                                          //<<PARSER
      // povedla se analyza programu takze jej muzeme interpretovat  
      
      // tisk instrukci - PAK ZAKOMENTOVAT
      // for(int i=0;i<=fTable.count;i++){
        // printinstrlist(fTable.function[i]->instr);
        // printf("=============\n");
      // }
      
      interpret(&fTable, tLit);                                    //<<INTERPRET
    }
        
    //uzavreni vseho
    htable_UDMA_free(tLit);
    FT_destroy(&fTable);
    fclose(f);
  
  }//konec aktivni cinnosti programu
  
  //uvolneni veskere pameti
  UDMA_dispose();
  
  if (err_flg != OK)  //jestli je chyba tak tisknu
    print_err();
  return err_flg;    //navrat vzdy
}

/***** konec souboru****/
