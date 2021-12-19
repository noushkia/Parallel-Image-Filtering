#include "threads.hpp"
#include <iostream>

unsigned char*** initialize_pixels_thread(int NUMBER_OF_THREADS, int cols)
{
    unsigned char*** pixels_thread;

    pixels_thread = new unsigned char**[NUMBER_OF_THREADS];
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pixels_thread[i] = new unsigned char*[cols];
        for (int j = 0; j < cols; j++) {
            pixels_thread[i][j] = new unsigned char[color_pallete];
        }
    }

    return pixels_thread;
}

void* getImg(void* row)
{
    struct Row curr_row = *(struct Row *) row; // How to assign void* as struct

    for (int j = curr_row.cols - 1; j >= 0; j--)
        for (int k = 0; k < 3; k++)
        {
            switch (k)
            {
            case 0:
                curr_row.pixels[j][RED] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                break;
            case 1:
                curr_row.pixels[j][GREEN] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                break;
            case 2:
                curr_row.pixels[j][BLUE] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                break;
            }
            curr_row.count++;
        }
}