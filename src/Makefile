CPPFLAGS = -pg -Wall -Wextra -Werror 

CORE_OBJS = chess_utils.o evaluation.o move_generation.o move.o piece.o player.o position.o search.o tt.o

all: main perft perft_divide uci

main: main.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft: perft.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft_divide: perft_divide.o ${CORE_OBJS} 
	g++ ${CPPFLAGS} $^ -o $@

uci: uci.o uci_utils.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

%.o: %.cpp castling_rights.h chess_utils.h evaluation.h move_generation.h move.h piece.h player.h position.h search.h tt.h uci_utils.h
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o main perft perft_divide uci
