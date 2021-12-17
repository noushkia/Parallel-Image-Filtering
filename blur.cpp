#include <iostream>

#include "typedefs.hpp"
#include "blur.hpp"

using namespace std;

int get_avg(vector<int> pixel)
{
    int sum = 0;
    for (int color = 0; color < color_pallete; color++)
        sum += pixel[color];
	return sum;
}

void grayscale(int rows, int cols, vector<vector<vector<int>>> &image)
{
    cout << "Initiating grayscale filter" << endl;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			int color = get_avg(image[i][j]);
            for (int k = 0; k < color_pallete; k++)
			    image[i][j][k] = color;
		}
    cout << "Filter successfull" << endl;
    return;
}


int blur_color(int rows, int cols, vector<vector<vector<int>>> &real_image, int i, int j, int color)
{
	int blurred = 0;
	for (int k = i - 2; k <= i + 2 && k > 0 && k < rows; k++)
		for (int l = j - 2; l <= j + 2 && l > 0 && l < cols; l++)
			blurred += real_image[k][l][color];
	return blurred / 25;
}

// Blur image
void blur(int rows, int cols, vector<vector<vector<int>>> &image)
{
    cout << "Initiating blur filter" << endl;

	vector<vector<vector<int>>> real_image = image;

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
            for (int k = 0; k < color_pallete; k++)
			    real_image[i][j][k] = image[i][j][k];

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
            for (int k = 0; k < color_pallete; k++)
                image[i][j][k] = blur_color(rows, cols, real_image, i, j, k);

    cout << "Filter successfull" << endl;
    
    return;
}
