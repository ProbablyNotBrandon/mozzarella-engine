CPPFLAGS = -g -Wall -Wextra -Werror 

all: main perft perft_divide

main: main.o castling_rights.o chess_utils.o evaluation.o move_generation.o move_pick.o move.o piece.o player.o position.o search.o
	g++ ${CPPFLAGS} $^ -o $@

perft: perft.o castling_rights.o chess_utils.o evaluation.o move_generation.o move_pick.o move.o piece.o player.o position.o search.o 
	g++ ${CPPFLAGS} $^ -o $@

perft_divide: perft_divide.o castling_rights.o chess_utils.o evaluation.o move_generation.o move_pick.o move.o piece.o player.o position.o search.o 
	g++ ${CPPFLAGS} $^ -o $@

%.o: %.cpp castling_rights.h chess_utils.h evaluation.h move_generation.h move_pick.h move.h piece.h player.h position.h search.h 
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o main perft perft_divide
