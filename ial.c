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
*   Soubor:  ial.c
*   Popis:   V tomoto souboru je implementovana metoda vyhledavani podretezce
*            v retezci (Boyer-Mooreuv algoritmus), metoda razeni (Merge sort),
*            tabulka symbolu (hashovaci tabulka) a dalsi vestavene funkce
*            jazyka IFJ11.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "ial.h"
/*============================================================================*/


// _________ SUBSTR____________________________________________________________//
//_____________________________________________________________________________//
// funkce vyhledá v retezci a vrátí ukazatel na nově alokovaný retezec         //
// !!! nutno pote dealokovat !!!                                               //
// TODO:(pred zavolanim funkce) pri spatne zadanych parametrech vracet nil     //
// - nadbytecne parametry ignorovat                                            //
// - desetinnou cast cisla odrezat                                             //
//_____________________________________________________________________________//

char * substr_c(char *s,int start , int end)
{
    int new_len=1; // delka podretezce
    // implicitne 1 kvuli testu na prazdny retezec


    // - zaporne pozice prepocitame na pozice od konce
    // string cislovan od 1

    if (start<0)
        start=(strlen(s)+1)+start;
    if(end<0)
        end=(strlen(s)+1)+end;


    // kontrola jestli nemame konec v nule nebo nechceme cist mimo retezec
    if (end <1 || (start>(int)strlen(s) && end>(int)strlen(s)))
        new_len=0; // automaticky vracime prazdny retezec => delka 0

    // dalsi kontrola jestli jsme zacatkem uvnitr stringu
    if(start<1)
        start=1;   // na zacatek


    //  chceme zacinat za koncem?
    if(start>(int)strlen(s))
        new_len=0;  // automaticky vracime prazdny retezec => delka 0

    // pokud chceme cist za konec => cteme na konec
    if(end>(int)strlen(s))
        end=strlen(s);

    // vypocet delky noveho retezce
    if (new_len !=0 )
        new_len=(end-start)>=0 ? end-start+1 : 0;


    char *new_s=UDMA_malloc((new_len+1)*sizeof(char)); // alokace mista pro retezec + /0

    if(new_s==NULL)  // chybny UDMA_malloc
    {
        err_flg=ERR_INT;  // globalni chyba
        return NULL;   
    }
    for (int i=0;i<new_len;i++,start++)   // nokopirovani podretezce
    {
        new_s[i]=s[start-1];
    }

    // pridani /0 do noveho retezce
    new_s[new_len]='\0';

    // vracime string
    return new_s;

}



//________________M E R GE  S O R T________________//
//_________________________________________________//
//                                                 //
// seradi znaky v retezci dle ascii                //
// radicim algoritmem merge sort                   //
//_________________________________________________//


//___________________MERGE________________________//
// slevani dvou serazenych posloupnosti
// start-middle -1.posloupnost
// middle-end -2 posloupnost
void merge(char* s,char * pole ,int start,int middle,int end)
{
    // kopirovani do pomocneho pole
    for (int i=start;i<=end;i++)
        pole[i]=s[i];


    // i1- index do pomocneho pole (prvni serazena posloupnost)
    // j-index do puvodniho kde budem z5 kopirovat
    int i1=start;
    int j=start;
    int i2=middle+1;  // index druhe serazene posloupnosti (pomocne pole)

    while (i1<=middle && i2<=end)   // dokud nevycerpame alespon 1 polsoupnost
    {

        if (pole[i1] <= pole[i2])            // vybereme vetsi prvek a vlozime do puvodniho pole
            s[j++]=pole[i1++]; //inexy inkrementujeme
        else
            s[j++]=pole[i2++];
    }


        while(i1<=middle)    // zkopirovani zbytku do pole
            s[j++]=pole[i1++];

        while(i2<end)
            s[j++]=pole[i2++];
}


//____________________MERGE SORT_________________________//
// funkce rekurzivni metodou postupne puli pole a serazuje je
// pulky pak sleje dohoromady funkci merge
//
void merge_sort(char*s,char*pole,int start, int end)
{

    // vypocet prostredniho indexu
    int middle=(start+end) / 2;

    // je uz serazeno cele pole?
    if(start<end)  // podminka rekurze
    {
        merge_sort(s,pole,start,middle);   // rekurzivni volani na 2 podpole
        merge_sort(s,pole,middle+1,end);
        merge(s,pole,start,middle,end);    // slevani 2 serazenych posloupnosti
    }
}


