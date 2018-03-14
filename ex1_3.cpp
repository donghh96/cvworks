//Modify the program to copy the image to a grey-scale image.
//Save the grey-scale image with a different format than the
//original one.


#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;

Mat image, image2;

int main(int argc, char** argv)
{
    if ( argc == 2 ) {
        image = imread( argv[1], 1 );

        image2.create( image.size(), CV_8UC1);
        cvtColor( image, image2, CV_BGR2GRAY);

        namedWindow("image", CV_WINDOW_AUTOSIZE);
        namedWindow("image2", CV_WINDOW_AUTOSIZE);

        imshow("image", image);
        imshow("image2", image2);

        imwrite("revert.png", image2);
        waitKey(0);
    }
    else {
        cout << "need one argument" << endl;
        exit(0);
    }
}
