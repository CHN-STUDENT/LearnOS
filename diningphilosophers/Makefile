# To run, enter
# make all

all: diningphilosophers

diningphilosophers: main.o dining.o eating.o thinking.o philosopher.o
	gcc -o diningphilosophers main.o dining.o thinking.o eating.o philosopher.o -lpthread

main.o: main.c dp.h
	gcc -c main.c -lpthread

dining.o: dining.c dp.h
	gcc -c dining.c -lpthread 

philosopher.o: philosopher.c dp.h
	gcc -c philosopher.c -lpthread

eating.o: eating.c
	gcc -c eating.c -lpthread

thinking.o: thinking.c
	gcc -c thinking.c -lpthread

