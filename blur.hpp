#ifndef _BLUR_HPP_
#define _BLUR_HPP_

#include <vector>

void grayscale(int height, int width, std::vector<std::vector<std::vector<int>>> &image);

void blur(int rows, int cols, std::vector<std::vector<std::vector<int>>> &image);

#endif