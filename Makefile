###############################################################################
#        VUTBR - FIT - IFJ - Interpret jazyka IFJ11
#
#   Autori:  xkvita01   Jakub Kvita
#            xcienc02   Ondrej Cienciala
#            xpecse00   Robert Pecserke
#            xkripp00   Martin Krippel
#            xpyszk02   Petr Pyzsko
#
#   Datum dokonceni:   11.12.2011
#
#   Podrobnejsi info o aktualnim rocniku projektu:
#            https://www.fit.vutbr.cz/study/courses/IFJ/public/project/
#
###############################################################################
#   Soubor:  Makefile
#   Popis:   Preklad prekladace. Na Merlinovi a Eve prekladat pomoci gmake!
#
###############################################################################

# Jmeno programu neni v tomto projektu dulezite
PROGRAM=ifj11

# DULEZITE:
# Sem vzdy napsat seznam vsech objektovych souboru, ktere se budou slinkovavat
OBJ= main.o scanner.o ial.o err_handler.o interpreter.o parser.o instruction_list.o express_parser.o udma.o

# Nazev souboru, do ktereho se generuji zavislosti
DEPFILE=.depend

# Prekladac
CC=gcc

# Prepinace pro prekladac
# Je zapnuta i zakladni optimalizace, protoze umoznuje najit vice chyb
CFLAGS=-std=c99 -Wall -pedantic -W -lm -O


$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lm -o $@



# Jine jmeno pro -include (kvuli kompatibilite)
# Vlozi soubor .depend, pokud existuje
# Soubor .depend obsahuje vygenerovane zavislosti
sinclude ${DEPFILE}


# Vytvoreni souboru se zavislostmi, pokud se zmeni .c nebo .h soubor
$(DEPFILE): *.c *.h
	gcc -MM *.c > $(DEPFILE)



.PHONY: clean pack debug lex expr ial udma func

# Vola se prikazem make clean
# Vymaze nepotrebne soubory
clean:
	rm -fv *.o ${PROGRAM} ${PROGRAM}.zip ${DEPFILE} test_ial


# Zabali archiv
pack:
#	tar cvzf $(PROGRAM).tar.gz *
	zip $(PROGRAM).zip *.c *.h Makefile -x test*


# Spusti znovu make se vsemi predchozimi prepinaci a debug prepinacem
# Pouzita definice makra na prikazove radce (maji prednost pred temi v makefilu)
# -B - vse preloz bez ohledu na casove zavislosti
debug:
	make -B "CFLAGS=-g3 ${CFLAGS}"   # Lze zapsat i kratsim zpusobem:
                                   # "CFLAGS += -g3"


# Testovaci preklad pro scanner
lex:
	make -B "OBJ= test_scanner.o scanner.o err_handler.o"

expr:
	make -B "OBJ= test_exprpars.o scanner.o parser.o err_handler.o ial.o instruction_list.o"

pars:
	make -B "OBJ= scanner.o ial.o instruction_list.o express_parser.o parser.o err_handler.o   pars_test.o"
	
func:
	make -B "OBJ= test_functable.o err_handler.o"

# test TS a IAL	
ial:		
	$(CC) $(CFLAGS) -DDEBUG test_ial.c -o test_ial

# Testovaci preklad pro UDMA
udma:
	make -B "OBJ= test_udma.o udma.o"

#testovanie pre interpret
inte:
  make -B "OBJ= scanner.o ial.o instruction_list.o express_parser.o parser.o err_handler.o interpret.o pars_test.o test_pars_interpret.o"
  
#################### END ###################