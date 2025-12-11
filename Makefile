all:
	gcc -o programaTrab *.c -lm

make zip:
	zip trabalho.zip *.c *.h Makefile

run: all
	./programaTrab