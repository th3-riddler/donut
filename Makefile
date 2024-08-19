COMPILER = g++

CFLAGS = -Wall -g -Wno-char-subscripts -O3

TARGET = reduxinator

SRCS = main.cpp Chessboard/chessboard.cpp Move/move.cpp

OBJS = main.o Chessboard/chessboard.o Move/move.o

# Precompiled header
PCH = Macros/precompiled.hpp
PCH_GCH = $(PCH).gch

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMPILER) $(CFLAGS) -o $(TARGET) $(OBJS)
	rm -f Chessboard/chessboard.o Move/move.o main.o

%.o: %.cpp $(PCH_GCH)
	$(COMPILER) $(CFLAGS) -c $< -o $@

$(PCH_GCH): $(PCH)
	$(COMPILER) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(PCH_GCH)