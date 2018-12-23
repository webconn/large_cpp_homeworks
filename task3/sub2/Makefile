TARGET = main
OBJS = shop.o product.o main.o
HEADERS = product.h shop.h

TEST_TARGET = test
TEST_OBJS = shop.o product.o test.o

CXXFLAGS = -std=c++17 -pthread -g
LDFLAGS = -pthread

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c -o $@ $< $(CXXFLAGS)

clean:
	rm $(TARGET) $(OBJS) $(TEST_OBJS) $(TEST_TARGET) -rf

.PHONY: all clean
