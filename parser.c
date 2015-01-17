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
*   Soubor:  parser.c
*   Popis:   Implementace syntaktickeho a semantickeho analyzatoru.
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include "err_handler.h"  // Osetreni chyb, chybove konstanty
#include "parser.h"
#include "express_parser.h"
#include "instruction_list.h"
#include <math.h>
#include <limits.h>

/*============================================================================*/

// formaty pre prikaz read

#define SPEC_NONE      -1
#define SPEC_READ_N    -2
#define SPEC_READ_L    -3
#define SPEC_READ_A    -4
#define SPEC_READ_NUMB -5


// typ funkcie

#define FUNC_TYPE      0
#define FUNC_SUBSTR    1
#define FUNC_FIND      2
#define FUNC_SORT      3
#define FUNC_OTHER     4

bool statment_sequence(FTable *fTable, t_table *tProm, t_table *tLit);
bool alt_statment_sequence(FTable *fTable, t_table *tProm, t_table *tLit);

/*
 * Pomocna funkcia na vypisovanie chybovych hlaseni
 */
void typeName(int type)
{
  switch(type)
  {
    case END_OF_FILE:
      fprintf(stderr, "END_OF_FILE");
      break;
    case ID:
      fprintf(stderr, "ID");
       break;
    case NUMBER:
      fprintf(stderr, "NUMBER");
       break;
    case BOOL:
      fprintf(stderr, "BOOL");  
      break;
    case STRING:
      fprintf(stderr, "STRING");  
      break;
      
    case PLUS:
      fprintf(stderr, "PLUS");  
      break;
    case MINUS:
      fprintf(stderr, "MINUS"); 
      break;
    case MUL:
      fprintf(stderr, "MUL");
      break;
    case DIV:
      fprintf(stderr, "DIV");
      break;
    case POW:
      fprintf(stderr, "POW"); 
      break;
    case MOD:
      fprintf(stderr, "MOD"); 
      break;
      
    case ASSIGN:
      fprintf(stderr,  " = ");
      break;
      
    case EQ:
      fprintf(stderr, " == ");
      break;
      
    case NE:
      fprintf(stderr, " != "); 
      break;
  
    case LT:
      fprintf(stderr, " < ");
      break;
    case GT:
      fprintf(stderr, " > ");
      break;
    case LE:
      fprintf(stderr, " <= ");
      break;
    case GE:
      fprintf(stderr, " >= ");
      break;
    case L_BRACKET:
      fprintf(stderr, "L_BRACKET"); 
      break;
    case R_BRACKET:
      fprintf(stderr, "R_BRACKET");
      break;
       
    case SEMICOLON:
      fprintf(stderr, "SEMICOLON"); 
      break;
    case CONC:
      fprintf(stderr, " .. "); 
      break;
    case COMMA:
      fprintf(stderr, "COMMA"); 
      break;
    case DO:
      fprintf(stderr, "DO");
      break;
    case ELSE:
      fprintf(stderr, "ELSE");
      break;
    case END:
      fprintf(stderr, "END"); 
      break;
    case FUNCTION:
      fprintf(stderr, "FUNCTION"); 
      break;
    case IF:
      fprintf(stderr, "IF");  
      break;
    case LOCAL:
      fprintf(stderr, "LOCAL"); 
      break;
   
    case NIL:
      fprintf(stderr, "NIL");  
      break;
    case READ:
      fprintf(stderr, "READ"); 
      break;
    case RETURN:
      fprintf(stderr, "RETURN"); 
      break;
    case THEN:
      fprintf(stderr, "THEN");
      break;
    case WHILE:
      fprintf(stderr, "WHILE");
      break;
    case WRITE:
      fprintf(stderr, "WRITE");
      break;
      
    case RESERVED:
      fprintf(stderr, "RESERVED");  
      break;
      
   default:
      fprintf(stderr, "%d ",type);  
      break;
  } 
  

}

/*
 * Nacita aktualny token do globalnej premennej
 */

void loadToken()
{
  token = get_token();
}

