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
*   Soubor:  scanner.c
*   Popis:   V tomoto souboru je implementovana cast interpretu jazyka IFJ -
*           lexikalni analyzator (scanner).
*
*******************************************************************************/

/*======= Hlavickove soubory =================================================*/
#include "scanner.h"      // Obsahuje taktez vsechny potrebne hlavickove soubory
/*===========================================================================*/



/*
 * Zakladni stavy konecneho automatu scanneru
 */ 
#define STATE_START           10        // Startovaci stav
#define STATE_ID              20
#define STATE_NUMBER          30
#define STATE_ZERO            31
#define STATE_STRING          40
#define STATE_COMMENT         50



/*
 * Konstanty pro alokaci pameti pro atribut
 */
#define FIRST_ATTR_MEM     8    // Velikost pameti pro prvni alokaci atributu
#define MUL_ATTR_MEM       2    // Dvojnasobne zvetseni pameti pri nedostatku



/*
 * Zdrojovy soubor
 */
static FILE *source_file;



/*
 * Pocitadlo radku
 */
static unsigned line_counter;



/*===========================================================================*/
/*
 *  Prototypy lokalnich funkci
 */
static void token_init(Ttoken *token);
static int next_char();
static void back_char(int c);
static void token_add_char(Ttoken *token, char c, int *count_char, int *count_mem);
static void finish_attribute(Ttoken *token, int *count_char);
static int check_key_reserved_words(Ttoken token);
/*===========================================================================*/





/*---------------------------------------------------------------------------
 * Nastaveni zdrojoveho souboru do globalni promenne
 * Nutne zavolat ve funkci, ktera pozdeji pouziva funkci get_token()
 * Inicializuje se pocitadlo radku
 */
void source_file_set(FILE *f)
{
  source_file = f;
  line_counter = 1;  // Radky se pocitaji od jednicky
}




/*---------------------------------------------------------------------------
 * Inicializace struktury tokenu
 */
static void token_init(Ttoken *token)
{
  token->type = NONE;
  token->line = 0;
  token->attr = NULL;
}




/*---------------------------------------------------------------------------
 * Nacte jeden znak ze souboru source_file, ktery vrati, a pocita radky
 * v globalni promenne line_counter
 */
static int next_char()
{
  char c = getc(source_file);
  if (c == '\n')
    line_counter++;
  return c;
}




/*---------------------------------------------------------------------------
 * Vrati jeden znak do souboru source_file a pocita radky
 * v globalni promenne line_counter
 */
static void back_char(int c)
{
  if (c == '\n')
    line_counter--;
  ungetc(c, source_file);
}




/*---------------------------------------------------------------------------
 * Prida znak do atributu tokenu, v pripade potreby alokuje dalsi pamet
 * @param count_char - pocet jiz ulozenych znaku
 * @param count_mem  - velikost alokovaneho mista
 *
 * Nejdrive alokuje pamet pro FIRST_ATTR_MEM znaku (8), pri nedostatku
 * zvetsuje MUL_ATTR_MEM krat (2x)
 *
 * V pripade selhani alokace nastavi globalni promennou priznaku chyby - err_flg
 */
static void token_add_char(Ttoken *token, char c, int *count_char, int *count_mem)
{
  if (*count_mem == 0) {  // Jeste nebyla alokovana pamet
    token->attr = (char *) UDMA_malloc(FIRST_ATTR_MEM * sizeof(char));
    *count_mem = FIRST_ATTR_MEM;
  }
  else   // Pamet jiz byla jednou alokovana
    if ((*count_char+1) > *count_mem) {// Pocet znaku je vetsi nez alokovana pamet
      *count_mem = *count_mem * MUL_ATTR_MEM;     // Nasobime velikost pameti
      token->attr = (char *) UDMA_realloc(token->attr, *count_mem * sizeof(char));
    }

  if (token->attr == NULL) {   // Alokace selhala
    err_flg = ERR_INT;         // Nastaveni vnitrni chyby
    *count_mem = 0;
    *count_char = 0;  
  }
  else {      // Vse je v poradku
    token->attr[*count_char] = c;
    (*count_char)++;
  }
}




/*---------------------------------------------------------------------------
 * Zkrati velikost pameti pro atribut na presne tolik, kolik atribut zabira,
 * a prida nulovy znak na konec
 * @param count_char - pocet jiz ulozenych znaku
 *
 * Pri selhani realokace nastavi globalni promennou priznaku chyby - err_flg
 */
static void finish_attribute(Ttoken *token, int *count_char)
{
  token->attr = (char *) UDMA_realloc(token->attr, *count_char * sizeof(char) + 1);

  if (token->attr == NULL) {   // Realokace selhala
    err_flg = ERR_INT;         // Nastaveni vnitrni chyby  
  }
  else   // Vse v poradku
    token->attr[*count_char] = '\0';
}