//____________________SORT_C________________________//
// hlavni funkce
// funkce alokuje pomocne pole zavola razeni merge sort
// to seradi pole s a pomocne pole pote dealokuje
//
void sort_c(char *s)
{
    // pomocne pole pro razeni
    char *pole=UDMA_malloc(strlen(s)*sizeof(char));

    if (pole==NULL)   //nastala chyba
    {
        err_flg=ERR_INT;
        return;
    }
        

   int end=strlen(s)-1;  // index posledniho prvku pole
   int start=0; // zacatecni index

    // zavolani serazeni
    merge_sort(s,pole,start,end);




    UDMA_free(pole);
}

// ____________ F I N D__________________________________//
//_______________________________________________________//
//                                                       //
// vyhleda v retezci text podretezec vzor pomocí         //
// Boyer-Moorova algoritmu                               //
//                                                       //
// TODO: v pripade špatnych parametru vracet nil         //
//                                                       //
// -1 ==false                                            //
// prazdny retezec je vždy na pozici 0                   //
//_______________________________________________________//


#define CHARS 256 // pocet znaku v ascii


//____________________LAST______________________________//
//
// pomocna funkce pro Boyer-moroov algoritmus
//
void last_f(int *last, char *vzor)
{
    for(int i=0;i<CHARS;i++) // inicializace -1 -> neobsahuje
        last[i]=-1;

    for (int i=0;i<(int)strlen(vzor);i++)  // naplneni tabulky last
        last[(int)vzor[i]]=i;
}

//_________________FIND_C______________________________//
//
// Hlavni funkce vyhledavani pomoci Boyer..... algoritmu
// funkce vyuziva tabulky last, ktera se vytvori na zaklade
// vyhledavaneho textu (vzoru) 
//
int find_c(char *text,char* vzor)
{
    // alokace tabulky last
    int *last;
    if ( (last=UDMA_malloc(sizeof(int)*CHARS)) == NULL)
        {
            err_flg=ERR_INT;
            return -2;  // nesmyslna hodnota indexu -> chyba
        }


    last_f(last,vzor);  // sestaveni tabulky last pro dany vzor
    int text_delka=strlen(text);
    int vzor_delka=strlen(vzor);
    int i=vzor_delka-1;   // pocatecni index do textuchar *pole=UDMA_malloc(strlen(s)*sizeof(char));
    int j=i;                    // pocatecni index do vzoru

    if (i  > text_delka-1 )  // vzor je vetsi nez text
        {
            UDMA_free(last);
            return -1;  // nenalezeno
        }

    if(vzor_delka==0)  // prazdny retezec nalezen na 0-tem indexu
        {
            UDMA_free(last);
            return 0;
        }

    // BOYER_MOORE

    do{

        if(vzor[j]==text[i]) // shoda?
            {
                if (j==0) // dosli jsme na zacatek => shoda
                    {
                        UDMA_free(last);  // uvolnime pamet
                        return i+1;  // vrat index +1
                    }
                else
                {
                    i--;        // zpetne hledani
                    j--;
                }
            }

        // preskakujeme znaky podle last tabulky
        else
        {
            int skok=last[(int)text[i]];  // podle tabulky last se podivame kolik muzeme skocit

            // vybereme minimalni skok a dle nej skocime
            i=i + vzor_delka - ( skok+1 < j ? skok+1 : j );

            //obnoveni j indexu
            j=vzor_delka-1;
        }


    } while( i<= text_delka-1);  // dokud nejsme na konci textu




    UDMA_free(last); // dealokace tabulky last
    return -1; // nic jsme nenasli
}
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  H A S H ///////////////////////////////////////////


//______________________ H A S H _______________________//


// hashovaci funkce ///////////////    -HASH-      ////////////////////
// z IJC
unsigned int hash_function(const char *str, unsigned int htable_size)
{
     unsigned int h=0;
     unsigned char *p;

     for(p=(unsigned char*)str; *p!='\0'; p++)
              h = 31*h + *p;

     return h % htable_size;
}
//////////////////////////////////////////////////////////////////////////


//// MUJ ITOA (na linuxu neni funkce itoa asi)
char * my_itoa(int hodnota)
{
    char *buf=UDMA_malloc(sizeof(char)*32);
    if(buf==NULL) // je to v gnoju
        return NULL;


    snprintf(buf,32,"%d",hodnota); // dupneme tam string s cislem;

    return buf;
}


/////////////////////    VYTVORENI  TABULKY   ///////////////////////////////////
// vytvoreni a inicializace tabulky o zadane velikosti
// navraci ukazatel na tabulku
//
struct htable* htable_init()
{
    struct htable* tabulka;

