compile: ./src/main.c ./include/scanner.l
	flex ./include/scanner.l
	mkdir -p build
	mv lex.yy.c ./build/lex.yy.c
	gcc -lfl ./src/*.c ./build/lex.yy.c -o ./etapa1

run: ./etapa1
	./etapa1

test: ./src/testing/*.c ./include/testing/*.h
	mkdir -p build
	gcc ./src/testing/*.c $(ls ./src/*.c | grep -v main.c) -o ./build/tests
	./build/tests

clean:
	rm -rf build
	rm -f etapa1
