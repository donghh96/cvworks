#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;


// Cut image into small images
// l = 64, 128, 256
void cutImage(const Mat& image, const int l, vector<Mat>& subImages)
//void cutImage(const Mat& image, const int n)
{
    // sub image width & height
    int n = image.cols / l;

    //from top to bottom, left to right
    for(int y=0; y<n; y++)
    {
        for(int x=0; x<n; x++)
        {
            Rect rect = Rect(x*l, y*l, l, l);
            Mat tmpimg = image(rect);
//            imshow("sub image", tmpimg);
            subImages.push_back(tmpimg);
        }
    }
}


int main(int argc, char** argv)
{
    Mat image, image_final;

    if(argc != 2) {
        cout << "The program takes 1 argument" << endl;
        cout << argv[0] << " image_file_path" << endl;
        exit(0);
    }


    string filepath = argv[1];

    int slashpos = filepath.find_first_of("/");
    int dotpos = filepath.find_last_of(".");
    string dir = filepath.substr(0, slashpos);
    string name = filepath.substr(slashpos+1, dotpos-slashpos-1);


    image = imread(argv[1], 1);

    if (image.empty()) {
        cout << "Load image file failed." << endl;
        exit(0);
    }

    vector<Mat> subImages256;
    vector<Mat> subImages128;
    vector<Mat> subImages64;

    cout << dir << " " << name << endl;

    // cut into small images
    cutImage(image, 256, subImages256);

    for(int i=0; i<subImages256.size(); i++)
    {
        stringstream ss;
        ss << i+1;
        string num = ss.str();
        imwrite("imageRS/" + name+"_256_" + num +".png",  subImages256[i]);
    }

    // 16 128*128
    cutImage(image, 128, subImages128);
    for(int i=0; i<subImages128.size(); i++)
    {
        stringstream ss;
        ss << i+1;
        string num = ss.str();
        imwrite("imageRS/" + name+"_128_" + num +".png",  subImages128[i]);
    }

    // 64 64*64
    cutImage(image, 64, subImages64);
    for(int i=0; i<subImages64.size(); i++)
    {
        stringstream ss;
        ss << i+1;
        string num = ss.str();
        imwrite("imageRS/" + name+"_64_" + num +".png",  subImages64[i]);
    }

//    imshow("image", image);
//    imshow("image_final", image_final);

    waitKey(0);
    exit(0);

}
