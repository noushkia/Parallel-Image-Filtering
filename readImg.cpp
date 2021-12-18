#include <iostream>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <math.h>
#include <algorithm>

#include "typedefs.hpp"
#include "image.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std;
using namespace std::chrono;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

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
vector<vector<vector<unsigned char>>> pixels;
vector<vector<vector<unsigned char>>> real_pixels;

void grayscale();
void blur(int lvl);
void sepia();
void mean();
void add_X(int width);

void initialize_pixels()
{
  for (int i = 0; i < rows; i++) {
    vector<unsigned char> pixel(color_pallete, 0);
    vector<vector<unsigned char>> row(cols, pixel);
    pixels.push_back(row);
    real_pixels.push_back(row);
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

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
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
          real_pixels[i][j][Image::RED] = fileReadBuffer[end - count];
          break;
        case 1:
          real_pixels[i][j][Image::GREEN] = fileReadBuffer[end - count];
          break;
        case 2:
          real_pixels[i][j][Image::BLUE] = fileReadBuffer[end - count];
          break;
        }
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
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
          fileBuffer[bufferSize - count] = pixels[i][j][Image::RED];
          break;
        case 1:
          fileBuffer[bufferSize - count] = pixels[i][j][Image::GREEN];
          break;
        case 2:
          fileBuffer[bufferSize - count] = pixels[i][j][Image::BLUE];
          break;
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
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

  auto start = high_resolution_clock::now();

  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);

  auto stop = high_resolution_clock::now();  
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken for reading image: " << duration.count() << " ms" << endl;

  pixels = real_pixels;

  auto total_start = high_resolution_clock::now();
  start = high_resolution_clock::now();

  blur(1);

  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken: " << duration.count() << " ms" << endl;
  
  writeOutBmp24(fileBuffer, "blur.bmp", bufferSize);
  real_pixels = pixels;
  
  start = high_resolution_clock::now();

  sepia();

  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken: " << duration.count() << " ms" << endl;
  
  writeOutBmp24(fileBuffer, "sepia.bmp", bufferSize);
  real_pixels = pixels;

  start = high_resolution_clock::now();

  mean();

  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken: " << duration.count() << " ms" << endl;

  writeOutBmp24(fileBuffer, "mean.bmp", bufferSize);
  real_pixels = pixels;

  start = high_resolution_clock::now();

  add_X(2);

  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken: " << duration.count() << " ms" << endl;

  writeOutBmp24(fileBuffer, "add_X.bmp", bufferSize);

  auto total_end = high_resolution_clock::now();
  duration = duration_cast<microseconds>(total_end - total_start);
  cout << "Total time taken for applying filters and writing images: " << duration.count() << " ms" << endl;

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
      for (int k = 0; k < color_pallete; k++) {
          pixels[i][j][k] = blur_color(i, j, k, lvl);
      }
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

vector<unsigned char> get_total_avg()
{
    vector<unsigned char> avg(color_pallete, 0);

    for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
            for (int k = 0; k < color_pallete; k++)
                avg[k] += real_pixels[i][j][k];

    for (int i = 0; i < color_pallete; i++)
        avg[i] /= (rows*cols);

    return avg;
}

void mean()
{
    cout << "Initiating mean filter" << endl;
    pixels = real_pixels;

    vector<unsigned char> avg(color_pallete, 0);
    avg = get_total_avg();

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
  pixels = real_pixels;

	for (int i = width; i < rows-width; i++)
        for (int j = -width; j <= width; j++) {
            // cout << int(pixels[i+j][i+j][0]) << endl;
            pixels[i+j][i+j] = {255, 255,  255};
            pixels[i][i+j] = {255, 255,  255};
            pixels[i+j][i] = {255, 255,  255};
            pixels[i+j][cols - 1 - i-j] = {255, 255,  255};
            pixels[i][cols - 1 - i-j] = {255, 255,  255};
            pixels[i+j][cols - 1 - i] = {255, 255,  255};
            // cout << int(pixels[i+j][i+j][0]) << endl;
        }

    cout << "Filter successfull" << endl;
    return;
}
