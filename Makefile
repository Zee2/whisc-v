CC=gcc
CFLAGS+=-Wall -fno-stack-protector

DEPS=simulator/simulator.h simulator/opcodes.h simulator/decode.h

decode.o: simulator/decode.c $(DEPS)
	$(CC) -c simulator/decode.c $(DEPS) $(CFLAGS)

main.o: main.c decode.o $(DEPS)
	$(CC) -c $(DEPS) main.c $(CFLAGS)

whiscv: main.o decode.o
	$(CC) -o whiscv main.o decode.o $(CFLAGS)

clean:
	rm -rf *.o simulator/*.o