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
*   Soubor:  interpreter.c
*   Popis:   Implementace funkci provadejici seznam instrukci z parseru.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include "interpreter.h"
#include <math.h>           // pow()
#include <stdbool.h>
/*============================================================================*/

// #define TISK


typedef struct DATA{
  instruction *instr; //ukazatel na instrukci kde se bude pokracovat
  int label;          //index labelu pokud si ukladam zasobnik labelu
} TData;

typedef struct LabelAndIP{
  TData data;
  struct LabelAndIP *next;  //dalsi prvek
} ISitem;

typedef struct stack{
  ISitem *top;
} IStack;

/* Fronta parametrov */

typedef struct QDATA{
    int qtyp;
    union value qhodnota;
} TQData;

typedef struct QITEM{
    TQData qdata;
    struct QITEM *qnext;
} Qitem;

typedef struct QUE_PARAM{
    Qitem *zac;
    Qitem *kon;
} Queparam;


/*======= Prototypy lok. funkci ==============================================*/
TData makeTData(instruction *instr,int label);//z parametru vyrobi strukturu TData
void IStack_init(IStack *Stack);  //inicializuje zasobnik
void IStack_UDMA_free(IStack *Stack);  //uvolni zasobnik
bool IStack_empty(IStack *Stack); //1 kdyz ano a 0 kdyz ne
int IStack_push(IStack *Stack, TData data); //prida TData na vrchol zasobniku 0 - chyba; 1 - v poradku
TData IStack_top(IStack *Stack);  //vraci data z vrcholu zasobniku
void IStack_pop(IStack *Stack);   //uvolni prvek z vrcholu zasobniku
instruction *getLabel(IStack *Stack,int index);   //najde navesti v seznamu
/*============================================================================*/
/*=======Protokoly funkcii pre frontu=========================================*/
TQData makeTQData(int typ,union value hodnota);   // vytvori strukturu TQData, obdoba funkcie makeTData
void Qinit(Queparam *Que);  // inicializacia fronty
int Qup(Queparam *Que,TQData data);    // vlozenie prvku na zaciatok fronty, 0 == chyba, 1 == dobre
TQData Qtop(Queparam *Que); // vrati hodnotu prveho prvku z fronty
void Qdown(Queparam *Que);  // zrusi prvy prvok vo fronte
void Qclean(Queparam *Que); // zrusi celu frontu
void Qprint(Queparam *Que); //vypise frontu
/*============================================================================*/


//z parametru vyrobi strukturu TData
TData makeTData(instruction *instr,int label){
  TData pom;
  pom.instr=instr;
  pom.label=label;
  return pom;
}

//inicializuje zasobnik
void IStack_init(IStack *Stack){
  Stack->top=NULL;
}

//uvolni zasobnik
void IStack_UDMA_free(IStack *Stack){
  ISitem *item;

  if(Stack==NULL)
    return;

  while(Stack->top!=NULL){
    Stack->top=Stack->top->next;
    item=Stack->top;
    UDMA_free(item);
  }
}

//1 kdyz ano a 0 kdyz ne
bool IStack_empty(IStack *Stack){
  if(Stack==NULL)
    return 1;
  else
    return Stack->top==NULL;
}

//prida prvek na vrchol zasobniku
int IStack_push(IStack *Stack, TData data){

  if(Stack==NULL)
    return 0;

  ISitem *pom;

  if((pom=(ISitem *)UDMA_malloc(sizeof(ISitem)))==NULL)
    return 0;

  pom->data.instr=data.instr;
  pom->data.label=data.label;
  pom->next=Stack->top;
  Stack->top=pom;

  return 1;
}

//vraci data z vrcholu zasobniku
TData IStack_top(IStack *Stack){

  if((Stack==NULL)||(Stack->top==NULL))
    return makeTData(NULL,0);
  else
    return makeTData(Stack->top->data.instr,Stack->top->data.label);
}

//uvolni prvek z vrcholu zasobniku
void IStack_pop(IStack *Stack){
  ISitem *pom;

  if((Stack==NULL)||(Stack->top==NULL))
    return;

  pom=Stack->top;
  Stack->top=Stack->top->next;
  UDMA_free(pom);

  return;
}

//hleda navesti v seznamu navesti s danym indexem - vraci kam mame skakat
instruction *getLabel(IStack *Stack,int index){

  if((Stack==NULL)||(Stack->top==NULL))
    return NULL;

  ISitem *item=Stack->top;

  while((item != NULL)&&(item->data.label != index))
    item=item->next;

  if(item != NULL)
    return item->data.instr;
  else
    return NULL;

}

/** funkcie pracujece a fronrou **/

 // vytvori strukturu TQData, obdoba funkcie makeTData
TQData makeTQData(int typ, union value hodnota)
{
    TQData pom;
    pom.qtyp = typ;
    pom.qhodnota = hodnota;
    return pom;
}

// inicializacia fronty
void Qinit(Queparam *Que)
{
    Que->zac = NULL;
    Que->kon = NULL;
}

