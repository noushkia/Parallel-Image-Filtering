#include <iostream>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <math.h>
#include <algorithm>
#include <iomanip>

#include "typedefs.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std;
using namespace std::chrono;

int rows;
int cols;
RGBS pixels;
RGBS real_pixels;
unsigned char avg[color_pallete];


void blur(int lvl);
void sepia();
void mean();
void add_X(int width);

void initialize_pixels()
{
  pixels = new unsigned char**[rows];
  real_pixels = new unsigned char**[rows];
  for (int i = 0; i < rows; i++) {
    pixels[i] = new unsigned char*[cols];
    real_pixels[i] = new unsigned char*[cols];
    for (int j = 0; j < cols; j++) {
      pixels[i][j] = new unsigned char[color_pallete];
      real_pixels[i][j] = new unsigned char[color_pallete];
    }
  }
}

void set_color(int row, int col, const int color[])
{
  for (int i = 0; i < color_pallete; i++)
    pixels[row][col][i] = color[i];
}

void update_real_pixels()
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      for (int k = 0; k < color_pallete; k++)
        real_pixels[i][j][k] = pixels[i][j][k];
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
          real_pixels[i][j][RED] = fileReadBuffer[end - count];
          break;
        case 1:
          real_pixels[i][j][GREEN] = fileReadBuffer[end - count];
          break;
        case 2:
          real_pixels[i][j][BLUE] = fileReadBuffer[end - count];
          break;
        }
        count++;
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
          fileBuffer[bufferSize - count] = pixels[i][j][RED];
          break;
        case 1:
          fileBuffer[bufferSize - count] = pixels[i][j][GREEN];
          break;
        case 2:
          fileBuffer[bufferSize - count] = pixels[i][j][BLUE];
          break;
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

void set_total_avg()
{
  int sum[color_pallete];
  for (int k = 0; k < color_pallete; k++)
    sum[k] = 0;

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < color_pallete; k++)
        sum[k] += real_pixels[i][j][k];
      
  for (int k = 0; k < color_pallete; k++)
    avg[k] = sum[k] / (rows*cols);
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
      set_total_avg();
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

    update_real_pixels();    

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken to write on real_pixel: " << std::fixed
                                                  << std::setprecision(2)
                                                  << duration.count()/1000.0 << " ms" << endl;
  }

  return;
}

void filter_serial(char *fileBuffer, int bufferSize, char *fileName)
{
  auto serial_start = high_resolution_clock::now();

  auto start = high_resolution_clock::now();

  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer); // Secondary Hotspot

  auto stop = high_resolution_clock::now();  
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken for reading image: " << std::fixed
                                           << std::setprecision(2)
                                           << duration.count()/1000.0 << " ms" << endl;

  pixels = real_pixels;

  apply_filters();

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

  filter_serial(fileBuffer, bufferSize, fileName);
  
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
      pixels[i][j][RED] = 
        min(((real_pixels[i][j][RED]*0.393) + (real_pixels[i][j][GREEN]*0.769) + (real_pixels[i][j][BLUE]*0.189)), 255.00);
      pixels[i][j][GREEN] = 
        min(((real_pixels[i][j][RED]*0.349) + (real_pixels[i][j][GREEN]*0.686) + (real_pixels[i][j][BLUE]*0.168)), 255.00);
      pixels[i][j][BLUE] = 
        min(((real_pixels[i][j][RED]*0.272) + (real_pixels[i][j][GREEN]*0.534) + (real_pixels[i][j][BLUE]*0.131)), 255.00);
    }

  cout << "Filter successfull" << endl;
  return;
}

void mean()
{
  cout << "Initiating mean filter" << endl;

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
