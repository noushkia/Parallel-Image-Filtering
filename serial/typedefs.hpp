#ifndef _TD_HPP_
#define _TD_HPP_

#include <string>

#pragma pack(1)
#pragma once

#define TOTAL_FILTERS 4
#define BLUR 0
#define SEPIA 1
#define MEAN 2
#define ADDX 3

#define RED 0
#define GREEN 1
#define BLUE 2

const int color_pallete = 3;
const int WHITE[] = {255, 255, 255};
const int TH_ROW = 20;
const int blur_lvl = 1;
const int x_width = 1;
const std::string output_dir = "Filtered/";


typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char*** RGBS;


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

#endif