/*******************************************************************
 * Funkcia literal simuluje pravidla:
 * literal -> number
 * literal -> string
 * literal -> bool
 * literal -> nil
 * 
 * indexL je index v tabulke literalov kam s ulozi literal
**/
bool literal(int *indexL, t_table *tLit, int * special)
{
  bool result = false;
  
  t_value hodnota; // hodnota literalu
  char ** hlpPtr = NULL;
  
  switch (token.type)
  {
    case NUMBER:
      
       
      hodnota.num = strtod(token.attr, hlpPtr); 
      if (hlpPtr != NULL)
      {
	err_flg = ERR_INT;
	fprintf(stderr, "Error wrong number format\n");
	result = false;
	break;
      }
      
      /*
       * do special vlozi hodnotu ak je kladna
       */
        double a =  trunc(hodnota.num);  // pomocna premenna
	
	if (a > INT_MAX)
	{
	  err_flg = ERR_INT;
	  fprintf(stderr, "Trying to read string with too many characters\n");
	  result = false;
	  break;
	}
	else
	{
          *special = (int) a; 
	}
      
      *indexL = pridej_literal(tLit, hodnota,token.type); // prida literal do tabulky symobolov
      
      loadToken();
      result = true;
      
      break;
      
      
    case STRING:
      hodnota.str = token.attr;
      
      *indexL = pridej_literal(tLit, hodnota,token.type); // prida literal do tabulky symobolov
      
      // skontroluje ci sa nejedna o specialny format
      if (strcmp(token.attr,"*n") == 0)
      {
	*special = SPEC_READ_N; 
      }
      else if (strcmp(token.attr,"*l") == 0)
      {
	*special = SPEC_READ_L;
      }
      else if (strcmp(token.attr,"*a") == 0)
      {
	*special = SPEC_READ_A;
      }
   
      
      loadToken();
      result = true;
      
      
      
      break;
      
    case BOOL:
      
      if (strcmp(token.attr,"true") == 0)
      {
        hodnota.flag = true;
      }
      else
      {
	hodnota.flag = false;
      }
      
      *indexL = pridej_literal(tLit, hodnota,token.type); // prida literal do tabulky symobolov
      loadToken();
      result = true;
      
      break;      
      
    case NIL:
      
      hodnota.str = NULL;
      
      *indexL = pridej_literal(tLit, hodnota,token.type);
      loadToken();
      result = true;
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line); 
      typeName(token.type);
      fprintf(stderr, ", expected LITERAL\n");
      break;
  
  }
  
  
  return result;
}


/*******************************************************************
 * Funkcia next_expresion simuluje pravidla:
 * next-expresion -> , expresion next-expresion
 * next-expresion ->  )    
**/

bool next_expresion(FTable *fTable, t_table *tProm, t_table *tLit)
{
  int index;
  bool result = false;
  instruction * instrExpr; // zoznam instukcii vrateny express_parserom  
  
  switch (token.type)
  {
    case R_BRACKET:
      
      loadToken();
      result = true;
      
      break;
      
      case COMMA:
      
      loadToken();
        
      
      if (expres_parser(tProm,tLit, &instrExpr,&index) == true)
      {
      
        // vygenerovanie instrucie write
        instruction *instr = makeinstr(WRITE,0,0,index);

	
        if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
        {
	  instrExpr = instr;
        }
        else
        {
	  // spojenie istrukcii vyrazoveho parsera a insrukcie write
	  FT_concatinstr(instrExpr,instr);
        }
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instrExpr;
        }
        else
        {
          FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
        }
        
        result =  next_expresion(fTable,tProm,tLit);
      
      }
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or COMMA\n");
      break;
  
  }
  
  return result;
}


/*******************************************************************
 * Funkcia expresion_sequence simuluje pravidla:
 * expresion-sequence ->  expresion next-expresion
 * expresion-sequence ->  )   
**/

bool expresion_sequence(FTable *fTable, t_table *tProm, t_table *tLit) 
{
  int index; // index systemovej premennej na ktorej je ulozeny vysledok vyrazu
  bool result = false;
  instruction * instrExpr; // zoznam instukcii vrateny express_parserom
  
  switch (token.type)
  {
    case R_BRACKET:
      
      loadToken();
      result = true;
      
      
      break;
      
  
      case NIL:
      case BOOL:
      case L_BRACKET:
      case ID:
      case NUMBER:
      case STRING:
      
     

	
	// vynimka,nenacitava sa novy token, spracovanie vyrazov
	
	// prebehne syntakticka analyza a zaroven sa ulozia instukcie do instrExpr
  
	if (expres_parser(tProm,tLit, &instrExpr,&index) ) 
	{
	
      	   // vygenerovanie instrucie write
          instruction *instr = makeinstr(WRITE,0,0,index);

	
	
	  if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
	  {
	    instrExpr = instr;
	  }
	  else
	  {
	    // spojenie istrukcii vyrazoveho parsera a insrukcie write
	    FT_concatinstr(instrExpr,instr);
	  }
	
          if (fTable->function[fTable->count]->instr == NULL) 
          {
            fTable->function[fTable->count]->instr = instrExpr;
          }
          else
          {
            FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
          }
          
          result =  next_expresion(fTable,tProm,tLit);
        
	}

      
        break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or EXPRESION\n");
      break;
  
  }
  
  return result;
}



/*******************************************************************
 * Funkcia next_parameter simuluje pravidla:
 * next-parameter -> , id  next-parameter 
 * next-parameter ->  )    
**/