// prida prvok na vrchol fronty, vrati 0 == chyba, 1 == spravne
int Qup(Queparam *Que, TQData data)
{
    if (Que == NULL)
        return 0;

    Qitem *pom;
    if ((pom=(Qitem *)UDMA_malloc(sizeof(Qitem))) == NULL)
        return 0;
    else
        {
            pom->qdata = data;
            pom->qnext = NULL;
            if (Que->zac == NULL)
                Que->zac = pom;
            else
                Que->kon->qnext = pom;
            Que->kon = pom;
            return 1;
        }
}

// vrati hodnotu prveho prvku
TQData Qtop(Queparam *Que)
{
    union value hodnota;

    if ((Que->zac == NULL) || (Que == NULL))
        return makeTQData(NIL,hodnota);
    else
        return makeTQData(Que->zac->qdata.qtyp,Que->zac->qdata.qhodnota);
}

//odstrani hodnotu zo zaciatku fronty
void Qdown(Queparam *Que)
{
    Qitem *pom;
    if (Que->zac != NULL)
        {
            pom = Que->zac;
            if (Que->zac == Que->kon)
                Que->kon = NULL;
            Que->zac = Que->zac->qnext;
            UDMA_free(pom);
        }
}

// vycistenie celej fronty
void Qclean(Queparam *Que)
{
    Qitem *pom;
    while (Que->zac != NULL)
        {
            pom = Que->zac;
            if (Que->zac == Que->kon)
                Que->kon = NULL;
            Que->zac = Que->zac->qnext;
            UDMA_free(pom);
        }
}

// funkcia na vypisanie fronty
void Qprint(Queparam *Que)
{
    Qitem *pom;
    pom = Que->zac;
    while (pom != NULL)
        {
            printf("typ: %d\n", pom->qdata.qtyp);
            //printf("hodnota: X\n", pom->data.hodnota);  // neviem ako mam dat vypisat hodnotu, kedze je to union
            printf("--------------------------------------\n");
            pom = pom->qnext;
        }
}


/* koniec funkcii pracujucich s frontou
********************************************************************************************/
/* jadro interpretu */

