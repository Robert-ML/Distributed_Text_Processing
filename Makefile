# Copyright 2020 Lica Robert-Mihai <robertlica21@gmail.com>
CC=mpic++ -std=c++11
CFLAGS=-Wall -Wextra -lpthread #-O2
#CDEFINES=-D PROC_COUNT=$(grep -c ^processor /proc/cpuinfo)
CFILES=main.cpp ./master/master.cpp ./genres/genre.cpp ./genres/comedy/comedy.cpp ./genres/fantasy/fantasy.cpp ./genres/horror/horror.cpp ./genres/science_fiction/science_fiction.cpp

EXEC=mpirun
EXECFLAGS=-np –oversubscribe

# NO_INSTANCES=5
NO_INSTANCES=5
PROGRAM_N=main

build:
	$(CC) $(CFLAGS) $(CFILES) -o $(PROGRAM_N)

run:
	$(EXEC) $(EXECFLAGS) $(NO_INSTANCES) ./$(PROGRAM_N) $(input)

clean:
	rm $(PROGRAM_N)