bool next_parameter(FTable *fTable, t_table *tProm, t_table *tLit) 
{

  int indexP; // index polozky v tabulke premennych
  bool result = false;
  
  switch (token.type)
  {
    case R_BRACKET:
      
      loadToken();
      result = true;
      
      // vygenerovanie instrucii
      
      
      instruction *instr = makeinstr(QUE_CLEAN,0,0,0);

      if (fTable->function[fTable->count]->instr == NULL) 
      {
	fTable->function[fTable->count]->instr = instr;
      }
      else
      {
        FT_concatinstr(fTable->function[fTable->count]->instr,instr);
      }      
      
      break;
      
    case COMMA:
      
      loadToken();
      
      if (token.type == ID)
      {
	
	// semanticke operacie
	
        if ((najdi_symbol(tProm, token.attr) == NULL) && FT_findfunc(fTable,token.attr) == -1) // nebola definovana premenna ani funkcia
        {                                                                                     // s rovnakym nazvom
	  t_value hodnota;
          hodnota.str= NULL;
	  indexP =  pridej_promennou(tProm, token.attr,hodnota,token.type);
	  
  
	  instruction *instr2 = makeinstr(LOCAL,0,0,indexP);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr2;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr2);
	  }		  
	  
	  // vygenerovanie instrucii
	  instruction *instr = makeinstr(QUE_DOWN,0,0,indexP);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	  }
	  
          loadToken();
	  result = next_parameter(fTable,tProm,tLit);
	  
        }
        else
        {
           err_flg = ERR_SEM;
           fprintf(stderr, "%d: Variable %s was previously defined\n",token.line,token.attr);
	   result = false;
	   break;
        }	
	
      }
      else
      {
        err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
        typeName(token.type);
        fprintf(stderr, ", expected ID\n");
	
      }
      
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or COMMA\n");
      
      break;
  
  }
  
  return result;  
}




/*******************************************************************
 * Funkcia parameters simuluje pravidla:
 * parameters -> id  next-parameter
 * parameters -> )    
**/

bool parameters(FTable *fTable, t_table *tProm, t_table *tLit) 
{
  bool result = false;
  int indexP; // index polozky v tabulke premennych
  
  switch (token.type)
  {
    case R_BRACKET:
      
      loadToken();
      result = true;
      
      // vygenerovanie instrucii    
      
      instruction *instr = makeinstr(QUE_CLEAN,0,0,0);

      if (fTable->function[fTable->count]->instr == NULL) 
      {
	fTable->function[fTable->count]->instr = instr;
      }
      else
      {
        FT_concatinstr(fTable->function[fTable->count]->instr,instr);
      }
      
      
      break;
      
    case ID:
      
      
	// semanticke operacie
	
        if ((najdi_symbol(tProm, token.attr) == NULL) && FT_findfunc(fTable,token.attr) == -1) // nebola definovana premenna ani funkcia
        {                                                                                     // s rovnakym nazvom
	  t_value hodnota;
          hodnota.str= NULL;
	  indexP =  pridej_promennou(tProm, token.attr,hodnota,token.type);

	  // vygenerovanie instrucii
	  instruction *instr2 = makeinstr(LOCAL,0,0,indexP);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr2;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr2);
	  }	  
  
	  
	  instruction *instr = makeinstr(QUE_DOWN,0,0,indexP);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	  }
	  
          loadToken();
	  result = next_parameter(fTable,tProm,tLit);
	  
        }
        else
        {
           err_flg = ERR_SEM;
           fprintf(stderr, "%d: Variable %s was previously defined\n",token.line,token.attr);
	   result = false;
	   break;
        }	
             
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or ID\n");
      break;
  
  }
  
  return result;  
}



/*******************************************************************
 * Funkcia input_parameter simuluje pravidla:
 * input-parameter -> literal
 * input-parameter -> id    
**/

bool input_parameter(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  int indexL; 
  int special = SPEC_NONE;
  t_symbol* symb; // pomocna premenna
  
  switch (token.type)
  {
    case ID:
      
      symb = najdi_symbol(tProm, token.attr);
      
      if (symb == NULL)
      {
	// chyba premenna nebola definovana
         err_flg = ERR_SEM;
         fprintf(stderr, "%d: Variable %s was not defined\n",token.line,token.attr);
	 result = false;
	 break;
      }
      else
      {
         // vygenerovanie instrucii 
     
        instruction *instr = makeinstr(QUE_UP,symb->index,0,0); 
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instr;
        }
        else
        {
          FT_concatinstr(fTable->function[fTable->count]->instr,instr);
        }                
      
      }
      
      
      loadToken();
      result = true;
      
      break;
      
    case NUMBER:
    case NIL:
    case STRING:
    case BOOL:


      // vynimka nenacitava sa novy token, nacitany je aktualny
      
      result = literal(&indexL,tLit,&special);
      
      
      
      // vygenerovanie instrucii 
     
      instruction *instr = makeinstr(QUE_UP,indexL,0,0);
	
      if (fTable->function[fTable->count]->instr == NULL) 
      {
        fTable->function[fTable->count]->instr = instr;
      }
      else
      {
        FT_concatinstr(fTable->function[fTable->count]->instr,instr);
      }                
      
      break;
	
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected ID or LITERAL \n");
      break;
  
  }
  
  return result;     
  
}



/*******************************************************************
 * Funkcia next_input_parameter simuluje pravidla:
 * next-input-parameter -> ,  input-parameter next-input-parameter
 * next-input-parameter -> )     
**/

bool next_input_parameter(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  
  switch (token.type)
  {
    case COMMA:
      
      loadToken();
      result = input_parameter(fTable,tProm,tLit) && next_input_parameter(fTable,tProm,tLit);
      
      break;
      
    case R_BRACKET:
      
      loadToken();
      result = true;
      break;

	
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or COMMA \n");
      break;
  
  }
  
  return result;       
}



/*******************************************************************
 * Funkcia input_parameter_sequence simuluje pravidla:
 * input-parameter-sequence -> input-parameter next-input-parameter
 * input-parameter-sequence -> )    
**/