    if ((tabulka=UDMA_malloc(sizeof(struct htable)))==NULL) // alokujem tabulku
        {
        err_flg=ERR_INT;
        return NULL;
        }


    tabulka->size=TABLE_SIZE;  // doplnime jak je velka
    tabulka->count=0;  // pocitadlo

    // alokujeme pole ukazatelu
    tabulka->tab=UDMA_malloc(TABLE_SIZE*sizeof(struct htable_listitem*));

    if (tabulka->tab==NULL)  // pokud selhala alokace
      {
        UDMA_free(tabulka);
        err_flg=ERR_INT;
        return NULL;
      }

    unsigned int i=0;

    while(i<TABLE_SIZE)  // inicializuj na null
    {
        tabulka->tab[i]=NULL;
        i++;
    }

    #ifdef DEBUG
        puts("byla alokovana tabulka");
    #endif

    return tabulka;
}


//////////////////////////   CLEAR ////////////////////////////////
// vymaze vsechny symboly z tabulky a zbyde nam jen samotna
// tabulka symbolu

void htable_clear(struct htable* tabulka)
{
    unsigned int i=0;

    while( i < tabulka->size) // uvolneni polozek prochazi pole ukazatelu a maze
    {
       struct symbol *temp=tabulka->tab[i];  // dva ukazatele
       struct symbol *uk=tabulka->tab[i];

               while (uk!=NULL)
               {

                   //printf("typ je:%d\n",temp->typ);
                   temp=uk;  // nahrani ze zalozniho ukazatele


                    if(temp->typ==STRING)
                        UDMA_free(temp->hodnota.str);  // uvolneni hodnoty- jen pro string

                    UDMA_free(temp->jmeno);  // uvolneni stringu s nazvem

                    uk=uk->next;    // posun zalozniho ukazatele

                    UDMA_free(temp);   // vymazani polozky

               }
        i++;  // na dalsi index hashtable
    }

     #ifdef DEBUG
        puts("cistime tabulku");
    #endif
}
////////////////////////////   UDMA_freeE   ///////////////////////////////////////
//
//   zruseni tabulky symbolu nejprve smaze vsechny symboly
//   a pak celou tabulku symbolu posle do pekelnych bran
//   funkce UDMA_free(void *);
//
void htable_UDMA_free(struct htable* tabulka)
{

    htable_clear(tabulka);

    UDMA_free(tabulka->tab);  // uvolneni pole
    UDMA_free(tabulka);  // uvolnenni tabulky

    #ifdef DEBUG
        puts("byla dealokovana tabulka");
    #endif
}



//___________________ VYHLEDEJ PROMENNOU________________//
//  vyhleda podle jmena promennou az po zarazku nebo konec seznamu
// vrati ukazatel na polozku, nebo null

t_symbol* najdi_symbol(struct htable* tabulka,char* jmeno)
{
    unsigned int index=hash_function(jmeno,tabulka->size);
    // index ktery nam vrati hashfunkce

    struct symbol  *p=tabulka->tab[index];
    // nejaky ukazatel na prvni polozku


    // HLEDAME
    while(p!=NULL )  // chladej az do konca nebo do zarazky
    {
        if(p->typ==T_ZARAZKA)  // narazili jsme na zarazku ->konec
            break;


        if ((strcmp(jmeno,p->jmeno))==0)  // nalezl jsem shodny retezec
        {
            return p;
        }
         p=p->next; // posun dal
    }
    return NULL;  //
}

//_______________ PRIDEJ PROMENNOU___________________________//
// prida promennou s danym typem a hodnotou do tabulky
// pokud jiz existuje takova prommenna v dane urovni tak se hodnota prepise
// => pri deklaraci kontrolovat jestli již existuje s danym jmenem promenna
//
//
// danou urovni se rozumi v oblasti pred zarazkou
// v pripade spatne alokace vraci NULL

