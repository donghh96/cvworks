#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat image , image2;

int main( int argc , char** argv )
{
    if ( argc == 3)
    { 
        image = imread( argv [1] , 1 );
        image2 = imread( argv [2] , 1 );
        namedWindow( "Image", CV_WINDOW_AUTOSIZE );
        
        imshow( "Image", image );
        waitKey (0);
        imshow( "Image", image2 );
        waitKey (0);
        return 0;
    }
        
        
        else { cout << "it needs two arguments" << endl; 
        exit (0);
        }
        
}