bool input_parameter_sequence(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  
  switch (token.type)
  {
    case R_BRACKET:
      
      loadToken();
      result = true;
      
      break;
      
  
      case NIL:
      case BOOL:
      case ID:
      case NUMBER:
      case STRING:
	
	// vynimka nenacitava sa novy token, nacitany je aktualny
	
      result = input_parameter(fTable,tProm,tLit) && next_input_parameter(fTable,tProm,tLit);
      
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected R_BRACKET or INPUT-PARAMETER \n");
      break;
  
  }
  
  return result;    
}





/*******************************************************************
 * Funkcia right_value simuluje pravidla:
 * right-value -> read (  literal ) 
 * right-value ->  expresion
 * right-value -> id ( input-parameter-sequence
**/

bool right_value(FTable *fTable, t_table *tProm, t_table *tLit, int where) 
{
  
  int index;
  int indexL;
  int indexF; // index funkcii
  int functionType = FUNC_OTHER;  // typ funkcie
  bool result = false;
  int special = SPEC_NONE;
  instruction * instrExpr; // zoznam instukcii vrateny express_parserom 
  
  switch (token.type)
  {
    case READ:
      
      loadToken();
      
      if (token.type == L_BRACKET)
      {
	loadToken();
	
	if (literal(&indexL,tLit,&special) == true)
	{
	  
	  // vygenerujeme instukcie
	  
	  if (special == SPEC_NONE) // chyba
	  {
	    err_flg = ERR_SEM;
            fprintf(stderr, "%d: Read statment has wrong input format\n",token.line);
	    result = false;
	    break;
	  }	  
	  else if (special == SPEC_READ_N)
	  {
	    	  // vygenerovanie instrucii
	    instruction *instr = makeinstr(READ_N,where,0,0);

	    if (fTable->function[fTable->count]->instr == NULL) 
	    {
	      fTable->function[fTable->count]->instr = instr;
	    }
	    else
	    {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	    }
	  }
	  else if (special == SPEC_READ_A)
	  { 
	    	  // vygenerovanie instrucii
	    instruction *instr = makeinstr(READ_A,where,0,0);

	    if (fTable->function[fTable->count]->instr == NULL) 
	    {
	      fTable->function[fTable->count]->instr = instr;
	    }
	    else
	    {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	    }	    
	  }
	  else if (special == SPEC_READ_L)
	  { 
	    	  // vygenerovanie instrucii
	    instruction *instr = makeinstr(READ_L,where,0,0);

	    if (fTable->function[fTable->count]->instr == NULL) 
	    {
	      fTable->function[fTable->count]->instr = instr;
	    }
	    else
	    {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	    }	    
	  }
          else // special_numb
	  {
	    	  // vygenerovanie instrucii
	    instruction *instr = makeinstr(READ_NUMB,where,special,0); // special urcuje kolko znakov

	    if (fTable->function[fTable->count]->instr == NULL) 
	    {
	      fTable->function[fTable->count]->instr = instr;
	    }
	    else
	    {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	    }	    
	  }
	  	  
	  if (token.type == R_BRACKET)
	  {
	    loadToken();
	    return true;
	  }
	  else
	  {
	    err_flg = ERR_SYN;
	    fprintf(stderr, "%d: Got ", token.line);
            typeName(token.type);
            fprintf(stderr, ", expected R_BRACKET\n");	    
	  }
	  
	}
      }
      else
      {
	err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
        typeName(token.type);
        fprintf(stderr, ", expected L_BRACKET\n");
      }
      
      break;
      
    case ID:  // specialny pripad, nedetermizmus LL tabulky
 
      
	//kontrola na vstavane funcie
	
      if (strcmp(token.attr,"type") == 0)
      {
	  functionType = FUNC_TYPE;
      }
      else if (strcmp(token.attr,"substr") == 0)
      {
	functionType = FUNC_SUBSTR;
      }
      else if (strcmp(token.attr,"find") == 0)
      {
        functionType = FUNC_FIND;
      }
      else if (strcmp(token.attr,"sort") == 0)
      {
        functionType = FUNC_SORT;
      }	


      
      indexF = FT_findfunc(fTable,token.attr);
      if (indexF == -1 && functionType == FUNC_OTHER) // ID nie je funkcia, ale vyraz 
      {
	
	result = expres_parser(tProm,tLit, &(instrExpr),&index);
	
	
           // vygenerovanie instrucie copy
        instruction *instr = makeinstr(COPY,index,0,where);

	
        if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
        {
	  instrExpr = instr;
        }
        else
        {
	  // spojenie istrukcii vyrazoveho parsera a insrukcie copy
	  FT_concatinstr(instrExpr,instr);
        }
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instrExpr;
        }
        else
        {
         FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
        }    	
	
	
	break;
      }
      else // je to funkcia
      {
		
	
        loadToken();
      
        if (token.type == L_BRACKET) 
        {	  
	     
      	  	  	  
	  loadToken();	
	  result = input_parameter_sequence(fTable,tProm,tLit);
	  
	  if (result == true)
	  {
	    // kotrola ci nejde o vstavanu funkciu
	    // a generacia instrukcii
	    
	    instruction *instr;
	    
	    if (functionType == FUNC_TYPE)
	    {
	      instr = makeinstr(TYPE,0,0,0);
	    }
	    else if (functionType == FUNC_SUBSTR)
	    {
	      instr = makeinstr(SUBSTR_CUT,0,0,0);
	    }
	    else if (functionType == FUNC_FIND)
	    {
	      instr = makeinstr(SUBSTR_FIND,0,0,0);
	    }	
	    else if (functionType == FUNC_SORT)
	    {
	      instr = makeinstr(SORT,0,0,0);
	    }	 	    
	    else
	    {
	      instr = makeinstr(CALL_FUN,indexF,0,0);
	    }
	    
            

            if (fTable->function[fTable->count]->instr == NULL) 
            {
	      fTable->function[fTable->count]->instr = instr;
            }
            else
            {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr);
            }
            
            // popnutie vratenej hodnoty
            instruction * instr2 = makeinstr(QUE_DOWN,0,0,where);
	    
            if (fTable->function[fTable->count]->instr == NULL) 
            {
	      fTable->function[fTable->count]->instr = instr2;
            }
            else
            {
              FT_concatinstr(fTable->function[fTable->count]->instr,instr2);
            }	    
            
            
	  }
	  
        }
        else
        {
          err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
          typeName(token.type);
          fprintf(stderr, ", expected L_BRACKET\n");	
        }
        break;
      }
      
   
    case NIL:
    case BOOL:
    case NUMBER:
    case STRING:
    case L_BRACKET:
  
      // medzi pripadmi schvalne nie je ID, je to riesenie nedetermizmu
      // vynimka,nenacitava sa novy token, spracovanie vyrazov
      
      
      result = expres_parser(tProm,tLit, &instrExpr,&index);

      // vygenerovanie instrucie copy
      instruction *instr = makeinstr(COPY,index,0,where);

	
      if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
      {
	  instrExpr = instr;
      }
      else
      {
	// spojenie istrukcii vyrazoveho parsera a insrukcie copy
	FT_concatinstr(instrExpr,instr);
      }
	
      if (fTable->function[fTable->count]->instr == NULL) 
      {
        fTable->function[fTable->count]->instr = instrExpr;
      }
      else
      {
        FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
      }            
      
      break;      
      
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected READ, ID or EXPRESION\n");
      break;
  
  }
  
  return result;
}


