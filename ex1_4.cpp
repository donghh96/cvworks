//Write a program that loads a colour image. Using a trackbar,
//the user should be able to change the image to grey-scale and
//back to colour (the trackbar should allow for values 0 and 1).

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


//define color mat, grey mat.
Mat grey_image, color_image;
int markerclick = 0;

void callback_trackbar_click (int click, void* object)
{
    markerclick = click;
    if (markerclick == 0)
    {
        imshow("showresult", grey_image);
    }
    else
    {
        imshow("showresult", color_image);
    }
}

int main(int argc, char** argv)
{


    //open image file
    if (argc == 2)
    {
        color_image = imread(argv[1], 1);
        grey_image = imread(argv[1], 0);


        namedWindow("showresult", 1);

        createTrackbar("trackbar", "showresult", &markerclick, 1, callback_trackbar_click);

        setTrackbarPos("trackbar", "showresult", 0);

        imshow("showresult", grey_image);

        waitKey(0);
    }

    else {
        cout << "need one argument" << endl;
        exit(0);
    }


}

