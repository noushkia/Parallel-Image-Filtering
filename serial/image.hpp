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
    enum Color
    {
        RED,
        GREEN,
        BLUE
    };
};

}


#endif