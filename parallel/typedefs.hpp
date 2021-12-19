#ifndef _TD_HPP_
#define _TD_HPP_

#include <string>

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
const int TH_ROW = 25;
const int blur_lvl = 1;
const int x_width = 1;
const std::string output_dir = "Filtered/";


typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char*** RGBS;

#endif