/*******************************************************************
 * Funkcia statment simuluje pravidla:
 * statement -> id = right-value ;
 * statement -> write ( expresion-sequence  ;
 * statement -> if expresion then statment-sequence else statment-sequence end ;
 * statement -> while expresion do statment-sequence end ; 
 * statement -> return expresion ;
**/

bool statment(FTable *fTable, t_table *tProm, t_table *tLit) 
{
 
  int index;
  bool result = false;
  int where; // premmenna do ktorej sa uklada
  t_symbol* symb; // pomocna premenna
  instruction * instrExpr; // zoznam instukcii vrateny express_parserom
  
  int label1;  // navestia pouzivane v instrukciach
  int label2;  // while a if-then-else
  
  
  switch (token.type)
  {
    case ID:
      // overenie ci bola premenna definovava
      
      symb = najdi_symbol(tProm, token.attr); 
      if (symb != NULL) // bola definovana
      {
	where = symb->index;

      }
      else
      {
         err_flg = ERR_SEM;
         fprintf(stderr, "%d: Variable %s was not defined\n",token.line,token.attr);
	 result = false;
	 break;
      }
      
      
      loadToken();
      
      if (token.type == ASSIGN)
      {
	loadToken();
	
	if ( right_value(fTable,tProm,tLit,where) == true)
	{
	   if (token.type == SEMICOLON)
	   {
	     loadToken();
	     return true;
	   }
	   else
	   {
	     err_flg = ERR_SYN;
	     fprintf(stderr, "%d: Got ", token.line);
             typeName(token.type);
             fprintf(stderr, ", expected SEMICOLON\n");
	   }
	}
	
      }
      else
      {
        err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
        typeName(token.type);
        fprintf(stderr, ", expected ASSIGN\n");
      }
      
      break;
      
    case WRITE:
      
      loadToken();
      
      if (token.type == L_BRACKET)
      {
	loadToken();
	
	if (expresion_sequence(fTable,tProm,tLit) == true)
	{
	  if (token.type == SEMICOLON)
	  {
	    loadToken();
	    result = true;
	  }
	  else
	  {
            err_flg = ERR_SYN;
	    fprintf(stderr, "%d: Got ", token.line);
            typeName(token.type);
            fprintf(stderr, ", expected SEMICOLON\n");	    
	  }
	}
	 
      }
      else
      {
        err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
        typeName(token.type);
        fprintf(stderr, ", expected L_BRACKET\n");	
      }
      
      
      
      break;
      
    case IF:
      /*
       *  vytvori sled instrukcii
       * 
       * 
       * expresion
       * ifnotgoto label1
       * --sekvencia prikazov
       * goto label2
       * label1
       * --sekvencia prikazov else
       * label2
       * */
      
      label1 = LabelIndex();  // inicializacia navesti
      label2 = LabelIndex();
      
      
      
      loadToken();
      
      if (expres_parser(tProm,tLit, &(instrExpr),&index) == true)
      {
	
      	 // vygenerovanie instrucie ifnotgoto
        instruction *instr = makeinstr(IFNOTGOTO,index,0,label1);

	
	if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
	{
	  instrExpr = instr;
	}
	else
	{
	  // spojenie istrukcii vyrazoveho parsera a insrukcie return
	  FT_concatinstr(instrExpr,instr);
	}
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instrExpr;
        }
        else
        {
          FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
        }	
        
        
	
	if (token.type == THEN)
	{
	  loadToken();
	  
	  if (alt_statment_sequence(fTable,tProm,tLit) == true)
	  {
	    
	    // vygenerovanie instrukcii "goto label2" a label1
	    
	    instruction *instr2 = makeinstr(GOTO,0,0,label2);
            FT_concatinstr(fTable->function[fTable->count]->instr,instr2);
	    
	    instruction *instr3 = makeinstr(LABEL,label1,0,0);
            FT_concatinstr(fTable->function[fTable->count]->instr,instr3);	    
	    
	    
            if (statment_sequence(fTable,tProm,tLit) == true)
	    {
	      // vygnenerovanie instrukcie label2
	      
	      instruction *instr4 = makeinstr(LABEL,label2,0,0);
              FT_concatinstr(fTable->function[fTable->count]->instr,instr4);	
	      
	      if (token.type == SEMICOLON)
	      {
		
		loadToken();
		result = true;
	      }
	      else
	      {
	        err_flg = ERR_SYN;
		fprintf(stderr, "%d: Got ", token.line);
                typeName(token.type);
                fprintf(stderr, ", expected THEN\n");
	      }
	    }
	  }
	  
	}
	else
	{
	  err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
          typeName(token.type);
          fprintf(stderr, ", expected THEN\n");	
	}
      }
      
      break;
      
    case WHILE:
      
      /*
       *  vytvori sled instrukcii
       * 
       * label1
       * expresion
       * ifnotgoto label 2
       * --zoznam prikazov
       * goto label1
       * label2
       * */  
    
      label1 = LabelIndex();  // inicializacia navesti
      label2 = LabelIndex();

      // vygenerovanie instrukcie label1      
      
      instruction *instr2 = makeinstr(LABEL,label1,0,0);
      
      if (fTable->function[fTable->count]->instr == NULL) 
      {
        fTable->function[fTable->count]->instr = instr2;
      }
      else
      {
        FT_concatinstr(fTable->function[fTable->count]->instr,instr2);
      }		
      
      loadToken();
      
      if (expres_parser(tProm,tLit, &(instrExpr),&index) == true) 
      {
	
      	 // vygenerovanie instrucie ifnotgoto
        instruction *instr = makeinstr(IFNOTGOTO,index,0,label2);

	
	if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
	{
	  instrExpr = instr;
	}
	else
	{
	  // spojenie istrukcii vyrazoveho parsera a insrukcie return
	  FT_concatinstr(instrExpr,instr);
	}
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instrExpr;
        }
        else
        {
          FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
        }		
	
	
	if (token.type == DO)
	{
	  loadToken();
	  if (statment_sequence(fTable,tProm,tLit) == true)
	  {
	      
	    if (token.type == SEMICOLON)
	    {
	      
	        // vygenerovanie instrukcii "goto label1" a label2
	    
	      instruction *instr3 = makeinstr(GOTO,0,0,label1);
              FT_concatinstr(fTable->function[fTable->count]->instr,instr3);
	    
	      instruction *instr4 = makeinstr(LABEL,label2,0,0);
              FT_concatinstr(fTable->function[fTable->count]->instr,instr4);		      
	      
	      
	      loadToken();
	      result = true;
	    }
	    else
	    {
	      err_flg = ERR_SYN;
	      fprintf(stderr, "%d: Got ", token.line);
              typeName(token.type);
              fprintf(stderr, ", expected SEMICOLON\n");
	    }
	
	  }
	}
	else
	{
	  err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
          typeName(token.type);
          fprintf(stderr, ", expected DO\n");
	}
      }
      
      break;
      
    case RETURN:
      
      loadToken();
      
      if (expres_parser(tProm,tLit, &(instrExpr),&index) == true) 
      {
	
      	 // vygenerovanie instrucie return
        instruction *instr = makeinstr(RETURN,0,0,index);

	
	if (instrExpr == NULL) // ak expres_parser negeneruje instrukcie
	{
	  instrExpr = instr;
	}
	else
	{
	  // spojenie istrukcii vyrazoveho parsera a insrukcie return
	  FT_concatinstr(instrExpr,instr);
	}
	
        if (fTable->function[fTable->count]->instr == NULL) 
        {
          fTable->function[fTable->count]->instr = instrExpr;
        }
        else
        {
          FT_concatinstr(fTable->function[fTable->count]->instr,instrExpr);
        }	
	
	
	if (token.type == SEMICOLON)
	{
  
	  loadToken();
	  result = true;
	}
	else
	{
	  err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
          typeName(token.type);
          fprintf(stderr, ", expected SEMICOLON\n");
	}
      }
	
      break;
      
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected STATMENT\n");
      break;
  
  }
  
  return result;  
}