/*---------------------------------------------------------------------------
 * Uvolni pamet pro atribut tokenu
 * Token bude ve stejnem stavu, jako po inicializaci
 *
 * Neuvolňuje primo strukturu tokenu, protoze se predpoklada, ze ta je alokovana
 * staticky
 */
void token_UDMA_free(Ttoken *token)
{
  token->type = NONE;
  token->line = 0;
  UDMA_free(token->attr); 
}




/*---------------------------------------------------------------------------
 * Zkontroluje, zda identifikator neni klicove nebo rezervovane slovo
 * Vrati typ tokenu  
 */
static int check_key_reserved_words(Ttoken token)
{
  // Kontrola klicovych slov
  if      (strcmp("do",       token.attr) == 0)    return DO;
  else if (strcmp("else",     token.attr) == 0)    return ELSE;
  else if (strcmp("end",      token.attr) == 0)    return END;
  else if (strcmp("false",    token.attr) == 0)    return BOOL;  // Literal
  else if (strcmp("function", token.attr) == 0)    return FUNCTION;
  else if (strcmp("if",       token.attr) == 0)    return IF;
  else if (strcmp("local",    token.attr) == 0)    return LOCAL;
  else if (strcmp("nil",      token.attr) == 0)    return NIL;
  else if (strcmp("read",     token.attr) == 0)    return READ;
  else if (strcmp("return",   token.attr) == 0)    return RETURN;
  else if (strcmp("then",     token.attr) == 0)    return THEN;
  else if (strcmp("true",     token.attr) == 0)    return BOOL;  // Literal
  else if (strcmp("while",    token.attr) == 0)    return WHILE;
  else if (strcmp("write",    token.attr) == 0)    return WRITE;
  // Kontrola rezervovanych slov
  else if (strcmp("and",    token.attr)   == 0  ||
           strcmp("break",  token.attr)   == 0  ||
           strcmp("elseif", token.attr)   == 0  ||
           strcmp("for",    token.attr)   == 0  ||
           strcmp("in",     token.attr)   == 0  ||
           strcmp("not",    token.attr)   == 0  ||
           strcmp("or",     token.attr)   == 0  ||
           strcmp("repeat", token.attr)   == 0  ||
           strcmp("until",  token.attr)   == 0)
    return RESERVED;
  else 
    return ID;
}




/*---------------------------------------------------------------------------
 * SCANNER - LEXIKALNI ANALYZATOR
 * Prochazi zdrojovym kodem a vraci token
 */
