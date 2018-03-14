//Write a program to copy an image and mirror it. Show both
//the original image and the mirrored image simultaneously
//(open two windows).


//macro for Mat structures
#define pixelB(image, x, y) image.data[image.step[0] * y+image.step[1] *x]
#define pixelG(image, x, y) image.data[image.step[0] * y+image.step[1] *x+1]
#define pixelR(image, x, y) image.data[image.step[0] * y+image.step[1] *x+2]


#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat image, image2;

int main(int argc, char** argv)
{
    if ( argc == 2 ) {
        image = imread( argv[1], 1 );
        image2.create( image.size(), CV_8UC3);
        cout << "CV_8UC3 is: " << CV_8UC3 << endl;


        for ( int x = 0; x < image.cols; x++ ) {
            for ( int y = 0; y < image.rows; y ++) {
                pixelB( image2, ( image.cols - x), y) = pixelB(image, x, y);
                pixelG( image2, ( image.cols - x), y) = pixelG(image, x, y);
                pixelR( image2, ( image.cols - x), y) = pixelR(image, x, y);
            }
        }

        namedWindow("image", CV_WINDOW_AUTOSIZE);
        namedWindow("image2", CV_WINDOW_AUTOSIZE);

        imshow("image", image);
        imshow("image2", image2);
        waitKey(0);
    }
    else {
        cout << "need one argument" << endl;
        exit(0);
    }
}
