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
unsigned char avg[color_pallete];

RGBS real_pixels;
vector<RGBS> pixels_threads;

void* blur(void *tid);
void* sepia(void *tid);
void set_total_avg();
void* mean(void *tid);
void add_X();

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
    real_pixels[row][col][i] = color[i];
}

void update_real_pixels()
{
  for (int tid = 0; tid < rows/TH_ROW; tid++)
  {
    for (int i = 0; i < TH_ROW; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        for (int k = 0; k < color_pallete; k++)
          real_pixels[tid*TH_ROW + i][j][k] = pixels_threads[tid][i][j][k];
      }
    }
  }
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
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = real_pixels[i][j][RED];
          break;
        case 1:
          fileBuffer[bufferSize - count] = real_pixels[i][j][GREEN];
          break;
        case 2:
          fileBuffer[bufferSize - count] = real_pixels[i][j][BLUE];
          break;
        }
        count++;
      }
  }
  
  write.write(fileBuffer, bufferSize);
}

void apply_to_threads(void *(*filter) (void *))
{
  int NUMBER_OF_THREADS = rows / TH_ROW;
  pthread_t threads[NUMBER_OF_THREADS];

  for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    int return_code = pthread_create(&threads[tid], NULL, filter, (void*)tid);
		if (return_code)
		{
      cerr << "ERROR! return code from pthread_create() is " << return_code << endl;
      exit(-1);
		}

  }

  for (long tid = 0 ; tid < NUMBER_OF_THREADS ; tid++)
  {
    int return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      cerr << "ERROR! return code from pthread_join() is " << return_code << endl;
      exit(-1);
    }
  }
}

void apply_filters()
{
  for (int i = 0; i < TOTAL_FILTERS; i++) {
    bool update_real = true;
    auto start = high_resolution_clock::now();
    switch (i) // The Main Hotspot
    {
    case BLUR:
      cout << "Applying blur filter\n";
      apply_to_threads(blur);
      break;
    case SEPIA:
      cout << "Applying sepia filter\n";
      apply_to_threads(sepia);
      break;
    case MEAN:
      cout << "Applying mean filter\n";
      set_total_avg();
      apply_to_threads(mean);
      break;
    case ADDX:
      cout << "Applying X filter\n";
      add_X(); // Least time taken
      update_real = false;
      break;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken: " << std::fixed
                           << std::setprecision(2)
                           << duration.count()/1000.0 << " ms" << endl;

    start = high_resolution_clock::now();

    if (update_real)
      update_real_pixels();

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

  initialize_pixels_threads(NUMBER_OF_THREADS, rows, cols, pixels_threads);

  auto start = high_resolution_clock::now();

  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer); // Secondary Hotspot

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken for reading image: " << std::fixed
                                           << std::setprecision(2)
                                           << duration.count()/1000.0 << " ms" << endl;

  update_real_pixels();

  auto parallel_start = high_resolution_clock::now();

  apply_filters();

  writeOutBmp24(fileBuffer, "output.bmp", bufferSize);

  auto parallel_end = high_resolution_clock::now();
  auto parallel_duration = duration_cast<microseconds>(parallel_end - parallel_start);
  cout << "Total time taken for applying filters and writing images: " 
                                              << std::fixed
                                              << std::setprecision(2)
                                              << (parallel_duration.count()/1000.0) << " ms" << endl;

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

void* blur(void* tid)
{
  int lvl = blur_lvl;
  long id = (long) tid;

	for (int i = 0; i < TH_ROW; i++) {
		for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        pixels_threads[id][i][j][k] = blur_color(id*TH_ROW + i, j, k, lvl);
  }
}

void* sepia(void *tid)
{
  long id = (long) tid;

  for (int i = 0; i < TH_ROW; i++)
    for (int j = 0; j < cols; j++)
    {
      pixels_threads[id][i][j][0] = min(((real_pixels[id*TH_ROW+i][j][0]*0.393) + (real_pixels[id*TH_ROW+i][j][1]*0.769) + (real_pixels[id*TH_ROW+i][j][2]*0.189)), 255.00);
      pixels_threads[id][i][j][1] = min(((real_pixels[id*TH_ROW+i][j][0]*0.349) + (real_pixels[id*TH_ROW+i][j][1]*0.686) + (real_pixels[id*TH_ROW+i][j][2]*0.168)), 255.00);
      pixels_threads[id][i][j][2] = min(((real_pixels[id*TH_ROW+i][j][0]*0.272) + (real_pixels[id*TH_ROW+i][j][1]*0.534) + (real_pixels[id*TH_ROW+i][j][2]*0.131)), 255.00);
    }
}

void set_total_avg()
{
  for (int k = 0; k < color_pallete; k++)
    avg[k] = 0;

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        avg[k] += real_pixels[i][j][k];

  for (int i = 0; i < color_pallete; i++)
    avg[i] /= (rows*cols);
}

void* mean(void *tid)
{
  long id = (long) tid;

  for (int i = 0; i < TH_ROW; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        pixels_threads[id][i][j][k] = min((real_pixels[id*TH_ROW+i][j][k]*0.4 + avg[k]*0.6), 255.00);
}

void add_X()
{
  int width = x_width;

	for (int i = width; i < rows-width; i++)
    for (int j = -width; j <= width; j++) {
      set_color(i+j, i+j, WHITE);
      set_color(i, i+j, WHITE);
      set_color(i+j, i, WHITE);
      set_color(i+j, cols - 1 - i-j, WHITE);
      set_color(i, cols - 1 - i-j, WHITE);
      set_color(i+j, cols - 1 - i, WHITE);
    }

    return;
}
