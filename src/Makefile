CPPFLAGS = -pg -O3 -Wall -Wextra -Werror

CORE_OBJS = chess_utils.o evaluation.o move_generation.o move.o piece.o player.o position.o search.o timer.o tt.o

all: main perft perft_divide uci bench

main: main.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft: perft.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft_divide: perft_divide.o ${CORE_OBJS} 
	g++ ${CPPFLAGS} $^ -o $@

uci: uci.o uci_utils.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

bench: ../tests/movegen_bench ../tests/search_bench

../tests/%: ../tests/%.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

../tests/%.o: ../tests/%.cpp
	g++ ${CPPFLAGS} -I.. -c $< -o $@

%.o: %.cpp castling_rights.h chess_utils.h evaluation.h move_generation.h move.h piece.h player.h position.h search.h timer.h tt.h uci_utils.h
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o main perft perft_divide uci
