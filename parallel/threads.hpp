#ifndef _TH_HPP_
#define _TH_HPP_

#include <vector>
#include <pthread.h>
#include "typedefs.hpp"

struct Row
{
    int count;
    int end;
    int cols;
    int rows;
    char *fileReadBuffer;
    RGBS pixels;
};

void initialize_pixels_threads(int NUMBER_OF_THREADS, int rows, int cols, std::vector<RGBS> &pixels_threads);

void* getImg(void* rows);

#endif