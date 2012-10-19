optimizer: accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c
	gcc -O3 -o optimizer accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c -lm -lpthread
debug: accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c 
	gcc -g -O0 -o optimizer accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c -lm -lpthread
profile: accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c 
	gcc -pg -O3 -o optimizer accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c -lm -lpthread
profile-google: accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c 
	gcc -O3 -o optimizer accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c -lm -lpthread -lprofiler
clean: 
	rm optimizer accessories.o cjalgorithm.o fitness.o keyboard.o main.o tools.o values.o 