t_symbol * pridej_symbol(struct htable* tabulka,char* jmeno,int typ,union value hodnota)
{

    unsigned int index=hash_function(jmeno,tabulka->size);
    // index ktery nam vrati hashfunkce

    struct symbol  *temp=tabulka->tab[index];
    // nejaky ukazatel na prvni polozku


    //____PREPIS
    t_symbol* uk=NULL;
    if( (uk=najdi_symbol(tabulka,jmeno)) !=NULL) // uz existuje
        {
            uk->typ=typ; // muze dojit i ke zmenu typu

            if(uk->typ==STRING)  // v pripade stringu musime UDMA_free na starou hodnotu
                UDMA_free(uk->hodnota.str);

            uk->hodnota=hodnota;


        #ifdef DEBUG
            printf("zmenili jsme symbol se jmenem: %s typ: %d\n",uk->jmeno,uk->typ);
        #endif

            return uk;
        }


    //____NOVY symbol
    t_symbol *p=UDMA_malloc(sizeof(struct symbol));  // alokujem si novy symbol
    if (p==NULL)
      {
        err_flg=ERR_INT;
        return NULL;
      }

    // inicializujeme novou polozku
    p->jmeno=jmeno;
    p->hodnota=hodnota;
    p->next=temp;
    p->typ=typ;



        #ifdef DEBUG
            printf("pridali jsme symbol se jmenem: %s typ: %d\n",p->jmeno,p->typ);
        #endif

    // Aktualizujeme tabulku
    tabulka->tab[index]=p;
    return p;
}

//_________________ PRIDEJ ZARAZKY______________________________//
// prida zarazky pro oddeleni urovni
// v pripade problemu s pameti modifukuje err_flg

void pridej_zarazky(struct htable *tabulka)
{

    for(unsigned int i=0;i<tabulka->size;i++)      //// projdi a pridavej zarazky
        {
            t_symbol* zarazka=UDMA_malloc(sizeof(t_symbol));
            if(zarazka==NULL)
            {
                err_flg=ERR_INT; // chyba
                return;
            }

            zarazka->typ=T_ZARAZKA;
            zarazka->next=tabulka->tab[i];

            tabulka->tab[i]=zarazka;  // upraveni ukazatele na prvni prvek
            zarazka->jmeno=NULL;  // inicializace zarazky



        }
        #ifdef DEBUG
            printf("pridany zarazky----------------------------: \n");
        #endif

}

//________________ UVOLNI ZARAZKY ________________//
//
// uvolni nam nejvyssi uroven zarazek
// tj. vsechny symboly po zarazky
// !! nevolat pokud nejsou vytvoreny zarazky

void uvolni_zarazky(struct htable *tabulka)
{
    for(unsigned int i=0;i<tabulka->size;i++)
    {
       struct symbol *temp=tabulka->tab[i];  // dva ukazatele
       struct symbol *uk=tabulka->tab[i];

               while (uk->typ !=T_ZARAZKA)  // smazine vseho po zarazky
               {

                   temp=uk;  // nahrani ze zalozniho ukazatele

                    if(temp->typ==STRING)
                        UDMA_free(temp->hodnota.str);  // uvolneni hodnoty- jen pro string

                    UDMA_free(temp->jmeno);  // uvolneni stringu s nazvem

                    uk=uk->next;    // posun zalozniho ukazatele

                    UDMA_free(temp);   // vymazani polozky

                }
            tabulka->tab[i]=uk->next;
            UDMA_free(uk); // smazani zarazky


    }
        #ifdef DEBUG
            printf("vse po zarazky smazano\n");
        #endif
}



//______________ PRIDEJ LITERAL_____________________//
//
// specialni funkce pro pridani literalu
// staci ji jen predhodit tabulku hodnotu v unii a typ (dle tokenu)
// vrati nam index - poradove zaporne cislo literalu
int pridej_literal(struct htable *tabulka,t_value hodnota,int typ)
{
      int index=--(tabulka->count);   // dekrementujeme pocitadlo a priradime do

      char *jmeno=my_itoa(index); //vytvorime si jmeno dle integeru
      if(jmeno==NULL)
        {
            err_flg=ERR_INT;
            return 0;
        }

      pridej_symbol(tabulka,jmeno,typ,hodnota);  // dupneme ho do tabulky


      return index;  // vratime jeho index
}

//______________PRIDEJ PROMENNOU_________________//
//
// obdobna funkce jako literal
// pripadne vlozte prazdnou hodnotu

int pridej_promennou(struct htable *tabulka,char *jmeno,t_value hodnota,int typ)
{
      int index=++(tabulka->count);   // inkrementujeme pocitadlo a priradime do

      t_symbol* symbol=pridej_symbol(tabulka,jmeno,typ,hodnota);  // dupneme ho do tabulky

      if(symbol==NULL)
        return 0; // spatne je to
        
      symbol->index=index;

      return index;  // vratime jeho index
}


//___________________ TYPE__________________//
// vrati nam integer s hodnotou daneho typu dle
// #define v scanneru

