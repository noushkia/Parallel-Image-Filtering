CC=g++
STD=-std=c++11
CF=$(STD)
LINK_FLAGS = -pthread

ImageFilters.out: readImg.o threads.o
	$(CC) $(CF) $(LINK_FLAGS) -o ImageFilters.out readImg.o threads.o

$(BUILD_DIR)/readImg.o: readImg.cpp threads.hpp typedefs.hpp
	$(CC) $(CF) $(LINK_FLAGS) -c readImg.cpp -o readImg.o

$(BUILD_DIR)/threads.o: threads.cpp threads.hpp typedefs.hpp
	$(CC) $(CF) -c threads.cpp -o threads.o

.PHONY: clean
clean:
	rm -rf *.o *.out