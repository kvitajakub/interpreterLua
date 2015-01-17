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
*   Soubor:  instruction_list.c
*   Popis:   Implementace funkci pracujicich se seznamem instrukci.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "instruction_list.h"
/*============================================================================*/

int label_counter=1; //pocitadlo pro index navesti


//navrat indexu navesti
int LabelIndex(){
  return label_counter++;
}

//inicializace nove tabulky
void FT_init(FTable *Ftable){
  Ftable->count=-1;  // neni tam zadny zapis
  Ftable->function=NULL;
}

//prdani nove funkce do tabulky
int FT_addfunc(FTable *Ftable,char *name){
  
  Ftable->count++;
  //akolace ukazatele na novy prvek v tabulce
  if((Ftable->function=(func **)UDMA_realloc(Ftable->function, (Ftable->count+1)*sizeof(func *)))==NULL)
    return 0;
    
  //alokace noveho prvku
  if((Ftable->function[Ftable->count]=(func *)UDMA_malloc(sizeof(func)))==NULL)
    return 0;
  
  //inicializace
  Ftable->function[Ftable->count]->name=name; //kopirovani celeho retezce
  Ftable->function[Ftable->count]->instr=NULL;
  
  return 1;
}

//vytvori instrukci z jejich prvku
instruction *makeinstr(int type,int op1,int op2,int op3){
  
  instruction *instr;
  
  if((instr=(instruction *)UDMA_malloc(sizeof(instruction)))==NULL)
    return NULL;
  
  instr->type=type;
  instr->op1=op1;
  instr->op2=op2;
  instr->op3=op3;
  instr->next=NULL;
  
  return instr;
}

//vrati index na kterem je v tabulce funkce s jmenem name
int FT_findfunc(FTable *Ftable,char *name){
  
  int index=0;
  
  //zvysuju index pokud mam kde hledat a jeste jsem nenasel
  while((index<=Ftable->count)&&(strcmp(Ftable->function[index]->name,name)))
    index++;
    
  //pokud je vetsi tak jsem nenasel -vracim chybnou hodnotu
  if(index>Ftable->count)
    return -1;
  else
    //nasel jsem tak vracim ten index
    return index;
}

//spoji dva seznamy instrukci
int FT_concatinstr(instruction *inst1,instruction *inst2){
  
  if(inst1==NULL)
    return 0;
  
  //pokud je druhy NULL tak nemusim hledat protoze uz tam je
  if(inst2==NULL)
    return 1;
    
  //najdu posledni prvek prvniho seznamu
  while(inst1->next!=NULL)
    inst1=inst1->next;
  
  //prilepim druhy seznam na konec prvniho
  inst1->next=inst2;
  return 1;
}

//uvolneni seznamu instrukci
void instrUDMA_free(instruction *instr){
  
  instruction *pom=instr;
  while(instr!=NULL){
    instr=instr->next;
    UDMA_free(pom);
    pom=instr;
  }
}

//zruseni cele tabulky funkci
void FT_destroy(FTable *Ftable){
  //uvolneni jedoho prvku tabulky
  for(int i=0;i<=Ftable->count;i++){
    instrUDMA_free(Ftable->function[i]->instr);
    UDMA_free(Ftable->function[i]);
  }
  //uvolneni tabulky - ukazatelu
  UDMA_free(Ftable->function);
  
}

void printinstr(instruction *instr){
  
  switch(instr->type){
    case READ_N     : printf("READ_N     : "); break;
    case READ_L     : printf("READ_L     : "); break;
    case READ_A     : printf("READ_A     : "); break;
    case READ_NUMB  : printf("READ_NUMB  : "); break;
    case WRITE      : printf("WRITE      : "); break;
    case LABEL      : printf("LABEL      : "); break;
    case IFNOTGOTO  : printf("IFNOTGOTO  : "); break;
    case GOTO       : printf("GOTO       : "); break;
    case NOT        : printf("NOT        : "); break;
    case COPY       : printf("COPY       : "); break;
    case PLUS       : printf("PLUS       : "); break;
    case MINUS      : printf("MINUS      : "); break;
    case POW        : printf("POW        : "); break;
    case MUL        : printf("MUL        : "); break;
    case DIV        : printf("DIV        : "); break;
    case MOD        : printf("MOD        : "); break;
    case LT         : printf("LT         : "); break;
    case LE         : printf("LE         : "); break;
    case GT         : printf("GT         : "); break;
    case GE         : printf("GE         : "); break;
    case EQ         : printf("EQ         : "); break;
    case SUBSTR_FIND: printf("SUBSTR_FIND: "); break;
    case SUBSTR_CUT : printf("SUBSTR_CUT : "); break;
    case SORT       : printf("SORT       : "); break;
    case TYPE       : printf("TYPE       : "); break;
    case CALL_FUN   : printf("CALL_FUN   : "); break;
    case QUE_UP     : printf("QUE_UP     : "); break;
    case QUE_DOWN   : printf("QUE_DOWN   : "); break;
    case QUE_CLEAN  : printf("QUE_CLEAN  : "); break;
    case LOCAL      : printf("LOCAL      : "); break;
    case CONC       : printf("CONC       : "); break;
    case RETURN     : printf("RETURN     : "); break;
    default  : printf("MAME ALE FAKT VELKY PROBLEM S INSTRUKCEMA"); break;
  }  
  printf("%4d %4d %4d\n",instr->op1,instr->op2,instr->op3);
}

//vytiskne cely seznam instrukci
void printinstrlist(instruction *instr){
  for(;instr!=NULL;instr=instr->next)
    printinstr(instr);
}

//vytiskne jednu funkci
void printfunc(func *function){
  
  printf("======FUNKCE==============\n");
  printf("Jmeno: %s\n",function->name);
  printf("Instrukce funkce:\n");
    printinstrlist(function->instr);
  printf("======KONEC===============\n");
}


/***** konec souboru****/