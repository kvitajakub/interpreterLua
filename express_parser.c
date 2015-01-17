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
*   Soubor:  express_parser.c
*   Popis:   Implementace funkci pro syntaktickou analyzu vyrazu.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include "express_parser.h"
/*============================================================================*/

//tokeny - zkopirovano z scanner.h
// #define END_OF_FILE  0               //  Konec souboru
// #define ID           1               //  Identifikator
// #define NUMBER       2               //  Ciselny literal
// #define BOOL         3               //  Boolean
// #define STRING       4               //  Retezcovy literal
// // Aritmeticke operatory
// #define PLUS         10              //  +
// #define MINUS        11              //  -
// #define MUL          12              //  *
// #define DIV          13              //  /
// #define POW          14              //  ^
// #define MOD          15              //  mod
// // Prirazeni
// #define ASSIGN       20              //  =
// // Relacni operatory
// #define EQ           30              //  ==
// #define NE           31              //  ~=
// #define LT           32              //  <
// #define GT           33              //  >
// #define LE           34              //  <=
// #define GE           35              //  >=
// // Zavorky
// #define L_BRACKET    40              //  (
// #define R_BRACKET    41              //  )
// // Strednik
// #define SEMICOLON    50              //  ;
// // Konkatenace
// #define CONC         60              //  ..

// #define NIL          76              //  nil


// #define TESTOVANI
// #define MIMOPARSER
// #define TISKINSTRUKCI


#define PT_LEVY    -2  //<
#define PT_ROVNO   -3  //=
#define PT_PRAVY   -4  //>
#define PT_ERROR   -5  //prazdne policko
#define PT_END     -6  //konec vyrazu v tokenu je uz dalsi vec mimo vyraz
                       //stejne se to ale musi zpracovat takze pokud to prijde
                       //do toho switche tak je to chyba

#define DOLLAR      8  //dno zasobniku
#define PA_E       -7  //neterminal ktery pouziva precedencni analyza