/*******************************************************************
 * Funkcia alt_statment_sequence simuluje pravidla:
 * alt-statment-sequence -> statement  alt_statment-sequence
 * statment-sequence ->  else 
**/

bool alt_statment_sequence(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  
  switch (token.type)
  {
    case ELSE:
      
      loadToken();
      result = true;
      
      break;
      
    case IF:
    case RETURN:
    case WHILE:  
    case WRITE:  
    case ID:
      
      // vynimka
      result = statment(fTable,tProm,tLit) && alt_statment_sequence(fTable,tProm,tLit);
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected ELSE or STATMENT\n");
      break;
  
  }
  
  return result;  
}
   



/*******************************************************************
 * Funkcia statment_sequence simuluje pravidla:
 * statment-sequence -> statement  statment-sequence
 * statment-sequence ->  end 
**/

bool statment_sequence(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  
  switch (token.type)
  {
    case END:
      
      loadToken();
      result = true;
      
      break;
      
    case IF:
    case RETURN:
    case WHILE:  
    case WRITE:  
    case ID:
      
      // vynimka
      result = statment(fTable,tProm,tLit) && statment_sequence(fTable,tProm,tLit);
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected END or STATMENT\n");
      break;
  
  }
  
  return result;  
}


/*******************************************************************
 * Funkcia ending simuluje pravidla:
 * ending -> ;
 * ending ->  = literal  ;
 * 
 * parameter where urcuje premmennu do ktorej sa literal ulozi
**/