void interpret(FTable *tab, t_table *tLit)
{
    // setbuf(stdout,NULL);
    
    
    t_table *tProm = htable_init(); //tabulka symbolu pro promenne

    instruction *instrukcia =  tab->function[tab->count]->instr;

    Queparam QParametre;
    IStack Stack_lab;
    IStack Stack_IP;

    Qinit(&QParametre);
    IStack_init(&Stack_lab);
    IStack_init(&Stack_IP);
    // pomocne premenne
    char *m;    // pouzivam pre mena literalov pri funkcii my_itoa
    char *n;
    double hodnota;     // pomocna prem pre munber
    int ok;     // kontrolna premenna
    t_value hod;    // hod (akratka hodnoty)
    t_symbol* symbol;
    t_symbol* symbol2;
    int typ1;    // typ literalu ci premennej
    int typ2;
    char c = EOF; // pre getchar()
    bool porovnanie;
    int pocet_znakov;   // read_number
    int i = -2;  // pre for cyklus
    TData ret_Sdata; //pro praci se zasobnikem u returnu
    TQData quedown_data; //pro presun z fronty parametru zpatky do promenne
    TQData quedown_data2; //pro vestavene funkce
    TQData quedown_data3; //pro vestavene funkce
    int mybreak=0;      //pro skoky
    instruction *pomins; //pro skoky
    int labindex; //pro skoky;
    char *a="BLBOST"; //retezec pro tisk
    int rdi;
    int rdpocitadlo;
    int rdc;
    char *rdretazec;
    int rdbreak = -5;
    //----------------------
    while(err_flg == OK)
        {
            switch(instrukcia->type)
                {
//>>> READ_N
                    case READ_N:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        
                        if ((ok = scanf("%lf", &hodnota)) == 1)
                            {
                                hod.num = hodnota;
                                nahrej_hodnotu(hod,instrukcia->op1,NUMBER,tProm);
                            }
                        else
                            {
                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> READ_L
                    case READ_L:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        rdi = 0;
                        rdpocitadlo = 1;
                        rdretazec = NULL;
                        rdretazec = UDMA_malloc(10 * sizeof(char) + 1);
                        if (rdretazec == NULL)
                            {
                                err_flg = ERR_INT;
                                break;
                            }
                        else
                            {
                                while ((rdc = getchar()) != '\n')        ////\n
                                    {
                                        if (rdc == EOF)
                                            {
                                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                                                rdbreak = 1;
                                                break;
                                            }
                                        else
                                            {
                                                rdretazec[rdi] = rdc;
                                                rdretazec[rdi+1] = '\0';
                                                rdi++;
                                                if ((rdi % 10) == 9)
                                                    {
                                                        rdpocitadlo++;
                                                        //printf("pocitadlo: %d\n",pocitadlo);
                                                        rdretazec = (char *) UDMA_realloc(rdretazec, sizeof(char) * rdpocitadlo * 10 + 1);

                                                        if (rdretazec == NULL)
                                                            {
                                                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                                                                rdbreak = 1;
                                                                break;
                                                            }
                                                    }
                                            }
                                    }
                                if (rdbreak == 1)
                                    break;
                                else
                                    {
                                        hod.str = rdretazec;
                                        nahrej_hodnotu(hod,instrukcia->op1,STRING,tProm);
                                    }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> READ_A
                    case READ_A:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        rdi = 0;
                        rdpocitadlo = 1;
                        rdretazec = NULL;
                        rdretazec = UDMA_malloc(10 * sizeof(char) + 1);
                        if (rdretazec == NULL)
                            {
                                err_flg = ERR_INT;
                                break;
                            }
                        else
                            {
                                while ((rdc = getchar()) != EOF)
                                    {
                                        if (rdc == EOF)
                                            {
                                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                                                rdbreak = 1;
                                                break;
                                            }
                                        else
                                            {
                                                rdretazec[rdi] = rdc;
                                                rdretazec[rdi+1] = '\0';
                                                rdi++;
                                                if ((rdi % 10) == 9)
                                                    {
                                                        rdpocitadlo++;
                                                        //printf("pocitadlo: %d\n",pocitadlo);
                                                        rdretazec = (char *) UDMA_realloc(rdretazec, sizeof(char) * rdpocitadlo * 10 + 1);

                                                        if (rdretazec == NULL)
                                                            {
                                                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                                                                rdbreak = 1;
                                                                break;
                                                            }
                                                    }
                                            }
                                    }
                                if (rdbreak == 1)
                                    break;
                                else
                                    {
                                        hod.str = rdretazec;
                                        nahrej_hodnotu(hod,instrukcia->op1,STRING,tProm);
                                    }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> READ_NUMB
                    case READ_NUMB:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        pocet_znakov = instrukcia->op2;
                        char *ret;
                        if ((ret = (char *)UDMA_malloc((pocet_znakov + 1) * sizeof(char))) == NULL)
                            {
                                err_flg = ERR_INT;
                                break;
                            }
                        else
                            {
                                for(i = 0; i < pocet_znakov; i++)
                                    {
                                        c = getchar();
                                        if (c == EOF)
                                            {
                                                nahrej_hodnotu(hod,instrukcia->op1,NIL,tProm);
                                                break;
                                            }
                                        ret[i] = c;
                                    }
                                if (c == EOF)
                                    break;
                                else
                                    {
                                        ret[pocet_znakov] = '\0';
                                        hod.str = ret;
                                        nahrej_hodnotu(hod,instrukcia->op1,STRING,tProm);
                                    }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> WRITE
                    case WRITE:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m =  my_itoa(instrukcia->op3);
                        if(instrukcia->op3 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op3 > 0)
                                symbol = najdi_symbol(tProm,m);
                             else
                                {
                                  err_flg = ERR_ITP;
                                  #ifdef TISK
                                    fprintf(stderr,"write chyba 1\n");
                                  #endif 
                                  break;
                                }

                        typ1 = symbol->typ;

                        if (typ1 == NUMBER)
                            {
                                double a = symbol->hodnota.num;
                                printf("%g", a);
                            }
                        else if (typ1 == STRING)
                            {
                                a=symbol->hodnota.str;
                                printf("%s", a);
                            }
                        else
                            {
                              if(instrukcia->op3 > 0){
                                err_flg = ERR_INT;
                                #ifdef TISK
                                  fprintf(stderr,"write chyba 2\n");
                                #endif 
                              }
                              if(instrukcia->op3 < 0){
                                err_flg = ERR_SEM;
                                #ifdef TISK
                                  fprintf(stderr,"write chyba 3\n");
                                #endif 
                              }
                              break;
                            }

                        instrukcia = instrukcia->next;
                     break;
//>>> LABEL
                    case LABEL:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //ulozim ukazatel na dalsi instrukci na zasobnik labelu
                        if(!IStack_push(&Stack_lab,makeTData(instrukcia->next,instrukcia->op1))){
                            err_flg = ERR_INT;
                            #ifdef TISK
                              fprintf(stderr,"label chyba 0\n");
                            #endif 
                            break;
                        }
                        //popojdu na dalsi instrukci
                        instrukcia = instrukcia->next;
                     break;
//>>> IFNOTGOTO
                    case IFNOTGOTO:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //nactu si podle ceho porovnavam
                        if(instrukcia->op1 != 0){
                          if(instrukcia->op1 < 0)
                            //porovnavam literal
                            symbol=najdi_symbol(tLit,my_itoa(instrukcia->op1));
                          else
                          //porovnavam promennou
                            symbol=najdi_symbol(tProm,my_itoa(instrukcia->op1));
                        }
                        else{
                            err_flg = ERR_INT;
                            #ifdef TISK
                              fprintf(stderr,"ifnotgoto chyba 0\n");
                            #endif 
                            break;
                        }

                        if((symbol->typ==NIL)||((symbol->typ==BOOL)&&(!symbol->hodnota.flag))){
                          //pokud je to NIL nebo FALSE tak skaceme
                          #ifdef TISK
                          if(symbol->typ==NIL)
                            printf("skok na nil\n");
                          if(!symbol->hodnota.flag)
                            printf("skok na false\n");
                          #endif
                          //uplne stejne jako goto
                          labindex = instrukcia->op3;
                          while ((pomins = getLabel(&Stack_lab,labindex)) == NULL)
                              {
                                  if (instrukcia == NULL)
                                      {
                                        err_flg = ERR_INT;
                                        #ifdef TISK
                                          fprintf(stderr,"ifnotgoto chyba 1\n");
                                        #endif 
                                        break;
                                      }
                                  if (instrukcia->type == LABEL)
                                      {
                                        if(!IStack_push(&Stack_lab,makeTData(instrukcia->next,instrukcia->op1))){
                                          err_flg = ERR_INT;
                                        #ifdef TISK
                                          fprintf(stderr,"ifnotgoto chyba 2\n");
                                        #endif 
                                        break;
                                        }
                                      }
                                  instrukcia = instrukcia->next;
                              }
                          //poskocime na instrukci kterou jsme nasli
                          instrukcia=pomins;
                        }
                        else{
                          //je to neco jineho takze TRUE takze neskaceme
                          instrukcia = instrukcia->next;
                        }

                     break;
//>>> GOTO
                    case GOTO:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //index navesti ktere hledam
                        labindex = instrukcia->op3;

                        //hledam navesti na zasobniku
                        while ((pomins = getLabel(&Stack_lab,labindex)) == NULL)
                            {
                                if (instrukcia == NULL)
                                    {
                                        err_flg = ERR_INT;
                                        #ifdef TISK
                                          fprintf(stderr,"goto chyba 2\n");
                                        #endif 
                                        mybreak=1;
                                        break;
                                    }
                                if (instrukcia->type == LABEL)
                                    {
                                      if(!IStack_push(&Stack_lab,makeTData(instrukcia->next,instrukcia->op1))){
                                        err_flg = ERR_INT;
                                        #ifdef TISK
                                          fprintf(stderr,"goto chyba 3\n");
                                        #endif 
                                        mybreak=1;
                                        break;
                                      }
                                    }
                                instrukcia = instrukcia->next;
                            }

                        //skoncili jsme na NULL
                        if(mybreak==1)
                          break;

                        //neco jsem nasel takze tam pujdu
                        instrukcia = pomins;

                     break;
//>>> NOT
                    case NOT:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                                symbol = najdi_symbol(tProm,m);
                             else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"not chyba 1\n");
                                    #endif 
                                    break;
                                }
                         if (instrukcia->op3 <= 0){
                              err_flg = ERR_ITP;
                              #ifdef TISK
                                fprintf(stderr,"not chyba 2\n");
                              #endif 
                              break;
                         }
                         
                        typ1 = symbol->typ;
                        if (typ1 == BOOL)
                            {
                                porovnanie = !(symbol->hodnota.flag);
                                hod.flag = porovnanie;
                                nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                            }

                        instrukcia = instrukcia->next;
                     break;
// >>>COPY
                    case COPY:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                                symbol = najdi_symbol(tProm,m);
                             else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"copy chyba 1\n");
                                    #endif 
                                    break;
                                }
                                
                         if (instrukcia->op3 <= 0){
                              err_flg = ERR_ITP;
                              #ifdef TISK
                                fprintf(stderr,"copy chyba 2\n");
                              #endif 
                              break;
                         }
                         
                        typ1 = symbol->typ;
                        
                        if (typ1 == NUMBER)
                            {
                                hod.num = symbol->hodnota.num;
                                nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                            }
                        else if (typ1 == STRING)
                                {
                                    hod.str = symbol->hodnota.str;
                                    nahrej_hodnotu(hod,instrukcia->op3,STRING,tProm);
                                }
                             else if (typ1 == BOOL)
                                        {
                                            hod.flag = symbol->hodnota.flag;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                  else if (typ1 == NIL)
                                              {
                                              nahrej_hodnotu(hod,instrukcia->op3,NIL,tProm);
                                              }
                                        else{
                                          err_flg = ERR_ITP;
                                          #ifdef TISK
                                            fprintf(stderr,"copy chyba 3\n");
                                          #endif 
                                          break;
                                        }
                        instrukcia = instrukcia->next;
                     break;
//>>> PLUS
                    case PLUS:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);       //  spravim si z indexov mena
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)            //  hladam premennu ci literal
                            symbol = najdi_symbol(tLit,m);  //  nasiel som literal
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);  //  nasiel som premennu
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"plus chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"plus chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;

                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                hodnota = symbol->hodnota.num + symbol2->hodnota.num;
                                hod.num = hodnota;
                                nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                            }
                        else
                            {
                              if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> MINUS
                    case MINUS:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"minus chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"minus chyba 2\n");
                                    #endif 
                                    break;
                                }

                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;

                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                hodnota = symbol->hodnota.num - symbol2->hodnota.num;
                                hod.num = hodnota;
                                nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                            }
                        else
                            {
                              if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> POW
                    case POW:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"pow chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"pow chyba 1\n");
                                    #endif 
                                    break;
                                }

                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;

                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                double x;   // pomocne premenne
                                double y;
                                int iy;     // kvoli pretypovaniu, ak x < 0 y musi byt int
                                x = symbol->hodnota.num;
                                y = symbol2->hodnota.num;
                                if (x < 0)
                                    {
                                        iy = (int)y;
                                        hodnota = pow(x,iy);
                                    }
                                else
                                    {
                                        hodnota = pow(x,y);
                                    }
                                hod.num = hodnota;
                                nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                            }
                        else
                            {
                             if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> MUL
                    case MUL:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mul chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mul chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;

                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                hodnota = symbol->hodnota.num * symbol2->hodnota.num;
                                hod.num = hodnota;
                                nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                            }
                        else
                            {
                              if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> DIV
                    case DIV:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 1\n");
                                    #endif 
                                    break;
                                }

                        m = my_itoa(instrukcia->op1);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 1.5\n");
                                    #endif 
                                    break;
                                }
                                
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        
                        if ((typ1 == NUMBER)&&(typ2 == NUMBER))
                            {
                                if (symbol2->hodnota.num == 0)
                                    {
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"div chyba 2\n");
                                      #endif 
                                      break;
                                    }
                                else
                                    {
                                      hodnota = symbol->hodnota.num / symbol2->hodnota.num;
                                      hod.num = hodnota;
                                      nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                                    }
                            }
                        else
                            {
                              if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"div chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> MOD
                    case MOD:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 1\n");
                                    #endif 
                                    break;
                                }
                                
                         m = my_itoa(instrukcia->op1);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 1.5\n");
                                    #endif 
                                    break;
                                }

                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER)&&(typ2 == NUMBER))
                            {
                                if (symbol2->hodnota.num == 0)
                                    {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 2\n");
                                    #endif 
                                    break;
                                    }
                                else
                                    {    
                                      double pomd1;// pomocne premenne, aby som mohol pouzit modulo, potrebujem pretypovat double na int
                                      double pomd2;
                                      int pomi1;
                                      int pomi2;
                                      pomd1 = symbol->hodnota.num;
                                      pomd2 = symbol2->hodnota.num;
                                      pomi1 = (int)pomd1;
                                      pomi2 = (int)pomd2;
                                      hodnota = pomi1 % pomi2;
                                      hod.num = hodnota;
                                      nahrej_hodnotu(hod,instrukcia->op3,NUMBER,tProm);
                                    }
                            }
                        else
                            {
                              if((typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 3\n");
                                    #endif 
                                    break;
                              }
                              if((typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 4\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                    err_flg = ERR_SEM;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 5\n");
                                    #endif 
                                    break;
                              }
                              if((typ2 != NUMBER)&&(instrukcia->op2 > 0)){
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"mod chyba 6\n");
                                    #endif 
                                    break;
                              }
                            }
                        instrukcia = instrukcia->next;
                     break;
//>>> LT
                    case LT:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"lt chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"lt chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                porovnanie = symbol->hodnota.num < symbol2->hodnota.num;
                                hod.flag = porovnanie;
                                nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                            }
                        else if ((typ1 == STRING) && (typ2 == STRING))
                                {
                                    if (strcmp(symbol->hodnota.str, symbol2->hodnota.str) < 0)
                                        {
                                            porovnanie = true;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                    else
                                        {
                                            porovnanie = false;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                }
                             else
                                {
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 3\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 4\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ2 != STRING)&&(typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 5\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 6\n");
                                      #endif 
                                      break;
                                    }
                                    
                                    //jsou tospravne operandy ale spatna kombinace
                                    if((instrukcia->op1 > 0)||(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 7\n");
                                      #endif 
                                      break;
                                    }
                                    else{
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"lt chyba 8\n");
                                      #endif 
                                      break;
                                    }
                                }
                        instrukcia = instrukcia->next;
                     break;
//>>> LE
                    case LE:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"le chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"le chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                porovnanie = symbol->hodnota.num <= symbol2->hodnota.num;
                                hod.flag = porovnanie;
                                nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                            }
                        else if ((typ1 == STRING) && (typ2 == STRING))
                                {
                                    if (strcmp(symbol->hodnota.str, symbol2->hodnota.str) <= 0)
                                        {
                                            porovnanie = true;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                    else
                                        {
                                            porovnanie = false;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                }
                             else
                                {
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 3\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 4\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ2 != STRING)&&(typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 5\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 6\n");
                                      #endif 
                                      break;
                                    }
                                    
                                    //jsou tospravne operandy ale spatna kombinace
                                    if((instrukcia->op1 > 0)||(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 7\n");
                                      #endif 
                                      break;
                                    }
                                    else{
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"le chyba 8\n");
                                      #endif 
                                      break;
                                    }
                                }
                        instrukcia = instrukcia->next;
                     break;
//>>> GT
                    case GT:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"gt chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"gt chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                porovnanie = symbol->hodnota.num > symbol2->hodnota.num;
                                hod.flag = porovnanie;
                                nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                            }
                        else if ((typ1 == STRING) && (typ2 == STRING))
                                {
                                    if (strcmp(symbol->hodnota.str, symbol2->hodnota.str) > 0)
                                        {
                                            porovnanie = true;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                    else
                                        {
                                            porovnanie = false;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                }
                             else
                                {
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 3\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 4\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ2 != STRING)&&(typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 5\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 6\n");
                                      #endif 
                                      break;
                                    }
                                    
                                    //jsou tospravne operandy ale spatna kombinace
                                    if((instrukcia->op1 > 0)||(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 7\n");
                                      #endif 
                                      break;
                                    }
                                    else{
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"gt chyba 8\n");
                                      #endif 
                                      break;
                                    }
                                }
                        instrukcia = instrukcia->next;
                     break;
//>>> GE
                    case GE:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"ge chyba 1\n");
                                    #endif 
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                    err_flg = ERR_ITP;
                                    #ifdef TISK
                                      fprintf(stderr,"ge chyba 2\n");
                                    #endif 
                                    break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                            {
                                porovnanie = symbol->hodnota.num >= symbol2->hodnota.num;
                                hod.flag = porovnanie;
                                nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                            }
                        else if ((typ1 == STRING) && (typ2 == STRING))
                                {
                                    if (strcmp(symbol->hodnota.str, symbol2->hodnota.str) >= 0)
                                        {
                                            porovnanie = true;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                    else
                                        {
                                            porovnanie = false;
                                            hod.flag = porovnanie;
                                            nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                                        }
                                }
                             else
                                {
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 3\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op1 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 4\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ2 != STRING)&&(typ2 != NUMBER)&&(instrukcia->op2 < 0)){
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 5\n");
                                      #endif 
                                      break;
                                    }
                                    if((typ1 != STRING)&&(typ1 != NUMBER)&&(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 6\n");
                                      #endif 
                                      break;
                                    }
                                    
                                    //jsou tospravne operandy ale spatna kombinace
                                    if((instrukcia->op1 > 0)||(instrukcia->op2 > 0)){
                                      err_flg = ERR_ITP;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 7\n");
                                      #endif 
                                      break;
                                    }
                                    else{
                                      err_flg = ERR_SEM;
                                      #ifdef TISK
                                        fprintf(stderr,"ge chyba 8\n");
                                      #endif 
                                      break;
                                    }
                                }
                                
                        instrukcia = instrukcia->next;
                     break;
