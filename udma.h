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
*   Soubor:  udma.h
*   Popis:   Rozhrani UDMA - uzivatelskeho pridelovani pameti.
*
*            UDMA == User Dynamic Memory Allocation
*
*     UDMA funkce zde slouzi jako obalovaci funkce pro malloc(), realloc() a
*     free(), pricemz navic vytvari jednosmerny linearni seznam. Pri kazdem
*     alokovani pameti se vlozi na zacatek seznamu novy prvek obsahujici
*     ukazatel na pozadovanou pamet. Pri uvolnovani pameti se prochazi seznam
*     od zacatku a smaze se prvek, ktery obsahoval dany ukazatel.
*     Pokud se zavola funkce UDMA_dispose(), uvolni se veskera pamet a cely
*     seznam.
*
*******************************************************************************/


#ifndef __UDMA_H__
  #define __UDMA_H__

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>        // Vstupne/vystupni operace
#include <stdlib.h>       // Obecne funkce, pamet
/*===========================================================================*/




/*===========================================================================*/
/*
 *  Prototypy globalnich funkci
 */

/*
 * UDMA funkce chovajici se jako malloc()
 *
 * @param size - velikost pozadovane pameti (v pripade potreby lze zmenit typ 
 *               na unsigned long long)
 */
void *UDMA_malloc(unsigned size);


/*
 * UDMA funkce chovajici se jako realloc()
 *
 * @param size - velikost pozadovane pameti (v pripade potreby lze zmenit typ 
 *               na unsigned long long)
 */
void *UDMA_realloc(void *ptr, unsigned size);


/*
 * UDMA funkce chovajici se jako free()
 */
void UDMA_free(void *ptr);


/*
 * Uvolni veskerou alokovanou pamet a cely seznam
 */
void UDMA_dispose();



#endif

/***** konec souboru****/
