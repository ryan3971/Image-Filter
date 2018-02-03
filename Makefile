CC = gcc
CFLAGS = -W -Wall -pedantic -g  -O2 -std=c99 -pg
LDFLAGS = -pg

OBJS = main_denoise.o process_image.o timing.o

denoise: $(OBJS)	
	$(CC) -o $@ $? $(LDFLAGS)


clean:
	rm -rf $(OBJS) denoise
	