//>>> EQ
                    case EQ:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0)
                            symbol = najdi_symbol(tLit,m);
                        else if (instrukcia->op1 > 0)
                            symbol = najdi_symbol(tProm,m);
                            else
                                {
                                  err_flg=ERR_ITP;
                                  #ifdef TEST
                                    fprintf(stderr,"eq chyba 1\n");
                                  #endif
                                    break;
                                }
                        if (instrukcia->op2 < 0)
                            symbol2 = najdi_symbol(tLit,n);
                        else if (instrukcia->op2 > 0)
                            symbol2 = najdi_symbol(tProm,n);
                            else
                                {
                                  err_flg=ERR_ITP;
                                  #ifdef TEST
                                    fprintf(stderr,"eq chyba 2\n");
                                  #endif
                                  break;
                                }
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;
                        if ((typ1 == NUMBER) && (typ2 == NUMBER))
                                hod.flag = symbol->hodnota.num == symbol2->hodnota.num;
                        else if ((typ1 == STRING) && (typ2 == STRING))
                                    hod.flag=!strcmp(symbol->hodnota.str, symbol2->hodnota.str);
                             else if ((typ1 == BOOL) && (typ2 == BOOL))
                                         hod.flag=symbol->hodnota.flag == symbol2->hodnota.flag;
                                  else if ((typ1 == NIL) && (typ2 == NIL))
                                            hod.flag=true;
                                      else{
                                        hod.flag=false;
                                      }
                        
                        //ulozim vysledek
                        nahrej_hodnotu(hod,instrukcia->op3,BOOL,tProm);
                        
                        //dalsi instrukce        
                        instrukcia = instrukcia->next;
                     break;
