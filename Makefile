COMPILER = g++

CFLAGS = -Wall -g -Wno-char-subscripts -Wno-write-strings -Ofast -fomit-frame-pointer -DUSE_SSE41 -msse4.1 -DUSE_SSSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse

TARGET = engineVersions/iris

SRCS = main.cpp Chessboard/chessboard.cpp Move/move.cpp Search/search.cpp Evaluation/evaluation.cpp nnueEval/nnueEval.cpp nnue/nnue.cpp nnue/misc.cpp

OBJS = main.o Chessboard/chessboard.o Move/move.o Search/search.o Evaluation/evaluation.o nnueEval/nnueEval.o nnue/nnue.o nnue/misc.o

# Precompiled header
PCH = Macros/precompiled.hpp
PCH_GCH = $(PCH).gch

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMPILER) $(CFLAGS) -o $(TARGET) $(OBJS)
	rm -f Chessboard/chessboard.o Move/move.o main.o Search/search.o Evaluation/evaluation.o nnueEval/nnueEval.o nnue/nnue.o nnue/misc.o

%.o: %.cpp $(PCH_GCH)
	$(COMPILER) $(CFLAGS) -c $< -o $@

$(PCH_GCH): $(PCH)
	$(COMPILER) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(PCH_GCH)