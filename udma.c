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
*   Soubor:  udma.c
*   Popis:   V tomoto souboru je implementovano uzivatelske pridelovani pameti
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

/*======= Hlavickove soubory ================================================*/
#include "udma.h"
/*===========================================================================*/



/*
 * Struktura prvku UDMA seznamu
 */
typedef struct tUDMA_elem {
  struct tUDMA_elem *next;
  void *memory;
} TUDMA_elem;



/*
 * Globalni ukazatel na 1. prvek seznamu UDMA
 */
TUDMA_elem *first_UDMA_elem = NULL;



/*===========================================================================*/
/*
 *  Prototypy lokalnich funkci
 */
static void UDMA_free2(void *ptr);
/*===========================================================================*/



/*
 * UDMA funkce chovajici se jako malloc()
 *
 * @param size - velikost pozadovane pameti (v pripade potreby lze zmenit typ 
 *               na unsigned long long)
 */
void *UDMA_malloc(unsigned size)
{
  void *memory_ptr = malloc(size);
  TUDMA_elem *new_UDMA_elem_ptr = malloc(sizeof(TUDMA_elem));

  if (memory_ptr == NULL || new_UDMA_elem_ptr == NULL) { // Alokace se nezdarila
    // fprintf(stderr, "UDMA: Nedostatek pameti!\n");
    free(memory_ptr);   // Cast alokace se mohla zdarit, nutno uvolnit
    free(new_UDMA_elem_ptr);
    return NULL;
  }

  new_UDMA_elem_ptr->memory = memory_ptr;
  new_UDMA_elem_ptr->next = first_UDMA_elem;
  first_UDMA_elem = new_UDMA_elem_ptr;
  return memory_ptr;
}




/*
 * UDMA funkce chovajici se jako free()
 */
void UDMA_free(void *ptr)
{
    ptr = ptr; 
    
    // NECHAT ZAKOMENTOVANE - riesi to problem konfliktov pri uvolnovani
    
    
    
/*  if (ptr == NULL)
    return;

  if (first_UDMA_elem == NULL) { // Prvek neni v seznamu (seznam je prazdny), PODEZRELE
    //fprintf(stderr, "UDMA: Uvolneni pameti, ktera nebyla v UDMA seznamu!\n");
    //free(ptr);
    return;
  }

  TUDMA_elem *temp_ptr = first_UDMA_elem;
  TUDMA_elem *free_elem_ptr = NULL;

  if (temp_ptr->memory == ptr) {  // Zpracovani 1. prvku
    first_UDMA_elem = temp_ptr->next;
    free(temp_ptr);
    free(ptr);
    return;
  }

  int found = 0;  // Nanelezeno

  while (temp_ptr->next != NULL) {  // Prochazej seznam UDMA
    if (temp_ptr->next->memory == ptr) {  // Nasli jsme prvek k vymazani
      free_elem_ptr = temp_ptr->next;
      temp_ptr->next = temp_ptr->next->next;
      free(free_elem_ptr);
      free(ptr);
      found = 1;  // Nalezeno
      break;
    }
    temp_ptr = temp_ptr->next;
  }

  if (!found) {   // Prvek jsme v seznamu nenasli, PODEZRELE
    fprintf(stderr, "UDMA: Uvolneni pameti, ktera nebyla v UDMA seznamu!\n");
    free(ptr);
  }*/
}




/*
 * Chova se podobne jako UDMA_free(), ale neuvolnuje data (uzivatelem
 * pozadovanou pamet) prvku
 */
static void UDMA_free2(void *ptr)
{
  if (ptr == NULL)
    return;

  if (first_UDMA_elem == NULL) { // Prvek neni v seznamu (seznam je prazdny), PODEZRELE
    fprintf(stderr, "UDMA: Uvolneni pameti, ktera nebyla v UDMA seznamu!\n");
    return;
  }

  TUDMA_elem *temp_ptr = first_UDMA_elem;
  TUDMA_elem *free_elem_ptr = NULL;

  if (temp_ptr->memory == ptr) {  // Zpracovani 1. prvku
    first_UDMA_elem = temp_ptr->next;
    free(temp_ptr);
    return;
  }

  int found = 0;  // Nanelezeno

  while (temp_ptr->next != NULL) {  // Prochazej seznam UDMA
    if (temp_ptr->next->memory == ptr) {  // Nasli jsme prvek k vymazani
      free_elem_ptr = temp_ptr->next;
      temp_ptr->next = temp_ptr->next->next;
      free(free_elem_ptr);
      found = 1;  // Nalezeno
      break;
    }
    temp_ptr = temp_ptr->next;
  }

  if (!found) {   // Prvek jsme v seznamu nenasli, PODEZRELE
    fprintf(stderr, "UDMA: Uvolneni pameti, ktera nebyla v UDMA seznamu!\n");
  }
}




/*
 * UDMA funkce chovajici se jako realloc()
 *
 * @param size - velikost pozadovane pameti (v pripade potreby lze zmenit typ 
 *               na unsigned long long)
 */
void *UDMA_realloc(void *ptr, unsigned size)
{
  if (ptr == NULL) {  // Stejne chovani jako malloc()
    return UDMA_malloc(size);
  }

  if (size == 0) {  // Ptr != NULL, size == 0 => stejne chovani jako free()
    UDMA_free(ptr);
    return NULL;
  }

  void *temp_ptr = ptr;
  ptr = realloc(ptr, size);
  if (ptr == NULL) {       // Nezdarilo se, pamet je jiz uvolnena
    UDMA_free2(temp_ptr);  // Pouze odstran prvek z UDMA seznamu, ale neuvolnuj pamet
    return NULL;
  }

  if (ptr != temp_ptr) {   // Zmenil se puvodni ukazatel
    UDMA_free2(temp_ptr);  //  Odstraneni stareho prvku z UDMA seznamu
    // Vytvoreni noveho prvku UDMA seznamu a vlozeni ukazatele do nej
    TUDMA_elem *new_UDMA_elem_ptr = malloc(sizeof(TUDMA_elem));

    if (new_UDMA_elem_ptr == NULL) {
      // fprintf(stderr, "UDMA: Nedostatek pameti!\n");
      return NULL;
    }

    new_UDMA_elem_ptr->memory = ptr;
    new_UDMA_elem_ptr->next = first_UDMA_elem;
    first_UDMA_elem = new_UDMA_elem_ptr;
  }

  // Pokud se ukazatel nezmenil, neni treba nic delat

  return ptr;
}




/*
 * Uvolni veskerou alokovanou pamet a cely seznam
 */
void UDMA_dispose()
{
  TUDMA_elem *temp_ptr;

  while (first_UDMA_elem != NULL) {
    free(first_UDMA_elem->memory);
    temp_ptr = first_UDMA_elem;
    first_UDMA_elem = first_UDMA_elem->next;
    free(temp_ptr);
  }

  first_UDMA_elem = NULL;
}



/***** konec souboru****/