//>>> SUBSTR_FIND
                    case SUBSTR_FIND:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //presunu data z fronty parametru do promenne
                        quedown_data=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //presunu data z fronty parametru do promenne
                        quedown_data2=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //zrusim celou frontu
                        Qclean(&QParametre);

                        if((quedown_data.qtyp!=STRING)||(quedown_data2.qtyp!=STRING)){
                          //spatne parametry vracim nil
                          quedown_data.qtyp=NIL;
                        }
                        else{
                          //oba parametry v poradku

                          quedown_data.qhodnota.num=(double)find_c(quedown_data.qhodnota.str,quedown_data2.qhodnota.str);
                          quedown_data.qtyp=NUMBER;

                          //jestli jsme ho nenasli tak vracime false
                          if(quedown_data.qhodnota.num == -1){
                            quedown_data.qhodnota.flag=0;
                            quedown_data.qtyp=BOOL;
                          }
                        }

                        //dam zpatky na frontu
                        if(!Qup(&QParametre,quedown_data)){
                           err_flg=ERR_INT;
                           break;
                        }

                        //dalsi instrukce
                        instrukcia = instrukcia->next;
                    break;
//>>> CONC
                    case CONC:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        m = my_itoa(instrukcia->op1);
                        n = my_itoa(instrukcia->op2);
                        if (instrukcia->op1 < 0){
                            symbol = najdi_symbol(tLit,m);
                            if(symbol->typ != STRING){
                              err_flg=ERR_SEM;
                                  #ifdef TEST
                                    fprintf(stderr,"conc chyba 0\n");
                                  #endif
                              break;
                            }
                          }
                        else if (instrukcia->op1 > 0){
                              symbol = najdi_symbol(tProm,m);
                              if(symbol->typ != STRING){
                                err_flg=ERR_ITP;
                                    #ifdef TEST
                                      fprintf(stderr,"conc chyba 0.1\n");
                                    #endif
                                break;
                              }
                            }
                            else
                                {
                                  err_flg=ERR_ITP;
                                  #ifdef TEST
                                    fprintf(stderr,"conc chyba 1\n");
                                  #endif
                                    break;
                                }
                        if (instrukcia->op2 < 0){
                            symbol2 = najdi_symbol(tLit,n);
                            if(symbol->typ != STRING){
                              err_flg=ERR_SEM;
                                  #ifdef TEST
                                    fprintf(stderr,"conc chyba 0\n");
                                  #endif
                              break;
                            }
                          }
                        else if (instrukcia->op2 > 0) {
                            symbol2 = najdi_symbol(tProm,n);
                            if(symbol->typ != STRING){
                              err_flg=ERR_ITP;
                                  #ifdef TEST
                                    fprintf(stderr,"conc chyba 0\n");
                                  #endif
                              break;
                            }
                          }
                            else
                                {
                                    err_flg=ERR_SEM;
                                    #ifdef TEST
                                      fprintf(stderr,"conc chyba 0\n");
                                    #endif
                                    break;
                                }
                                
                        typ1 = symbol->typ;
                        typ2 = symbol2->typ;

                        int prvy;
                        int druhy;
                        int treti;
                        prvy = strlen(symbol->hodnota.str);
                        druhy = strlen(symbol2->hodnota.str);
                        treti = prvy + druhy +1;
                        char *novy = UDMA_malloc((treti)*sizeof(char));
                        if (novy == NULL)
                            {
                                err_flg = ERR_INT;
                                break;
                            }
                        else
                            {
                                strcpy(novy,symbol->hodnota.str);
                                strcat(novy, symbol2->hodnota.str);
                                hod.str = novy;
                                nahrej_hodnotu(hod,instrukcia->op3,STRING,tProm);
                            }

                            
                        instrukcia = instrukcia->next;
                     break;
