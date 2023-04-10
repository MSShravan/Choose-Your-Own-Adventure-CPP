FLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++98
PROGS=cyoa-step1 cyoa-step2 cyoa-step3 cyoa-step4
OBJS=$(patsubst %,%.o,$(PROGS)) cyoa.o
all: $(PROGS)
cyoa-step%: cyoa-step%.o cyoa.o
	g++ -o $@ $^
%.o: %.cpp
	g++ $(FLAGS) -c $<

.PHONY: clean
clean:
	rm -f *~ $(PROGS) $(OBJS) *.gch

cyoa.o: cyoa.hpp
