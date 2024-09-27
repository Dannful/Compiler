compile: ./src/main.c ./include/scanner.l
	flex ./include/scanner.l
	mv lex.yy.c ./include/lex.yy.c
	gcc -lfl ./src/*.c ./include/lex.yy.c -o compiler

run: ./compiler
	./compiler

test: ./src/testing/*.c ./include/testing/*.h
	gcc ./src/testing/*.c $(ls ./src/*.c | grep -v main.c) -o tests
	./tests

clean:
	rm -f ./include/lex.yy.c
	rm -f ./compiler
	rm -f ./tests