//>>> SORT
                    case SORT:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //presunu data z fronty parametru do promenne
                        quedown_data=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //zrusim celou frontu
                        Qclean(&QParametre);

                        //pokud je to string tak seradim
                        if(quedown_data.qtyp == STRING)
                          sort_c(quedown_data.qhodnota.str);
                        else
                          //jinak je to NIL
                          quedown_data.qtyp=NIL;

                        //dam zpatky na frontu
                        if(!Qup(&QParametre,quedown_data)){
                           err_flg=ERR_INT;
                           break;
                        }

                        //dalsi instrukce
                        instrukcia = instrukcia->next;
                     break;
//>>> TYPE
                    case TYPE:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //presunu data z fronty parametru do promenne
                        quedown_data=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //zrusim celou frontu
                        Qclean(&QParametre);

                        switch (quedown_data.qtyp){
                          case STRING :
                              if((a=(char *)UDMA_malloc(sizeof(char)*7))==NULL){
                               err_flg=ERR_INT;
                               break;
                              }
                              strcpy(a,"string");
                            break;
                          case BOOL   :
                              if((a=(char *)UDMA_malloc(sizeof(char)*5))==NULL){
                               err_flg=ERR_INT;
                               break;
                              }
                              strcpy(a,"bool");
                            break;
                          case NUMBER :
                              if((a=(char *)UDMA_malloc(sizeof(char)*7))==NULL){
                               err_flg=ERR_INT;
                               break;
                              }
                              strcpy(a,"number");
                            break;
                          default     :
                              if((a=(char *)UDMA_malloc(sizeof(char)*4))==NULL){
                               err_flg=ERR_INT;
                               break;
                              }
                              strcpy(a,"nil");
                            break;
                        }

                        //pokud chyba pri alokaci tak skoncim
                        if(err_flg!=0)
                          break;

                        quedown_data.qtyp=STRING;
                        quedown_data.qhodnota.str=a;

                        if(!Qup(&QParametre,quedown_data)){
                           err_flg=ERR_INT;
                           break;
                        }

                        instrukcia = instrukcia->next;
                     break;
