CC = tcc # change to preferred C compiler
AS = xa # change to preferred 6502 assembler
VERSION = prealpha

all : quick6502 sample

quick6502 : quick6502.o

sample : sample.s
	xa sample.s -o sample

clean :
	-rm *.o *.bin quick6502 sample

dist : clean
	tar czf quick6502-$(VERSION).tar.gz *

.PHONY = all clean
