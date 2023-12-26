CC = g++
CFLAGS = --std=c++17 -Wall -Werror -pedantic
LIB = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lboost_unit_test_framework
DEPS = sokoban.hpp

PROGRAM = Sokoban
OBJECTS = Sokoban.o main.o
LIBRARY = Sokoban.a

.PHONY: all clean

all: $(PROGRAM) $(LIBRARY)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(LIBRARY): Sokoban.o
	ar rcs $@ $^

clean:
	rm -f *.o $(PROGRAM) $(LIBRARY)

lint:
	cpplint *.cpp *.hpp