//>>> SUBSTR_CUT
                    case SUBSTR_CUT:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //presunu data z fronty parametru do promenne
                        quedown_data=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //presunu data z fronty parametru do promenne
                        quedown_data2=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //presunu data z fronty parametru do promenne
                        quedown_data3=Qtop(&QParametre);
                        Qdown(&QParametre);

                        //zrusim celou frontu
                        Qclean(&QParametre);

                        if((quedown_data.qtyp==STRING)&&(quedown_data2.qtyp==NUMBER)&&(quedown_data3.qtyp==NUMBER)){
                          //vsechny parametry v poradku
                          quedown_data.qhodnota.str=substr_c(quedown_data.qhodnota.str,quedown_data2.qhodnota.num,quedown_data3.qhodnota.num);
                        }
                        else{
                          //nejaky parametr je spatny vracim nil
                          quedown_data.qtyp=NIL;
                        }

                        //vysledek vratim zpatky na frontu
                        if(!Qup(&QParametre,quedown_data)){
                           err_flg=ERR_INT;
                           break;
                        }

                        //dalsi instrukce
                        instrukcia = instrukcia->next;
                     break;
//>>> CALL_FUN
                    case CALL_FUN:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //kontrola spavnosti atributu
                        if((instrukcia->op1 < 0)||(instrukcia->op1 > tab->count)){
                          err_flg=ERR_ITP;
                          break;
                        }
                        //ulozeni ukazatele na dalsi instrukci na zasobnik
                        if(!IStack_push(&Stack_IP,makeTData(instrukcia->next,0))){
                          err_flg=ERR_INT;
                          break;
                        }
                        //pridani zarazek do TS pro promenne
                        pridej_zarazky(tProm);
                        //zmena dalsi instrukce
                        instrukcia = tab->function[instrukcia->op1]->instr;
                     break;
