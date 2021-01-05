CPPFLAGS += -I.
CFLAGS += -W -Wall -Werror
CFLAGS += -g -Os

all: rotary_encoder.o examples/simulation examples/skeleton

clean:
	rm -f *.o */*.o examples/simulation examples/skeleton

rotary_encoder.o: rotary_encoder.c rotary_encoder.h

examples/simulation: examples/simulation.o rotary_encoder.o
	$(CC) $(LDFLAGS) $^ -lncursesw -o $@

examples/skeleton: examples/skeleton.o rotary_encoder.o
	$(CC) $(LDFLAGS) $^ -o $@

examples/simulation.o: examples/simulation.c rotary_encoder.h
examples/skeleton.o: rotary_encoder.h

.PHONY: all clean