Ttoken get_token()
{
  Ttoken token;
  token_init(&token);  // Nastaveni defaultnich hodnot tokenu
  int c;               // Nacitany znak
  int count_char = 0;  // Pocet znaku v atributu tokenu
  int count_mem = 0;   // Velikost alokovane pameti pro atribut
  int state = STATE_START;   // Pocatecni stav konecneho automatu
  int state_num = 'c'; // Stav nacitani cisla
                 //  - 'c' = cela cast
                 //  - 'd' = desetinna cast
                 //  - 'e' = exponencialni cast 

  while (1) {     
    c = next_char();

    switch (state) {
      // ----------------------------------------------------------------------
      case STATE_START: 
        if (isspace(c))  // Bily znak
          ;              // Nedelej nic, nacti dalsi znak

        else if (isalpha(c) || c == '_') {   // Identifikator
          token_add_char(&token, c, &count_char, &count_mem);
          state = STATE_ID;
        }

        else if (c >= '1' && c <= '9') {   // Cisla krom nuly
          token_add_char(&token, c, &count_char, &count_mem);
          state = STATE_NUMBER;        
        }

        else if (c == '0')    // Nula
          state = STATE_ZERO;        

        else if (c == '"')    // Retezcovy literal 
          state = STATE_STRING;        // Znak " se neuklada

        else if (c == '-')    // Radkovy a blokovy komentar nebo Minus
          if ((c = next_char()) != '-') {
            token.type = MINUS;
            back_char(c);
          }
          else 
            state = STATE_COMMENT;
        
        else if (c == '+')    // Plus
          token.type = PLUS;
        
        else if (c == '*')    // Krat
          token.type = MUL;
        
        else if (c == '/')    // Deleno
          token.type = DIV;
        
        else if (c == '^')    // Umocneni
          token.type = POW;
          
        else if (c == '%')    // Mod - zbytek po deleni
          token.type = MOD;
       
        else if (c == '~')    // Ruzne
          if ((c = next_char()) == '=')
            token.type = NE;
          else {
            back_char(c);
            err_flg = ERR_LEX;
          }

        else if (c == '<')    // Mensi, mensi rovno
          if ((c = next_char()) == '=')
            token.type = LE;
          else {
            back_char(c);
            token.type = LT;
          }
        
        else if (c == '>')    // Vetsi, vetsi rovno
          if ((c = next_char()) == '=')
            token.type = GE;
          else {
            back_char(c);
            token.type = GT;
          }
        
        else if (c == '=')    // Prirazeni, porovnani
          if ((c = next_char()) == '=')
            token.type = EQ;
          else {
            back_char(c);
            token.type = ASSIGN;
          }
                      
        else if (c == '(')   // Leva zavorka
          token.type = L_BRACKET;
        
        else if (c == ')')   // Prava zavorka
          token.type = R_BRACKET;
        
        else if (c == ';')   // Strednik
          token.type = SEMICOLON;
        
        else if (c == '.')   // Konkatenace
          if ((c = next_char()) == '.')
            token.type = CONC;
          else {
            back_char(c);
            err_flg = ERR_LEX;
          }

        else if (c == ',')   // Carka
          token.type = COMMA;

        else if (c == EOF)
          token.type = END_OF_FILE;

        else      // Chybny vstup
          err_flg = ERR_LEX;
        break;
      // ----------------------------------------------------------------------  
      case STATE_ID:            // Stav nacitani identifikatoru
        if (isalnum(c) || c == '_')
          token_add_char(&token, c, &count_char, &count_mem);
        else {
          back_char(c);
          token.type = ID;
        }
        break;
      // ----------------------------------------------------------------------  
      case STATE_NUMBER:        // Stav nacitani cisla
        if (isdigit(c)) {       // Je to cislo, tak ho pridej
          token_add_char(&token, c, &count_char, &count_mem);
        }
        else if (c == '.') {
          if (state_num == 'c') {    // Byli jsme ve stavu nacitani cele casti
            token_add_char(&token, c, &count_char, &count_mem);
            state_num = 'd';         // Budem ve stavu nacitani desetinne casti
            c = next_char();   // Zkontrolujeme, zda dalsi znak je cislo
            if (isdigit(c))
              token_add_char(&token, c, &count_char, &count_mem);
            else {
              back_char(c);
              token.type = NUMBER;              
              err_flg = ERR_LEX;
            }
          }
          else {    // Nacitali jsme desetinnou nebo exponencialni cast
            back_char(c);
            token.type = NUMBER;
          }
        }
        else if (c == 'e' || c == 'E') {
          if (state_num == 'e') {      // Jiz jsme byli v exponencialni casti
            back_char(c);
            token.type = NUMBER;
          }
          else {
            token_add_char(&token, c, &count_char, &count_mem);
            state_num = 'e';        // Budem nacitat exponencialni cast
            c = next_char();  // Zkontrolujeme, zda prijde +, - nebo cislo
            if (c == '+' || c == '-') {
              token_add_char(&token, c, &count_char, &count_mem);
              c = next_char();
              if (isdigit(c))
                token_add_char(&token, c, &count_char, &count_mem);
              else {                // Za + nebo - neprislo cislo
                back_char(c);
                token.type = NUMBER;              
                err_flg = ERR_LEX;
              }
            }
            else if (isdigit(c))
              token_add_char(&token, c, &count_char, &count_mem);
            else {              // Neprislo +, - ani cislo
              back_char(c);
              token.type = NUMBER;              
              err_flg = ERR_LEX;
            }
          }
        }
        else {  // Prislo cokoliv jineho
          back_char(c);
          token.type = NUMBER;
        }      
        break;
      // ----------------------------------------------------------------------  
      case STATE_ZERO:    // Stav preskakujici pocatecni nuly
        if (c == '0') 
          ;
        else if (isdigit(c)) {
          token_add_char(&token, c, &count_char, &count_mem);
          state = STATE_NUMBER;
        }
        else if (c == '.') {
          token_add_char(&token, '0', &count_char, &count_mem);
          token_add_char(&token, c, &count_char, &count_mem);
          state_num = 'd';
          state = STATE_NUMBER;
        }
        else if (c == 'e' || c == 'E') {
          token_add_char(&token, '0', &count_char, &count_mem);
          token_add_char(&token, c, &count_char, &count_mem);
          state_num = 'e';
          state = STATE_NUMBER;
        }
        else {
          back_char(c);
          token_add_char(&token, '0', &count_char, &count_mem);
          token.type = NUMBER;
        }
        break;
      // ----------------------------------------------------------------------  
      case STATE_STRING:     // Stav nacitani retezce
        if (c == EOF) {      // Narazili jsme na konec souboru
          token.type = STRING;
          err_flg = ERR_LEX;
        }
        else if (c == '\\') {     // Escape sekvence
          c = next_char();
          if (c == 'n')
            token_add_char(&token, '\n', &count_char, &count_mem);
          else if (c == 't')
            token_add_char(&token, '\t', &count_char, &count_mem);
          else if (c == '\\')
            token_add_char(&token, '\\', &count_char, &count_mem);
          else if (c == '"')
            token_add_char(&token, '\"', &count_char, &count_mem);
          else if (isdigit(c)) { // Ciselna escape sekvence - nahradi se za znak
            int result;
            result = c - '0';     // Ciselna hodnota prvniho ciselneho znaku
            c = next_char();
            if (isdigit(c)) {   // I druhy znak je cislo
              result = result * 10  +  c - '0';
              c = next_char();
              if (isdigit(c)) {   // I treti znak je cislo
                result = result * 10  +  c - '0';
                if (result <= 255)
                  token_add_char(&token, result, &count_char, &count_mem);
                else {
                  token.type = STRING;
                  err_flg = ERR_LEX;
                }
              }
              else {  // Treti znak neni cislo
                back_char(c);
                if (result <= 255)
                  token_add_char(&token, result, &count_char, &count_mem);
                else {
                  token.type = STRING;
                  err_flg = ERR_LEX;
                }
              }
            }
            else {   // Druhy znak neni cislo
              back_char(c);
              if (result <= 255)
                token_add_char(&token, result, &count_char, &count_mem);
              else {
                token.type = STRING;
                err_flg = ERR_LEX;
              }
            }
          } // ciselna escape sekvence
          else { // Libovolny znak za \ krom povolenych a cisla je chyba
            token.type = STRING;
            err_flg = ERR_LEX;
          }
        }
        else if (c == '"') {     // Konec stringu 
          token.type = STRING;
        }
        else                     // Znak je cokoliv krom EOF, \ a "
          token_add_char(&token, c, &count_char, &count_mem);
        break;
      // ----------------------------------------------------------------------
      case STATE_COMMENT:  // Stav preskoceni komentare (jiz byla nactena 2 --)
        if (c == '[') {
          if ((c = next_char()) == '[') {   // Blokovy komentar
            while (1) {    // Nacitani komentare, hlidani zavorek a EOFu
              c = next_char();
              if (c == EOF) {        // Narazili jsme na konec souboru, chyba
                token.type = END_OF_FILE;
                err_flg = ERR_LEX;
                break;
              }
              else if (c == ']') {         // Nacetli jsme 1. zavorku
                c = next_char();     // Predpokladam nacteni 2. zavorky
                if (c == ']') {            // Konec blokoveho komentare
                  state = STATE_START;
                  break;
                }
                else if (c == EOF) {       // Konec souboru, chyba
                  token.type = END_OF_FILE;
                  err_flg = ERR_LEX;
                  break;
                }            // Byl to normalni znak, cti dal
              }              // Taky normalni znak, cti dal
            }  // Konec while - nacitani blokoveho komentare
          }  // Konec if - blokovy komentar
          else if (c == EOF) {     // EOF namisto 2. zavorky neni chybou
            token.type = END_OF_FILE;
            break;
          }
          else if (c == '\n') {    // Misto 2. zavorky prislo odradkovni
            state = STATE_START;
          }
          else {   // Namisto 2.zavorky nejaky jiny znak -> radkovy komentar
            while (1) { // Cti az do konce radku nebo EOF
              c = next_char();
              if (c == EOF)         // Radkovy komentar nemusi byt uzavren
                token.type = END_OF_FILE;      // EOF je povolen
              else if (c == '\n')      // Dosli jsme na konec radku
                state = STATE_START;
              break;
            }
          }  
        } // Konec if - prvni zavorka
        else if (c == EOF) {     // EOF namisto 1. zavorky neni chybou
          token.type = END_OF_FILE;
          break;
        }
        else if (c == '\n') {    // Misto 1. zavorky prislo odradkovni
          state = STATE_START;
        }
        else {   // Namisto 1.zavorky nejaky jiny znak -> radkovy komentar
          while (1) { // Cti az do konce radku nebo EOF
            c = next_char();
            if (c == EOF)         // Radkovy komentar nemusi byt uzavren
              token.type = END_OF_FILE;      // EOF je povolen
            else if (c == '\n')      // Dosli jsme na konec radku
              state = STATE_START;
            break;
          }
        }
        break;      
    // ------------------------------------------------------------------------
    } // end swich
    
    if (token.type != NONE || err_flg != OK) {
      if (token.type >= ID && token.type <= STRING)
        finish_attribute(&token, &count_char);  // Zkraceni pameti pro atribut
      token.line = line_counter;  // Ulozeni radku, na kterem byl token nacten
      if (token.type == ID)   // Kontrola na klicova a rezervovana slova
        token.type = check_key_reserved_words(token);
      return token;
    }
  }  // end while
}



/***** konec souboru****/
