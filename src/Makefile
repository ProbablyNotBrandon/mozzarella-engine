CPPFLAGS = -g -Wall -Wextra -Werror 

all: main

main: main.o position.o move_generation.o castling_rights.o move.o piece.o chess_utils.o
	g++ ${CPPFLAGS} $^ -o $@

%.o: %.cpp castling_rights.h chess_utils.h move_generation.h move.h piece.h player.h position.h
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o main