package = starcrunch
version = 0.2
CC=g++
OBJS=starcrunch.o

all:
	$(CC) -o ./starcrunch ./starcrunch.cpp -I.


clean:
	rm $(OBJS)


install: 
	cp -f ./starcrunch ~/Documents/Games/starcrunch/bin
	cp -f ./starcrunch.png ~/Documents/Games/starcrunch/bin
	chown root:root ~/Documents/Games/starcrunch/bin/starcrunch
	chmod +x ~/Documents/Games/starcrunch/bin/starcrunch


.PHONY: all clean install

