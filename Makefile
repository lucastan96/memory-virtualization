dist/main.o: main.c
	gcc main.c -o dist/main

clean:
	rm -rf ./dist && mkdir dist