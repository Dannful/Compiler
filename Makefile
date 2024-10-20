# Grupo C
# Alunos:
# Gustavo Sleman Lenz - 00290394
# Vinicius Daniel Spadotto - 00341554

./etapa2: ./obj/parser.tab.o ./obj/lex.yy.o ./obj/main.o
	gcc -o etapa2 ./obj/lex.yy.o ./obj/parser.tab.o ./obj/main.o -lfl

./obj/main.o: ./src/main.c
	gcc -c ./src/main.c -o ./obj/main.o

./obj/parser.tab.o: ./obj/parser.tab.c
	gcc -c ./obj/parser.tab.c -o ./obj/parser.tab.o

./obj/lex.yy.o: ./obj/lex.yy.c
	gcc -c ./obj/lex.yy.c -o ./obj/lex.yy.o

./obj/parser.tab.c: ./src/parser.y ./obj/
	bison -d ./src/parser.y
	mv ./parser.tab.c ./obj/parser.tab.c
	mv ./parser.tab.h ./obj/parser.tab.h

./obj/lex.yy.c: ./src/scanner.l ./obj/
	flex -o ./obj/lex.yy.c ./src/scanner.l

./obj/:
	mkdir -p obj

run: ./etapa2
	./etapa2

clean:
	rm -rf obj
	rm -f etapa2