char *type_c(int poradove_cislo,struct htable* TS_prom,struct htable* TS_liter)
{

    char *jmeno_int=my_itoa(poradove_cislo);  // prevedeme si to na string
    char *typ_str=UDMA_malloc(sizeof(char)*8); // magicky osmi-charovy retezec pro string s typem

    if(jmeno_int==NULL || typ_str==NULL) // neco se nam tu dojebalo s pameti
    {
        UDMA_free(jmeno_int);     // uklidime pripadny bordel a skoncime s chybou
        UDMA_free(typ_str);         //
        err_flg=ERR_INT;
        return NULL;
    }

    t_symbol*sym; // pomocny uk

    if(poradove_cislo>0)  // mame hledat v tabulce symbolu
    {
        sym=najdi_symbol(TS_prom,jmeno_int);
    }
    else if(poradove_cislo<0)
    {
        sym=najdi_symbol(TS_liter,jmeno_int);
    }
    else
        return NULL; // kravina

    if(!sym) // nenasli jsme
        return NULL;


    switch (sym->typ)  // nasli jsme a vratime prislusny string
    {
        case STRING:    strcpy(typ_str,"string");    break;
        case NUMBER:   strcpy(typ_str,"number"); break;
        case BOOL:       strcpy(typ_str,"boolean"); break;
        default:           strcpy(typ_str,"nil");      break;
    }

    UDMA_free(jmeno_int);
    return typ_str;
}

//___________ PRIDEJ LOCAL____________________//
//  urceno pro instrukci LOCAL x
//  prida nam do TS promenou s danym cislem x
//  do TS
//
void pridej_local(int cislo, struct htable* tabulka)
{
    char *jmeno=my_itoa(cislo); // prevedeme na cislo
    if(jmeno==NULL)
    {
        err_flg=ERR_INT;
        return;
    }

    // narveme to do TS s nedefinovanou hodnotou a typem NIL
    t_value undefined;
    pridej_promennou(tabulka,jmeno,undefined,NIL);

}
//_________________NAHREJ HODNOTU____________//
// prida hodnotu x do promenne s cislem cislo
// urceno pro instrukce modifikujici promenne
//
void nahrej_hodnotu(t_value x,int cislo,int typ_hodnoty,struct htable *tabulka)
{
    char *jmeno=my_itoa(cislo); // prevedeme na cislo
    if(jmeno==NULL)
    {
        err_flg=ERR_INT;
        return;
    }

    t_symbol * sym=najdi_symbol(tabulka,jmeno);  // najdeme si ukazatel

    if(sym==NULL)  // nemelo by nastat ze ho nenajdeme
    {
        puts("nenalezena prommena --> fatalni chyba");
    }

    sym->typ=typ_hodnoty;   // nahrani dat do promenne
    sym->hodnota=x;


    #ifdef DEBUG
        char *_s=type_c(cislo,tabulka,tabulka);
        printf("zmenili jsme promennou: %s s typem: %s\n",jmeno,_s);
        UDMA_free(_s);
    #endif

    UDMA_free(jmeno);  // tohle bylo jen pomocne jmeno -> tahni k čertu
}

// _____________PRIDEJ SYSTEMOVOU_____________//
// prida systemovou promennou do TS a vrati jeji index
int pridej_systemovou(struct htable * tabulka)
{
        int index=++(tabulka->count);   // dekrementujeme pocitadlo a priradime do

        t_value hodnota; // undefined
        int typ=NIL;

      char *jmeno=my_itoa(index); //vytvorime si jmeno dle integeru
      if(jmeno==NULL)
        {
            err_flg=ERR_INT;
            return 0;
        }

      pridej_symbol(tabulka,jmeno,typ,hodnota);  // dupneme ho do tabulky


      return index;  // vratime jeho index
}


//_______________ PRINT TABLE _______________________//
// vytiskne tabulku

void print_ts(struct htable* tabulka)
{

    puts("-------------ZOBRAZENI TS---------------------");

    for(unsigned int i=0;i<tabulka->size;i++)
    {
       struct symbol *uk=tabulka->tab[i]; // ukazatel

        printf("TS[%d] | ",i);

        while (uk !=NULL)  // projdeme se zretezenim
        {
            if(uk->jmeno ==NULL && uk->typ==T_ZARAZKA)  // tiskneme muhehe
                printf(" ||zarazka|| ");
            else
                printf(" [%s] ",uk->jmeno);

            uk=uk->next; // posun
        }
        puts("");

    }
    puts("----------------------------------------------");
}



/***** konec souboru****/
