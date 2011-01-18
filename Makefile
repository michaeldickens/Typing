all: optimizer

optimizer: main.o cjalgorithm.o accessories.o algorithm.o fitness.o trifitness.o keyboard.o tools.o trikeys.o costs.o values.o
	gcc main.o cjalgorithm.o accessories.o algorithm.o fitness.o trifitness.o keyboard.o tools.o trikeys.o costs.o values.o -o optimizer

main.o: main.c
	gcc -c main.c

cjalgorithm.o: cjalgorithm.c
	gcc -c cjalgorithm.c
	
accessories.o: accessories.c
	gcc -c accessories.c

algorithm.o: algorithm.c
	gcc -c algorithm.c

fitness.o: fitness.c
	gcc -c fitness.c

trifitness.o: trifitness.c
	gcc -c trifitness.c

keyboard.o: keyboard.c
	gcc -c keyboard.c

tools.o: tools.c
	gcc -c tools.c

trikeys.o: trikeys.c
	gcc -c trikeys.c

costs.o: costs.c
	gcc -c costs.c

values.o: values.c
	gcc -c values.c
