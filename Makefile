all: optimizer

optimizer: main.c cjalgorithm.c accessories.c algorithm.c fitness.c trifitness.c keyboard.c tools.c trikeys.c costs.c values.c
	gcc main.c cjalgorithm.c accessories.c algorithm.c fitness.c trifitness.c keyboard.c tools.c trikeys.c costs.c values.c -o optimizer