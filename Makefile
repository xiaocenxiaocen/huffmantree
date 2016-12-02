CC = icc -O3 -g -Wall -std=c99 -openmp
CXX = icpc -O3 -g -Wall -std=c++0x -Wno-deprecated -openmp

INCLUDES = -I.

CFLAGS = -DSTABLE -DDEBUG -DNO_BLAS -DHAVE_MKL

LIBRARY = -L.

LDFLAGS = -lm -lpthread

target = huff_encode

all: $(target)

huff_encode: huffman.o
	$(CXX) -o $@ $+ $(LIBRARY) $(LDFLAGS)

.SUFFIXES: .o .cpp .c

.cpp.o:
	$(CXX) -c $(CFLAGS) $(INCLUDES) $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

.PHONY: clean
clean:
	-rm *.o
	-rm huff_encode