bool ending(FTable *fTable, t_table *tLit, int where)
{
  bool result = false;
  int indexL;
  int special = SPEC_NONE;
  
  switch (token.type)
  {
    case SEMICOLON:
      
      loadToken();
      
      result = true;
      
      break;
      
    case ASSIGN:
      
      loadToken();
      
      if (literal(&indexL,tLit,&special) == true)
      {

	// priradenie literalu, vygenerujem instrukciu
	
        instruction *instr = makeinstr(COPY,indexL,0,where);
        FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	
	if (token.type == SEMICOLON)
	{
	  loadToken();
          result = true;
	}
	else
	{
	  err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
	  typeName(token.type);
          fprintf(stderr, ", expected SEMICOLON\n");
	}
      }

      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected SEMICOLON or ASSIGN\n");
      break;
  
  }
  
  return result;
}




/*******************************************************************
 * Funkcia variable_declaration simuluje pravidlo:
 * variable-declaration -> local id ending
 * 
**/

bool variable_declaration(FTable *fTable, t_table *tProm, t_table *tLit)
{
  
  bool result = false;
  int indexP; // index premmennej
    int functionType = FUNC_OTHER;  // typ funkcie
  
  switch (token.type)
  {
    case LOCAL:
      
      loadToken();
      
      if (token.type == ID)
      {
	
        if (strcmp(token.attr,"type") == 0)
        {
	  functionType = FUNC_TYPE;
        }
        else if (strcmp(token.attr,"substr") == 0)
        {
	  functionType = FUNC_SUBSTR;
        }
        else if (strcmp(token.attr,"find") == 0)
        {
          functionType = FUNC_FIND;
        }
        else if (strcmp(token.attr,"sort") == 0)
        {
          functionType = FUNC_SORT;
        }	

        if ((najdi_symbol(tProm, token.attr) == NULL) && 
	  (FT_findfunc(fTable,token.attr) == -1) && functionType == FUNC_OTHER) // nebola definovana premenna ani funkcia
        {                                                                                     // s rovnakym nazvom
	  t_value hodnota;
          hodnota.str= NULL;
	  indexP =  pridej_promennou(tProm, token.attr,hodnota,token.type);
	  
  
	  // vygenerovanie instrucii
	  instruction *instr = makeinstr(LOCAL,0,0,indexP);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	  }
	  
	  loadToken();
	  result = ending(fTable,tLit,indexP);
	  
        }
        else
        {
           err_flg = ERR_SEM;
           fprintf(stderr, "%d: Variable %s was previously defined\n",token.line,token.attr);
	   result = false;
	   break;
        }
	
	

      }
      else
      {
	err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
	typeName(token.type);
        fprintf(stderr, ", expected ID\n");
      }
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected LOCAL\n");
      break;
  
  }
  
  return result;
}




/*******************************************************************
 * Funkcia variable_declaration_sequence simuluje pravidla:
 * variable-declaration-sequence ->  variable-declaration variable-declaration-sequence
 * variable-declaration-sequence ->  epsilon
**/

bool variable_declaration_sequence(FTable *fTable, t_table *tProm, t_table *tLit) 
{
  
  bool result = false;
  
  switch (token.type)
  {
    case END:
    case IF:
    case RETURN:
    case WHILE:
    case WRITE:
    case ID:
      
      // vynimka, nenacitava sa novy token kvoli epsilon pravidlu
      result = true;
      
      break;
      
    case LOCAL:
      
      // vynimka, s LOCAL pracuje aj dalsia funkcia
      
      result = variable_declaration(fTable,tProm,tLit) && variable_declaration_sequence(fTable,tProm,tLit); 
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line); 
      typeName(token.type);
      fprintf(stderr, ", expected END,IF,RETURN,WHILE,ID or LOCAL\n");
      break;
  
  }
  
  return result;
}


