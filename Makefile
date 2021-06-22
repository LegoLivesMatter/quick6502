CC = tcc # change to preferred C compiler
AS = xa # change to preferred 6502 assembler

all : quick6502 sample

quick6502 : quick6502.o

sample : sample.s
	xa sample.s -o sample

clean :
	-rm *.o *.bin quick6502 sample

.PHONY = all clean
