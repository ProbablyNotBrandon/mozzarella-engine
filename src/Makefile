CPPFLAGS = -g -Wall -Wextra -Werror 

CORE_OBJS = castling_rights.o chess_utils.o evaluation.o move_generation.o move_pick.o move.o piece.o player.o position.o search.o

all: main perft perft_divide uci

main: main.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft: perft.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft_divide: perft_divide.o ${CORE_OBJS} 
	g++ ${CPPFLAGS} $^ -o $@

uci: uci.o uci_utils.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

%.o: %.cpp castling_rights.h chess_utils.h evaluation.h move_generation.h move_pick.h move.h piece.h player.h position.h search.h uci_utils.h
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o main perft perft_divide uci
