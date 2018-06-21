#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

#define Mpixel(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)]


void histogram(const Mat& image, int histo[256])
{
//    int histogram[256] = {0};
    for (int y=0; y<image.cols; y++)
    {
        for (int x=0; x<image.rows; x++)
        {
            histo[image.at<uchar>(y, x)]++;
        }
    }

}

void contrstStreching(const Mat& image_src, Mat& image_dst)
{
    double mean, var, sdev;
    int N = image_src.rows * image_src.cols;
    float c = 15;
    for(int x =0 ; x< image_src.cols; x++) {
        for(int y =0 ;y< image_src.rows; y++) {
            mean = mean + Mpixel(image_src, x, y);
            var  = var + pow(Mpixel(image_src, x, y), 2);
        }
    }
    mean = mean/N;
    var = (var/N) - mean * mean;
    sdev = sqrt(var);

    for(int x =0 ; x< image_src.cols; x++) {
        for(int y =0 ;y< image_src.rows; y++) {
           float cpixel = (255.0 * (Mpixel(image_src, x, y) - mean + c * sdev)) / (2*c*sdev );
           if (cpixel > 255) cpixel = 255;
           if (cpixel < 0 ) cpixel = 0;
           Mpixel(image_dst, x, y) = cvRound(cpixel);
        }
    }
}


int main(int argc, char** argv)
{
    Mat image, image_contrast;

    if(argc != 2) {
        cout << "The program takes 1 argument" << endl;
        cout << argv[0] << " image_file_path" << endl;
        exit(0);
    }


    string filepath = argv[1];

    image = imread(argv[1], 1);

    if (image.empty()) {
        cout << "Load image file failed." << endl;
        exit(0);
    }

    int hist[256] ={0};
    histogram(image, hist);

    for(int i=0; i<256; i++)
    {
        cout << hist[i] << ", ";
    }

    cout <<endl;


//    imshow("image", image);
//    imshow("image_final", image_final);

    waitKey(0);
    exit(0);

}
