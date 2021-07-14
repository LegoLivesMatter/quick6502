CC = tcc # change to preferred C compiler
AS = xa # change to preferred 6502 assembler
VERSION = $(shell git log -n 1 --oneline | awk '{print $$1}' )

CFLAGS = -g -DENABLE_DEBUG
LDFLAGS = -lSDL2

all : quick6502 sample

quick6502 : quick6502.o framebuffer.o

sample : sample.s
	xa sample.s -o sample

clean :
	@-rm *.o *.bin quick6502 sample *.tar.gz 

stash :
	@echo "WARNING: Stashing uncommitted changes (if any)!"
	@git stash push >/dev/null

dist : clean stash
	tar -cf quick6502-${VERSION}.tar *
	gzip quick6502-${VERSION}.tar

.PHONY = all clean
