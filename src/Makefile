CPPFLAGS = -pg -O3 -Wall -Wextra -Werror

CORE_OBJS = evaluation.o move_generation.o move.o piece.o position.o search.o timer.o tt.o uci.o utils.o 

all: perft perft_divide mozzarella bench

perft: perft.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

perft_divide: perft_divide.o ${CORE_OBJS} 
	g++ ${CPPFLAGS} $^ -o $@

mozzarella: mozzarella.o utils.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

bench: ../tests/movegen_bench ../tests/search_bench

../tests/%: ../tests/%.o ${CORE_OBJS}
	g++ ${CPPFLAGS} $^ -o $@

../tests/%.o: ../tests/%.cpp
	g++ ${CPPFLAGS} -I.. -c $< -o $@

%.o: %.cpp castling_rights.h evaluation.h move_generation.h move.h piece.h player.h position.h search.h timer.h tt.h uci.h utils.h 
	g++ ${CPPFLAGS} -c $<

clean:
	rm *.o perft perft_divide mozzarella ../tests/movegen_bench ../tests/search_bench