//>>> RETURN
                    case RETURN:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //jestli je zasoobnik instrukci prazdny tak se vracime z mainu
                        //to znamena uspesny konec
                        if(IStack_empty(&Stack_IP)){
                            Qclean(&QParametre);
                            IStack_UDMA_free(&Stack_lab);
                            IStack_UDMA_free(&Stack_IP);
                            // htable_UDMA_free(tProm);
                            return;
                        }
                        //pokud ma funkce navratovy kod tak ho dam na zasobnik parametru
                        if(instrukcia->op3 != 0){
                          if(instrukcia->op3 < 0){
                            //je to literal
                            symbol=najdi_symbol(tLit,my_itoa(instrukcia->op3));
                            if(!Qup(&QParametre,makeTQData(symbol->typ,symbol->hodnota))){
                              err_flg=ERR_INT;
                              break;
                            }
                          }
                          else{
                            //je to promenna
                            symbol=najdi_symbol(tProm,my_itoa(instrukcia->op3));
                            if(!Qup(&QParametre,makeTQData(symbol->typ,symbol->hodnota))){
                              err_flg=ERR_INT;
                              break;
                            }
                          }
                        }
                        //uvolneni zarazek
                        uvolni_zarazky(tProm);
                        //zmena dalsi instrukce podle zasobniku
                        ret_Sdata=IStack_top(&Stack_IP);
                        IStack_pop(&Stack_IP);
                        instrukcia=ret_Sdata.instr;
                     break;
//>>> QUE_UP
                    case QUE_UP:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //nakopiruju promennou na frontu parametru

                        if(instrukcia->op1 != 0){
                          if(instrukcia->op1 < 0){
                            //je to literal
                            symbol=najdi_symbol(tLit,my_itoa(instrukcia->op1));
                            if(!Qup(&QParametre,makeTQData(symbol->typ,symbol->hodnota))){
                              err_flg=ERR_INT;
                              break;
                            }
                          }
                          else{
                            //je to promenna
                            symbol=najdi_symbol(tProm,my_itoa(instrukcia->op1));
                            if(!Qup(&QParametre,makeTQData(symbol->typ,symbol->hodnota))){
                              err_flg=ERR_INT;
                              break;
                            }
                          }
                        }
                        //dalsi instrukce
                        instrukcia = instrukcia->next;
                     break;
//>>> QUE_DOWN
                    case QUE_DOWN:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //presunu data z fronty parametru do promenne
                        quedown_data=Qtop(&QParametre);
                        Qdown(&QParametre);

                        if(instrukcia->op3 <= 0){
                          err_flg=ERR_INT;
                          break;
                        }
                        else{
                          //vse je v poradku ulozim ho do promenne
                          nahrej_hodnotu(quedown_data.qhodnota,instrukcia->op3,quedown_data.qtyp,tProm);
                        }

                        //dalsi instrukce
                        instrukcia = instrukcia->next;
                     break;
//>>> QUE_CLEAN
                    case QUE_CLEAN:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif

                        //zrusim celou frontu
                        Qclean(&QParametre);

                        instrukcia = instrukcia->next;
                     break;
//>>> LOCAL
                    case LOCAL:
                        #ifdef TISK
                          printinstr(instrukcia);
                        #endif
                        pridej_local(instrukcia->op3,tProm);    // asi je to vsetko, ak som spravne pochopil

                        instrukcia = instrukcia->next;
                     break;

                    default:
                        fprintf(stderr,"niekde nastala nejaka zrada, lebo toto je 'default\n'");
                        err_flg=ERR_INT;
                     break;
                }
        }
    //print_ts(tProm);        ////////////////////////////////////////// test
    Qclean(&QParametre);
    IStack_UDMA_free(&Stack_lab);
    IStack_UDMA_free(&Stack_IP);
    htable_UDMA_free(tProm);
    return;
}
/***** konec souboru****/
