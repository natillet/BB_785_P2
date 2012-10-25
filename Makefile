PROJ_NAME = project2
CC = gcc #arm-linux-gnueabi-gcc
VECTFLAGS = -ftree-vectorize -ffast-math -mvectorize-with-neon-quad -fsingle-precision-constant
CFLAGS = -Wall  -O3 -march=armv7-a -mcpu=cortex-a8  -mfloat-abi=softfp -mfpu=neon $(VECTFLAGS) #-funroll-loops
LIBS = -lm -lrt
OBJFILES := $(patsubst %.c,%.o,$(wildcard *.c))
$(PROJ_NAME): $(OBJFILES) 
#	echo $(OBJFILES)
	$(CC) -o $(PROJ_NAME) $(OBJFILES) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.lst: %.c
	$(CC) $(CFLAGS) -Wa,-adhln $(LIBS) $< > $@
clean:
	rm -f *.o *.lst
