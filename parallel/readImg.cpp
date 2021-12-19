#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <iomanip> 
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>

#include "typedefs.hpp"
#include "threads.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std;
using namespace std::chrono;

#pragma pack(1)
#pragma once

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizepixels;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;
RGBS pixels;
RGBS real_pixels;
vector<RGBS> pixels_threads;

void blur(int lvl);
void sepia();
void mean();
void add_X(int width);

void initialize_pixels()
{
  real_pixels = new unsigned char**[rows];
  for (int i = 0; i < rows; i++) {
    real_pixels[i] = new unsigned char*[cols];
    for (int j = 0; j < cols; j++) {
      real_pixels[i][j] = new unsigned char[color_pallete];
    }
  }
}

void set_color(int row, int col, const int color[]) // Move to threads
{
  for (int i = 0; i < color_pallete; i++)
    pixels[row][col][i] = color[i];
}

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File " << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer) // Multi-threaded
{
  int count = 1;
  int extra = cols % 4;
  int NUMBER_OF_THREADS = rows / TH_ROW;

  pthread_t threads[NUMBER_OF_THREADS];

  vector<struct Row> thread_rows;

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    count += extra;

    thread_rows.push_back({
      count,
      end,
      cols,
      TH_ROW,
      fileReadBuffer,
      pixels_threads[tid]
    });

    count += TH_ROW*cols*color_pallete;
  }
  
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    int return_code = pthread_create(&threads[tid], NULL, getImg, &thread_rows[tid]);
		if (return_code)
		{
      cerr << "ERROR! return code from pthread_create() is " << return_code << endl;
      exit(-1);
		}

  }

  for (int tid = 0 ; tid < NUMBER_OF_THREADS ; tid++)
  {
    int return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      cerr << "ERROR! return code from pthread_join() is " << return_code << endl;
      exit(-1);
    }
  }
}

void writeOutBmp24(char *fileBuffer, const string &nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }

  int count = 1;
  int extra = cols % 4;
  for (int tid = 0; tid < rows/TH_ROW; tid++)
  {
    for (int i = 0; i < TH_ROW; i++)
    {
      count += extra;
      for (int j = cols - 1; j >= 0; j--)
        for (int k = 0; k < 3; k++)
        {
          switch (k)
          {
          case 0:
            fileBuffer[bufferSize - count] = pixels_threads[tid][i][j][RED];
            break;
          case 1:
            fileBuffer[bufferSize - count] = pixels_threads[tid][i][j][GREEN];
            break;
          case 2:
            fileBuffer[bufferSize - count] = pixels_threads[tid][i][j][BLUE];
            break;
          }
          count++;
        }
    }
  }
  write.write(fileBuffer, bufferSize);
}

void apply_filters()
{
  for (int i = 0; i < TOTAL_FILTERS; i++) {
    auto start = high_resolution_clock::now();
    switch (i) // The Main Hotspot
    {
    case BLUR:
      blur(1); // Most time taken
      break;
    case SEPIA:
      sepia();
      break;
    case MEAN:
      mean();
      break;
    case ADDX:
      add_X(2); // Least time taken
      break;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken: " << std::fixed
                           << std::setprecision(2)
                           << duration.count()/1000.0 << " ms" << endl;

    start = high_resolution_clock::now();

    // real_pixels = pixels_threads;

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken to write on real_pixel: " << std::fixed
                                                  << std::setprecision(2)
                                                  << duration.count()/1000.0 << " ms" << endl;
  }

  return;
}

void filter_parallel(char *fileBuffer, int bufferSize, char *fileName)
{
  int NUMBER_OF_THREADS = rows / TH_ROW;

  initialize_pixels_threads(NUMBER_OF_THREADS, TH_ROW, cols, pixels_threads);

  auto start = high_resolution_clock::now();

  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer); // Secondary Hotspot

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken for reading image: " << std::fixed
                                           << std::setprecision(2)
                                           << duration.count()/1000.0 << " ms" << endl;

  // real_pixels = pixels_threads;

  auto serial_start = high_resolution_clock::now();

  // apply_filters();

  writeOutBmp24(fileBuffer, "output.bmp", bufferSize);

  auto serial_end = high_resolution_clock::now();
  auto serial_duration = duration_cast<microseconds>(serial_end - serial_start);
  cout << "Total time taken for applying filters and writing images: " 
                                              << std::fixed
                                              << std::setprecision(2)
                                              << (serial_duration.count()/1000.0) << " ms" << endl;

  return;
}

int main(int argc, char *argv[])
{
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  initialize_pixels();

  filter_parallel(fileBuffer, bufferSize, fileName);
  
  return 0;
}

int blur_color(int i, int j, int color, int lvl)
{
	int blurred = 0;
	for (int k = i - lvl; k <= i + lvl && k > 0 && k < rows; k++)
		for (int l = j - lvl; l <= j + lvl && l > 0 && l < cols; l++)
			blurred += real_pixels[k][l][color];
	return blurred / pow(lvl*2+1, 2);
}

void blur(int lvl)
{
  cout << "Initiating blur filter" << endl;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        pixels[i][j][k] = blur_color(i, j, k, lvl);
  }

  cout << "Filter successfull" << endl;
    
  return;
}

void sepia()
{
    cout << "Initiating sepia filter" << endl;

    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
      {
        pixels[i][j][0] = min(((real_pixels[i][j][0]*0.393) + (real_pixels[i][j][1]*0.769) + (real_pixels[i][j][2]*0.189)), 255.00);
        pixels[i][j][1] = min(((real_pixels[i][j][0]*0.349) + (real_pixels[i][j][1]*0.686) + (real_pixels[i][j][2]*0.168)), 255.00);
        pixels[i][j][2] = min(((real_pixels[i][j][0]*0.272) + (real_pixels[i][j][1]*0.534) + (real_pixels[i][j][2]*0.131)), 255.00);
      }

    cout << "Filter successfull" << endl;
    return;
}

void get_total_avg(unsigned char avg[])
{
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        avg[k] += real_pixels[i][j][k];

  for (int i = 0; i < color_pallete; i++)
    avg[i] /= (rows*cols);

  return;
}

void mean()
{
    cout << "Initiating mean filter" << endl;

    unsigned char avg[color_pallete];
    for (int k = 0; k < color_pallete; k++)
      avg[k] = 0;
    get_total_avg(avg);

    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        for (int k = 0; k < color_pallete; k++)
          pixels[i][j][k] = min((real_pixels[i][j][k]*0.4 + avg[k]*0.6), 255.00);

    cout << "Filter successfull" << endl;
    return;
}

void add_X(int width)
{
  cout << "Initiating X filter" << endl;

	for (int i = width; i < rows-width; i++)
    for (int j = -width; j <= width; j++) {
      set_color(i+j, i+j, WHITE);
      set_color(i, i+j, WHITE);
      set_color(i+j, i, WHITE);
      set_color(i+j, cols - 1 - i-j, WHITE);
      set_color(i, cols - 1 - i-j, WHITE);
      set_color(i+j, cols - 1 - i, WHITE);
    }

    cout << "Filter successfull" << endl;
    return;
}
