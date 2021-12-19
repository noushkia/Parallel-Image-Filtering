#include "threads.hpp"
#include <iostream>

using namespace std;

void initialize_pixels_threads(int NUMBER_OF_THREADS, int rows, int cols, std::vector<RGBS> &pixels_threads)
{
    for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        auto pixels_thread = new unsigned char**[rows];
        for (int i = 0; i < rows; i++) {
            pixels_thread[i] = new unsigned char*[cols];
            for (int j = 0; j < cols; j++) {
                pixels_thread[i][j] = new unsigned char[color_pallete];
            }
        }
        pixels_threads.push_back(pixels_thread);
    }

    // pthread_exit(NULL);
}

void* getImg(void* rows)
{
    struct Row curr_row = *(struct Row *) rows; // How to assign void* as struct
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

    // pthread_exit(NULL);
}
