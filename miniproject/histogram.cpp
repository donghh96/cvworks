#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

#define Mpixel(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)]

map <string, int> classMap = {{"D1", 0}, {"D7",1}, {"D15",2},{"D21",3},{"D28",4},{"D29",5},{"D31",6},{"D35",7},{"D49",8},
                            {"D65",9},{"D71",10},{"D101",11},{"D106",12},{"D108",13},{"D111",14}};

void histogram(const Mat& image, int histo[256])
{
//    int histogram[256] = {0};
    for (int y=0; y<image.cols; y++)
    {
        for (int x=0; x<image.rows; x++)
        {
            histo[Mpixel(image, x, y)]++;
        }
    }

}
void NormaliseHisto(const Mat& image, float histo[256])
{

    int N = image.cols * image.rows  / 4096;

    for (int y=0; y<image.cols; y++)
    {
        for (int x=0; x<image.rows; x++)
        {
            histo[Mpixel(image, x, y)] += 1.0;
        }
    }

    for(int i=0; i<256; i++)
    {
        histo[i] = histo[i] / (float)N;
    }

}

void contrastStreching(const Mat& image_src, Mat& image_dst)
{
    double mean, var, sdev;
    int N = image_src.rows * image_src.cols;
    float c = 1.5;
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
//           cout << cpixel << endl;
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
    cvtColor(image, image, CV_BGR2GRAY);

    image_contrast.create(image.size(), CV_8UC1);

    if (image.empty()) {
        cout << "Load image file failed." << endl;
        exit(0);
    }

    int slashPos = filepath.find_last_of("/");
    int pos = filepath.find_first_of("_");
    string texture = filepath.substr(slashPos+1, pos-slashPos-1);

    int textureClass = classMap[texture];

    float hist[256] ={0.0};
    contrastStreching(image, image_contrast);
    NormaliseHisto(image_contrast, hist);

    stringstream ss;
    ss << textureClass;
    for(int i=0; i<256; i++) {
        ss << "," << hist[i];
    }

    string output = ss.str();

    cout << output << endl;

//    imshow("image", image);
    imshow("image_contrast", image_contrast);

    waitKey(0);
    exit(0);

}
