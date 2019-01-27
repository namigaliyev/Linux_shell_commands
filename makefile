HEADERS = main.h
OBJECTS = main.o

default: main

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

main: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f program

run: main
	./main
