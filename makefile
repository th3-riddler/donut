COMPILER = g++

CFLAGS = -Wall -g -Wno-char-subscripts -O3

TARGET = reduxinator

SRCS = main.cpp chessboard.cpp move.cpp

OBJS = $(SRCS:.cpp=.o)

# Precompiled header
PCH = precompiled.hpp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMPILER) $(CFLAGS) -o $(TARGET) $(OBJS)
	rm -f $(OBJS)  # Rimuove i file oggetto dopo la creazione dell'eseguibile

%.o: %.cpp $(PCH_GCH)
	$(COMPILER) $(CFLAGS) -c $<

$(PCH_GCH): $(PCH)
	$(COMPILER) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(PCH_GCH)