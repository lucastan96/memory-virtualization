default: link

link: dist/main.o dist/util.o
	gcc dist/main.o dist/util.o -o dist/main

dist/main.o: main.c
	gcc -c -Wall main.c -o dist/main.o

dist/util.o: lib/util.c
	gcc -c -Wall lib/util.c -o dist/util.o

clean:
	rm -rf ./dist && mkdir dist