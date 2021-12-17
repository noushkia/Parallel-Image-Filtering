#ifndef _IMG_HPP_
#define _IMG_HPP_

#include <vector>

namespace std {

class Image
{
private:
    vector<vector<vector<int>> > pixels;

public:
    Image(vector<vector<vector<int>> > &pixels);
    //Smoothing
    //Sepia
    //Wash out
    //X on img
    enum Color
    {
        RED,
        GREEN,
        BLUE
    };
};

}


#endif