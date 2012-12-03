optimizer: accessories.c algorithm.c cjalgorithm.c costs.c fitness.c keyboard.c main.c tools.c trifitness.c trikeys.c values.c keystrokeValues.c
	gcc -o optimizer accessories.c algorithm.c cjalgorithm.c costs.c fitness.c keyboard.c main.c tools.c trifitness.c trikeys.c values.c keystrokeValues.c -lm
debug: accessories.c algorithm.c cjalgorithm.c costs.c fitness.c keyboard.c main.c tools.c trifitness.c trikeys.c values.c keystrokeValues.c
	gcc -o optimizer accessories.c algorithm.c cjalgorithm.c costs.c fitness.c keyboard.c main.c tools.c trifitness.c trikeys.c values.c keystrokeValues.c -g -lm
clean:
	rm optimizer accessories.o algorithm.o cjalgorithm.o costs.o fitness.o keyboard.o main.o tools.o trifitness.o trikeys.o values.o keystrokeValues.o
