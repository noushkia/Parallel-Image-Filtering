#ifndef _TH_HPP_

#define _TH_HPP_

#include <vector>
#include "typedefs.hpp"

struct Row
{
    int count;
    int end;
    int cols;
    char *fileReadBuffer;
    unsigned char** pixels;
};

unsigned char*** initialize_pixels_thread(int NUMBER_OF_THREADS, int cols);

void* getImg(void* row);

#endif