#include "threads.hpp"
#include <iostream>
void initialize_pixels_threads(int NUMBER_OF_THREADS, int rows, int cols, RGBS* &pixels_threads)
{
    pixels_threads = new RGBS[NUMBER_OF_THREADS];
    for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        pixels_threads[tid] = new unsigned char**[rows];
        for (int i = 0; i < rows; i++) {
            pixels_threads[tid][i] = new unsigned char*[cols];
            for (int j = 0; j < cols; j++) {
                pixels_threads[tid][i][j] = new unsigned char[color_pallete];
            }
        }
    }
}

void* getImg(void* rows)
{
    struct Row curr_row = *(struct Row *) rows;
    for (int i = 0; i < TH_ROW; i++)
    {
        for (int j = curr_row.cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
                case RED:
                    curr_row.pixels[i][j][RED] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                    break;
                case GREEN:
                    curr_row.pixels[i][j][GREEN] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                    break;
                case BLUE:
                    curr_row.pixels[i][j][BLUE] = curr_row.fileReadBuffer[curr_row.end - curr_row.count];
                    break;
                }
                curr_row.count++;
            }
    }
}
