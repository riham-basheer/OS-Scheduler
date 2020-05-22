build:
	gcc -g process_generator.c processQueue.c -o process_generator.out
	gcc -g clk.c -o clk.out
	gcc -g scheduler.c -o scheduler.out
	gcc -g process.c -o process.out
	gcc -g test_generator.c -o test_generator.out

clean:
	rm -f *.out *.o

all: clean build

run:
	./process_generator.out