int prec_table[21][21]={
           //  ^        *        /       mod       +        -        ..       <         >       <=     >=        ~=        ==      (         )       bool   number   string    nil       id       $
/* ^    */{PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* *    */{PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* /    */{PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* mod  */{PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* +    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* -    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* ..   */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* <    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* >    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* <=   */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* >=   */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* ~=   */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* ==   */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_LEVY ,PT_PRAVY,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_PRAVY},
/* (    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_ROVNO,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_ERROR},
/* )    */{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/*bool  */{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/*number*/{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/*string*/{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/*nil   */{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/* id   */{PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_PRAVY,PT_ERROR,PT_PRAVY,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_ERROR,PT_PRAVY},
/* $    */{PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_END  ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_LEVY ,PT_END  },
};


typedef struct DATA{
  int type;
  int value;
}Data;

typedef struct Sitem{
  Data data; //data
  struct Sitem* next; //dalsi
  struct Sitem* prev; //predchozi
}TItem;

typedef struct stack{
  TItem *top;
}TStack;

/*======= Prototypy lok. funkci ==============================================*/
int MakeReduction(TStack *Stack,t_table *Tprom, t_table *Tlit,instruction **instr);

int loadTokenMakeData(Data *data,t_table *Tprom, t_table *Tlit);
Data makedata(t_table *Tprom, t_table *Tlit);  //udela z tokenu data pro zasobnik
int Sinit(TStack *Stack);
int Spush(TStack* Stack,Data data);
Data Stop(TStack *Stack);
void Spop(TStack *Stack);
void SUDMA_free(TStack *Stack);
int SaddPTL(TStack *Stack);
Data Stopterm(TStack *Stack);
int getindex(int a);
int PTable(int stackitem,int inputitem);
int MYmakeinstr(instruction **instr,int type,int op1,int op2,int op3);

void printData(Data data);
void Sprint(TStack Stack);
/*============================================================================*/

//funkce pres kterou se spousti expresion parser
bool expres_parser(t_table *Tprom, t_table *Tlit, instruction **instr, int *index){

  TStack Stack;  //zasobnik symbolu
  Data data;     //token transformovany na moje data
  
  *instr=NULL;
  
  
  //chyba inicializace(pridava se dollar na spodek zasobniku)
  if(!Sinit(&Stack)){
    err_flg=ERR_INT;
    #ifdef TESTOVANI
      printf("Chyba EXPAR:1\n");
    #endif
    return false;
  }
  
  
  data=makedata(Tprom,Tlit);
  if(err_flg){
    #ifdef TESTOVANI
      printf("Chyba EXPAR:2\n");
    #endif
    return false;
  }
  
  //hlavni cyklus cteni tokenu
  do{
    #ifdef TESTOVANI
    Sprint(Stack);
    #endif
    
    switch(PTable(Stopterm(&Stack).type,data.type)){
    
      case PT_LEVY :      //V TABULCE JSME NASLI <
          
          //pridam < pred nejvrchnejsi terminal
          if(!SaddPTL(&Stack)){
            err_flg=ERR_INT;
            SUDMA_free(&Stack);
            instrUDMA_free(*instr);
            *instr=NULL;
            #ifdef TESTOVANI
              printf("Chyba EXPAR:3\n");
            #endif            
            return false;
          }
      //POZOR POKRACUJEME DALE
      
      case PT_ROVNO :     //V TABULCE JSME NASLI =
      
          //push(a)
          if(!Spush(&Stack,data)){
            err_flg=ERR_INT;
            SUDMA_free(&Stack);
            instrUDMA_free(*instr);
            *instr=NULL;
            #ifdef TESTOVANI
              printf("Chyba EXPAR:4\n");
            #endif
            return false;
          }
          //read(a)
          if(!loadTokenMakeData(&data,Tprom,Tlit)){
            SUDMA_free(&Stack);
            instrUDMA_free(*instr);
            *instr=NULL;
            #ifdef TESTOVANI
              printf("Chyba EXPAR:5\n");
            #endif           
            return false;
          }         
            
      break;
        
      case PT_PRAVY :     //V TABULCE JSME NASLI >
          
          //redukujeme zasobnik a vytvarime instrukce
          if(!MakeReduction(&Stack,Tprom,Tlit,instr)){
            SUDMA_free(&Stack);
            instrUDMA_free(*instr);
            *instr=NULL;
            #ifdef TESTOVANI
              printf("Chyba EXPAR:6\n");
            #endif            
            return false;
          }

        break;
    
      default :    //V TABULCE JSME NASLI PT_ERROR nebo PT_END coz je taky chyba;
          #ifdef TESTOVANI
            printf("SPATNA KOMBINACE V PRECEDENCNI TABULCE.\n");
            PTable(Stopterm(&Stack).type,data.type);
            printf("Posledni data: ");
            printData(data);
            data=Stopterm(&Stack);
            printf("Topterm: ");
            printData(data);
            Sprint(Stack);
          #endif
          SUDMA_free(&Stack);
          //uvolneni seznamu instrukci co jsem dosud udelal
          instrUDMA_free(*instr);
          *instr=NULL;
          #ifdef TESTOVANI
            printf("Chyba EXPAR:7\n");
          #endif
          err_flg=ERR_SYN;
          return false;
      break;
    } 
    
  // } while(getindex(data.type)!=20);
  //mam dva $
  } while(PTable(Stopterm(&Stack).type,data.type)!=PT_END);

  
  #ifdef TESTOVANI
    Sprint(Stack);
    printf("Posledni data: ");
    printData(data);
  #endif
  
  //zjistim na ktere adrese je navratova hodnota celeho vyrazu
  data=Stop(&Stack);
  *index=data.value;
  
  SUDMA_free(&Stack);

  return true;
}

//dostane zasobnik a aplikuje na nej pravidlo ktere mame
//nastavuje promennou err_flg
int MakeReduction(TStack *Stack,t_table *Tprom, t_table *Tlit,instruction **instr){

  Data data1;
  Data data2;
  Data data3;
  Data neterm;
  
  int operator;
  
  t_symbol *symbol; //kvuli semantickym kontrolam
  

//><><><><><><><><><><><><><><
  //presunu si vrchol zasobniku
  data3=Stop(Stack);
  Spop(Stack);
  
  //jestli jsem presunul neco co nemuzu rekudovat chyba
  if((data3.type==DOLLAR)||
     (data3.type==PT_LEVY)){
    #ifdef TESTOVANI
      printf("SPATNY HANDLE NA ZASOBNIKU 1\n");
    #endif
    err_flg=ERR_SYN;
    return 0;
  }
  
  //na vrcholu je dollar - nejaka blbost
  if(Stop(Stack).type==DOLLAR){
    #ifdef TESTOVANI
      printf("SPATNY HANDLE NA ZASOBNIKU 2\n");
    #endif
    err_flg=ERR_SYN;
    return 0;
  }
  
  //povedlo se muzu redukovat pravidly pro jeden token:)
  if(Stop(Stack).type==PT_LEVY){
    //odstraneni < z zasobniku
    Spop(Stack);
    data1=data3;
//*********************************************************
//  PRAVIDLA S JEDNIM OPERANDEM
//  operand je v data1
// zasobnik je vycisteny a pripraveny pro nahrani neterminalu 
  
    //promenna uz je v tabulce / je to zkontrolovane literaly jsou pridane do tabulky symbolu
    switch(data1.type){
      case ID     :                                              //pravidla  E -> id
      case BOOL   :                                              //          E -> bool
      case NUMBER :                                              //          E -> number
      case STRING :                                              //          E -> string
      case NIL    :                                              //          E -> nil
          
          neterm.type=PA_E;
          neterm.value=data1.value;
          
          //dam neterminal na zasobnik
          if(!Spush(Stack,neterm)){
            err_flg=ERR_INT;
            return 0;
          }
          
        break;
        
      default     :
        //spatny vstup - sice je neco na zasobniku a v datech ale nemam pro to pravidlo
        
        #ifdef TESTOVANI
          printf("SPATNY HANDLE NA ZASOBNIKU 3\n");
        #endif
        err_flg=ERR_SYN;
        return 0;
      
        break;
    }

    return 1;
  }
  
  //je tam dalsi prvek - pokracuju v nacitani
//><><><><><><><><><
  data2=Stop(Stack);
  Spop(Stack);
  
  //pravidla se dvema tokeny nemam takze automaticky chyba
  if((Stop(Stack).type==DOLLAR)||
     (Stop(Stack).type==PT_LEVY)){
    #ifdef TESTOVANI
      printf("SPATNY HANDLE NA ZASOBNIKU 4\n");
    #endif
    err_flg=ERR_SYN;
    return 0;     
  }
//><><><><><><><><><><>
  data1=Stop(Stack);
  Spop(Stack);
  
  //mam tri operandy a na vrcholu je < ->spravny handle
  if(Stop(Stack).type==PT_LEVY){
    //odstraneni <
    Spop(Stack);
  
//********************************************************
//   PRAVIDLA SE TREMI OPERANDY
//operandy jsou data1 data2 data3 presne jak maji byt - uz je obraceno poradi na zasobniku
// zasobnik je vycisteny a pripraveny pro nahrani neterminalu  
  
    
    if((data1.type==PA_E)&&(data3.type==PA_E)){                  //pravidla E -> E ? E
      //je to nejaka aritmeticka operace
      
        //priprava na vysledek ktery tam stejne musi byt
        neterm.type=PA_E;
        //pridam promennou a dostanu zpatky index
        neterm.value=pridej_systemovou(Tprom);
        
        //dam neterminal na zasobnik
        if(!Spush(Stack,neterm)){
          err_flg=ERR_INT;
          return 0;
        }
        //generuju prvni instrukci LOCAL ktera zde musi byt pro vse
        if(!MYmakeinstr(instr,1,0,0,neterm.value)){
          err_flg=ERR_INT;
          return 0;
        }
    
      //rozdeleni operaci a generovani instrukci
      
      switch(data2.type){
        case POW  :
            operator=3;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 1\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 2\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case MUL  :
            operator=4;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 3\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 4\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case DIV  :
            operator=5;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 5\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 6\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case MOD  :
            operator=6;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 7\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 8\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case PLUS :
            operator=7;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 9\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 10\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case MINUS:
            operator=8;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 11\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=NUMBER){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 12\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case CONC :
            operator=9;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(symbol->typ!=STRING){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 13\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(symbol->typ!=STRING){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 14\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case LT   :
            operator=10;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 15\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 16\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case GT   :
            operator=11;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 17\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 18\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case LE   :
            operator=12;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 19\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 20\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case GE   :
            operator=13;
            if(data1.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data1.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 21\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
            if(data3.value<0){
              //je to literal tak musim zkontrolovat jestli tady muze byt
              symbol=najdi_symbol(Tlit,my_itoa(data3.value));
              if(!((symbol->typ==STRING)||(symbol->typ==NUMBER))){
                #ifdef TESTOVANI
                  printf("EXPR semanticka chyba 22\n");
                #endif
                err_flg=ERR_SEM;
                return 0;
              }
            }
          break;
        case EQ   :
            operator=14;
            //tady muze byt jakykoli typ
          break;
        case NE   :
            operator=15;
            //tady muze byt jakykoli typ
          break;
        default   :
          //je tam nejaka blbost co neni operator
            #ifdef TESTOVANI
              printf("SPATNY HANDLE NA ZASOBNIKU 5\n");
            #endif
            err_flg=ERR_SYN;
            return 0;
          break;
      }
      
      //operace NE potrebuje dve instrukce - vsechny ostatni jednu
      if(operator<15){
        //generuju druhou instrukci v zavislosti na data2.type ->operator
        if(!MYmakeinstr(instr,operator,data1.value,data3.value,neterm.value)){
          err_flg=ERR_INT;
          return 0;
        }
      }
      else{
        //operace NE se sklada z EQ a NOT
        if(!MYmakeinstr(instr,14,data1.value,data3.value,neterm.value)){
          err_flg=ERR_INT;
          return 0;
        }
        if(!MYmakeinstr(instr,15,neterm.value,0,neterm.value)){
          err_flg=ERR_INT;
          return 0;
        }        
      }
      
      return 1;
    }

    
    if((data1.type==L_BRACKET)&&(data3.type==R_BRACKET)){        //pravidlo  E-> ( E )
      
      //mam dve zavorky ale mezi nima neni operand
      if(data2.type!=PA_E){
      #ifdef TESTOVANI
        printf("SPATNY HANDLE NA ZASOBNIKU 6\n");
      #endif
      err_flg=ERR_SYN;
      return 0;
      }
      
      //vse v poradku - neni to syntakticka chyba
        neterm.type=PA_E;
        neterm.value=data2.value;
        
        //dam neterminal na zasobnik
        if(!Spush(Stack,neterm)){
          err_flg=ERR_INT;
          return 0;
        }
      
      return 1;
    }
    else{
      #ifdef TESTOVANI
        printf("SPATNY HANDLE NA ZASOBNIKU 7\n");
      #endif
      err_flg=ERR_SYN;
      return 0;
    }
    
    
  }
  else{
  //je tam neco jineho takze ynovu blbost
    #ifdef TESTOVANI
      printf("SPATNY HANDLE NA ZASOBNIKU 8\n");
    #endif
    err_flg=ERR_SYN;
    return 0;     
  }

  return 1;
}

//udela presne to co je v nazvu
int loadTokenMakeData(Data *data,t_table *Tprom, t_table *Tlit){

  loadToken();
  if(err_flg)
    return 0; 

  (*data)=makedata(Tprom,Tlit);
  if(err_flg)
    return 0;    
  
  return 1;
}

//udela z tokenu data pro zasobnik, umisti literal do tabulky, zkontroluje
// jestli je promena deklarovana - je v tabulce
//NASTAVI PROMENNOU ERR_FLG
Data makedata(t_table *Tprom, t_table *Tlit){

  #ifdef MIMOPARSER
    t_value hodnota;
  #endif

  Data data;
  data.type=token.type;
  data.value=0;
  
  t_value univalue;
  t_symbol *prom;
  
  //jestli to ma nejaky atribut ktery potrebujeme tak ho zpracujeme
  switch(token.type){
    case ID:
        //zkontroluje jestli je v tabulce symbolu a prida index do data.value
        
        #ifdef MIMOPARSER
          hodnota.num=0;
          pridej_promennou(Tprom,token.attr,hodnota,NUMBER);
        #endif
        
        prom=najdi_symbol(Tprom, token.attr);
        //pokud neni v tabulce tak je to nedeklarovana promenna
        if(prom==NULL){
          err_flg=ERR_SEM;
          return data;
        }
        
        //jinak dam jeji index do dat
        data.value=prom->index;
    
        
      break;
    case NUMBER:
        univalue.num=atoi(token.attr);
        data.value=pridej_literal(Tlit,univalue,NUMBER);
        //jestli se stala nejaka chyba pro pridavani
        if(!data.value)
          err_flg=ERR_INT;
    
      break;
    case BOOL:
        univalue.flag=!strcmp("true",token.attr);
        data.value=pridej_literal(Tlit,univalue,BOOL);
        //jestli se stala nejaka chyba pro pridavani
        if(!data.value)
          err_flg=ERR_INT;
    
      break;
    case STRING:
        univalue.str=token.attr;
        data.value=pridej_literal(Tlit,univalue,STRING);
        //jestli se stala nejaka chyba pro pridavani
        if(!data.value)
          err_flg=ERR_INT;
        
      break;
    case NIL:
        data.value=pridej_literal(Tlit,univalue,NIL);
        //jestli se stala nejaka chyba pro pridavani
        if(!data.value)
          err_flg=ERR_INT;
    
      break;
    default: 
      //uz je to zpracovano tim typem
      break;
  }
  
  return data;
}

//inicializace - da na vrchol zasobniku dollar
int Sinit(TStack *Stack){

  Data data;
  data.type=DOLLAR;
  data.value=0;
  Stack->top=NULL;
  return Spush(Stack,data);
}

//da prvek na vrchol zasobniku
int Spush(TStack *Stack,Data data){
  
  TItem *item;
  if((item=(TItem *)UDMA_malloc(sizeof(TItem)))==NULL)
    return 0;
    
  if(Stack->top!=NULL)
    Stack->top->prev=item;
  
  item->next=Stack->top;
  item->prev=NULL;
  item->data.type=data.type;
  item->data.value=data.value;
  Stack->top=item;
  
  return 1;
}

//vraci vrchni prvek zasobniku
Data Stop(TStack *Stack){
  
  Data data;
  data.type=0;
  data.value=0;
  if(Stack->top!=NULL)
    data=Stack->top->data;
 
  return data;
}

//odstraneni vrchniho prvku ze zasobniku
void Spop(TStack *Stack){
  
  TItem *item=Stack->top;
  if(Stack->top!=NULL){
    Stack->top=Stack->top->next;
    UDMA_free(item);
    if(Stack->top!=NULL)
      Stack->top->prev=NULL;
  }
  return;
}

//uvolneni celeho zasobniku, vcetne DOLLARU
void SUDMA_free(TStack *Stack){
  
  TItem *item=Stack->top;
  
  while(Stack->top!=NULL){
    Stack->top=Stack->top->next;
    UDMA_free(item);
    item=Stack->top;
  }  
}

//prida pred terminal nejblize vrcholu < (PT_LEVY)
int SaddPTL(TStack *Stack){
  TItem *item=Stack->top;
  TItem *new;
  
  if((new=(TItem *)UDMA_malloc(sizeof(TItem)))==NULL)
    return 0;
  
  new->data.type=PT_LEVY;
  new->data.value=0;
  
  if(Stack->top==NULL){
    //na vrcholu zasobniku po inicializaci musi byt minimalne DOLLAR
    UDMA_free(new);
    return 0;
  }
  
  //hledam nejvrchnejsi terminal
  while((item->next!=NULL) && (item->data.type < 0))
    item=item->next;
  
  //nasel jsem ho
  if(item->data.type > 0){
    if(item!=Stack->top){
      //nasel jsem nekde v zasobniku
      new->prev=item->prev;
      new->next=item;
      new->prev->next=new;
      new->next->prev=new;
    }
    else{
      //je to vrchol zasobniku
      new->next=item;
      item->prev=new;
      Stack->top=new;
    } 
  }
  else{
    //jsem u posledniho a neni to terminal - nekdo neinicializoval tabulku
    UDMA_free(new);
    return 0;
  }
  
  return 1;
}

//vraci nejvrchnejsi terminal
Data Stopterm(TStack *Stack){

  TItem *item=Stack->top;
  Data data;
  data.type=0;
  data.value=0;
  
  while((item!=NULL) && (item->data.type < 0))
    item=item->next;
  
  if(item!=NULL)
    return item->data;
  else
    return data;
}

//mapovaci funkce tokenu na index
int getindex(int a){
  switch (a){
    //konkretni indexy jak mam nahore naimplementovanou tabulku..
    case POW      : a=0;   break;
    case MUL      : a=1;   break;
    case DIV      : a=2;   break;
    case MOD      : a=3;   break;
    case PLUS     : a=4;   break;
    case MINUS    : a=5;   break;
    case CONC     : a=6;   break;
    case LT       : a=7;   break;
    case GT       : a=8;   break;
    case LE       : a=9;   break;
    case GE       : a=10;  break;
    case NE       : a=11;  break;
    case EQ       : a=12;  break;
    case L_BRACKET: a=13;  break;
    case R_BRACKET: a=14;  break;
    case BOOL     : a=15;  break;
    case NUMBER   : a=16;  break;
    case STRING   : a=17;  break;
    case NIL      : a=18;  break;
    case ID       : a=19;  break;
    default       : a=20;  break;
  }
  return a;
}

//konverze tokenu a dalsich veci ktere maji jine indexy nez potrebuju do tabulky
// a - terminal na zasobniku
// b - token na vstupu
int PTable(int stackitem,int inputitem){
  
  int index_a=getindex(stackitem);
  int index_b=getindex(inputitem);
  
  #ifdef TESTOVANI
    printf("Pristup do tabulky: %2d x %2d\n",index_a,index_b);
  #endif
  
  return prec_table[index_a][index_b];
}

//vzgeneruje instrukci
int MYmakeinstr(instruction **instr,int type,int op1,int op2,int op3){

  instruction *pominstr;
  
  #ifdef TISKINSTRUKCI
    printf("                                instrukce: ");
    switch(type){
      case  1: printf("LOCAL"); break;
      case  2: printf("COPY "); break;
      case  3: printf("POW  "); break;
      case  4: printf("MUL  "); break;
      case  5: printf("DIV  "); break;
      case  6: printf("MOD  "); break;
      case  7: printf("PLUS "); break;
      case  8: printf("MINUS"); break;
      case  9: printf("CONC "); break;
      case 10: printf("LT   "); break;
      case 11: printf("GT   "); break;
      case 12: printf("LE   "); break;
      case 13: printf("GE   "); break;
      case 14: printf("EQ   "); break;
      case 15: printf("NOT  "); break;
      default: printf("NECO JE SPATNE"); break;
    }
    printf(" : %4d %4d %4d\n",op1,op2,op3);
  #endif
  
  switch(type){
      case  1: type=LOCAL   ; break;
      case  2: type=COPY    ; break;
      case  3: type=POW     ; break;
      case  4: type=MUL     ; break;
      case  5: type=DIV     ; break;
      case  6: type=MOD     ; break;
      case  7: type=PLUS    ; break;
      case  8: type=MINUS   ; break;
      case  9: type=CONC    ; break;
      case 10: type=LT      ; break;
      case 11: type=GT      ; break;
      case 12: type=LE      ; break;
      case 13: type=GE      ; break;
      case 14: type=EQ      ; break;
      case 15: type=NOT     ; break;
      default: fprintf(stderr,"NECO JE SPATNE HODNE HODNE SPATNE\n"); break;
    }
  
  if((pominstr=(instruction *)UDMA_malloc(sizeof(instruction)))==NULL){
    err_flg=ERR_INT;
    return 0;
  }
  
  pominstr->type=type;
  pominstr->op1=op1;
  pominstr->op2=op2;
  pominstr->op3=op3;  
  pominstr->next=NULL;
  
  if(*instr==NULL)
    //zatim nemam zadnou instrukci tak nemuzu delat konkatenaci
    *instr=pominstr;
  else
    FT_concatinstr(*instr,pominstr);
  
  return 1;
}


//vytiskne zasobnik
void Sprint(TStack Stack){
  printf("=====ZASOBNIK==================\n");
  for(TItem *item=Stack.top ; item!=NULL ; item=item->next){
    printData(item->data);
  }
  printf("===============================\n");
}

//vytiskne jednu polozku zasobniku
void printData(Data data){
    switch(data.type){
      case POW      : printf(" ^    "); break;
      case MUL      : printf(" *    "); break;
      case DIV      : printf(" /    "); break;
      case MOD      : printf(" %%    "); break;
      case PLUS     : printf(" +    "); break;
      case MINUS    : printf(" -    "); break;
      case CONC     : printf(" ..   "); break;
      case LT       : printf(" <    "); break;
      case GT       : printf(" >    "); break;
      case LE       : printf(" <=   "); break;
      case GE       : printf(" >=   "); break;
      case NE       : printf(" ~=   "); break;
      case EQ       : printf(" ==   "); break;
      case L_BRACKET: printf(" (    "); break;
      case R_BRACKET: printf(" )    "); break;
      case BOOL     : printf(" bool "); break;
      case NUMBER   : printf("number"); break;
      case STRING   : printf("string"); break;
      case NIL      : printf(" nil  "); break;
      case ID       : printf(" id   "); break;
      case DOLLAR   : printf(" $    "); break;
      case PT_LEVY  : printf("  vvvv"); break;
      case PT_PRAVY : printf("  ^^^^"); break;
      case PA_E     : printf(" E    "); break;
      default       : printf("%3d   ",data.type); break;
    }
    printf(" . %2d\n",data.value);
}