/*******************************************************************
 * Funkcia function_definition simuluje pravidlo:
 * function-definition -> function id ( parameters variable-declaration-sequence statment-sequence
 * 
**/
bool function_definition(FTable *fTable, t_table *tProm, t_table *tLit)
{
  bool result = false;
  
  switch (token.type)
  {
    case FUNCTION:
      
      loadToken();
      
      if (token.type == ID)
      {
	// kontrola ci sa programator nepokusa prepisat zabudovanu funkciu
	if ( (strcmp("type",token.attr) == 0) || (strcmp("substr",token.attr) == 0) ||
	(strcmp("find",token.attr) == 0) || (strcmp("sort",token.attr) == 0))
	{
	  err_flg = ERR_SEM;
	  fprintf(stderr, "%d: Attempting to redefine built-in function\n",token.line);
	  result = false;
	  break; 
	}
	// kontrola ci este fukcia nie je definovana
	else if (FT_findfunc(fTable,token.attr) == -1) // funkcia sa nenasla
	{
	  FT_addfunc(fTable,token.attr); // tak ju pridaj
	  pridej_zarazky(tProm);
	}
	else
	{
	  err_flg = ERR_SEM;
	  fprintf(stderr, "Function %s was already defined\n",token.attr);
	  result = false;
	  break;
	}
	
	
	loadToken();
	
	if (token.type == L_BRACKET)
	{
	  loadToken();
	  
	  result = parameters(fTable,tProm,tLit) && variable_declaration_sequence(fTable,tProm,tLit) && statment_sequence(fTable,tProm,tLit);
	  
	  // vygenerovanie instrukcie return NIL
	    	
	  instruction *instr = makeinstr(RETURN,0,0,0);

	  if (fTable->function[fTable->count]->instr == NULL) 
	  {
	    fTable->function[fTable->count]->instr = instr;
	  }
	  else
	  {
            FT_concatinstr(fTable->function[fTable->count]->instr,instr);
	  }	   	  
	  
	  
	  
	}
	else
	{
	  err_flg = ERR_SYN;
	  fprintf(stderr, "%d: Got ", token.line);
	  typeName(token.type);
          fprintf(stderr, ", expected L_BRACKET\n");
	}
	
      }
      else
      {
	err_flg = ERR_SYN;
	fprintf(stderr, "%d: Got ", token.line);
	typeName(token.type);
        fprintf(stderr, ", expected ID\n");
      }
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected FUNCTION\n");
      break;
  
  }
  
  return result;
}



/*******************************************************************
 * Funkcia function_definition_sequence simuluje pravidla:
 * function-definition-sequence ->  function-definition  function-definition-sequence
 * function-definition-sequence ->  ;   
 * 
**/
bool function_definition_sequence(FTable *fTable, t_table *tProm, t_table *tLit) 
{
  bool result = false;
  
  switch (token.type)
  {
    case FUNCTION:
      
      result = function_definition(fTable,tProm,tLit) && function_definition_sequence(fTable,tProm,tLit);
      
      break;
      
    case SEMICOLON:
      
      loadToken();
      result = true;
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected FUNCTION\n");
      break;
  
  }
  
  return result;
}



/*******************************************************************
 * Funkcia prog simuluje pravidlo:
 * prog -> function-definition  function-definition-sequence
 * 
**/
bool prog(FTable *fTable, t_table *tLit)
{
  bool result = false;
  
  t_table *tProm=htable_init(); //tabulka symbolu pro promenne
  
  loadToken();
  
  switch (token.type)
  {
    case FUNCTION:
      
      result = function_definition(fTable,tProm,tLit) && function_definition_sequence(fTable,tProm,tLit);
      
      // kontorla ci nahodou nieco este nenasleduje za mainom
      
      if (result == false)
      {
	break;
      }
      
      if (token.type != END_OF_FILE) 
      {
	result = false;
        err_flg = ERR_SYN;
        fprintf(stderr, "%d: Got ", token.line);
        typeName(token.type);
        fprintf(stderr, ", expected END_OF_FILE\n");
        break;	
      }
      
      if (result == true)  // syntakticka analyza prebehla bez problemov
      {
	// kontrola ci je main posledna
	if ( strcmp(fTable->function[fTable->count]->name,"main") != 0 )
	{
	  err_flg = ERR_SEM;
	  fprintf(stderr, "Function main shoud be the last defined function\n");
	  result = false;
	}
      }
      
      if (fTable->function[fTable->count]->instr->type != QUE_CLEAN)
      {
	err_flg = ERR_SEM;
	fprintf(stderr, "Function main shoud have no parameters\n");
	result = false;
      }
      
      break;
      
    default:
      err_flg = ERR_SYN;
      fprintf(stderr, "%d: Got ", token.line);
      typeName(token.type);
      fprintf(stderr, ", expected FUNCTION\n");
      break;
  
  }
  
  htable_UDMA_free(tProm); //uvolneni TS promennych
  
  return result;
}





//*****************************************************************************************************







/***** konec souboru****/