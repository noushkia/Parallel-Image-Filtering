CC=g++
STD=-std=c++11
CF=$(STD)


ImageFilters.out: readImg.o image.o
	$(CC) $(CF) -o ImageFilters.out readImg.o image.o

$(BUILD_DIR)/readImg.o: readImg.cpp image.hpp typedefs.hpp
	$(CC) $(CF) -c readImg.cpp -o readImg.o

$(BUILD_DIR)/image.o: image.cpp image.hpp typedefs.hpp
	$(CC) $(CF) -c image.cpp -o image.o

.PHONY: clean
clean:
	rm -rf *.o *.out