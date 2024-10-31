# Grupo C
# Alunos:
# Gustavo Sleman Lenz - 00290394
# Vinicius Daniel Spadotto - 00341554

./etapa2: ./src/*.c ./obj/*.c
	gcc ./src/*.c ./obj/*.c -o etapa3 -lfl

./obj/parser.tab.c: ./src/parser.y ./obj/
	bison -d ./src/parser.y
	mv ./parser.tab.c ./obj/parser.tab.c
	mv ./parser.tab.h ./obj/parser.tab.h

./obj/lex.yy.c: ./src/scanner.l ./obj/
	flex -o ./obj/lex.yy.c ./src/scanner.l

./obj/:
	mkdir -p obj

clean:
	rm -rf obj
	rm -f etapa2
