#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"

#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

//#define Mpixel(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)]
#define Mpixel(image, x, y)((int*)(((image).data) + (y)*((image).step)))[(x)]


bool isPowerof2(const Mat& image)
{
    return true;
}

double meanValue(const Mat_<int>& image, int rowStart, int rowEnd, int colStart, int colEnd)
{
    double mean, var, sdev;

    int N = (rowEnd - rowStart) * (colEnd - colStart);

    for(int x=colStart; x<colEnd; x++)
    {
        for(int y=rowStart; y<rowEnd; y++)
        {
            mean = mean + abs(Mpixel(image, x, y));
        }
    }

    mean = mean / N;
    return mean;
}

vector<double> meanAndSdev(const Mat_<int>& image, int rowStart, int rowEnd, int colStart, int colEnd)
{
    double mean, var, sdev;
    vector<double> output;

    int N = (rowEnd - rowStart) * (colEnd - colStart);

    for(int x=colStart; x<colEnd; x++)
    {
        for(int y=rowStart; y<rowEnd; y++)
        {
            mean = mean + abs(Mpixel(image, x, y));
            var = var + pow(Mpixel(image, x, y), 2);
        }
    }

    mean = mean / N;
    var = (var / N) - (mean * mean);
    sdev = sqrt(var);

    output.push_back(mean);
    output.push_back(sdev);
    return output;
}

void haarTransform(const Mat_<int>& image_src, Mat_<int>& image_dst, int level, vector<double>& features)
{
    // rows should equals columns
    int n = image_src.rows;


    // cout << n << " "<< m<< endl;
    int tempData[n];

    for(int l=1; l<level;l++)
    {
        n = n / l;

        // Row
        for(int y=0; y<n; y++)
        {
            for(int x=0; x<n/2; x++)
            {
                int tmp1 = Mpixel(image_src, 2*x, y);
                int tmp2 = Mpixel(image_src, 2*x+1, y);
                tempData[x] = (tmp1 + tmp2) / 2;
                tempData[x+n/2] = (tmp1 - tmp2) / 2;
            }

            for(int x=0; x<n; x++)
            {
                Mpixel(image_dst, x, y) = tempData[x];
            }
        }


        // Column
        for(int x=0; x<n; x++)
        {
            for(int y=0; y<n/2; y++)
            {
                int tmp1 = Mpixel(image_dst, x, 2*y);
                int tmp2 = Mpixel(image_dst, x, 2*y+1);
                tempData[y] = (tmp1 + tmp2) / 2;
                tempData[y+n/2] = (tmp1 - tmp2) / 2;
            }

            for(int y=0; y<n; y++)
            {
                Mpixel(image_dst, x, y) = tempData[y];
            }
        }



//        for(int y=0; y<n; y++)
//        {
//            for(int x=0; x<n; x++)
//            {
//                if(x>=n/2 || y>=n/2)
//                {
//                    Mpixel(image_dst, x, y) +=50;
//                }
//            }
//        }

        vector<double> tmpMeanAndSdev;

        tmpMeanAndSdev = meanAndSdev(image_dst, 0, n/2, 0, n/2);
        features.push_back(tmpMeanAndSdev[0]);
        features.push_back(tmpMeanAndSdev[1]);
        tmpMeanAndSdev.clear();

        tmpMeanAndSdev = meanAndSdev(image_dst, 0, n/2, n/2, n);
        features.push_back(tmpMeanAndSdev[0]);
        features.push_back(tmpMeanAndSdev[1]);
        tmpMeanAndSdev.clear();

        tmpMeanAndSdev = meanAndSdev(image_dst, n/2, n, 0, n/2);
        features.push_back(tmpMeanAndSdev[0]);
        features.push_back(tmpMeanAndSdev[1]);
        tmpMeanAndSdev.clear();

        tmpMeanAndSdev = meanAndSdev(image_dst, n/2, n, n/2, n);
        features.push_back(tmpMeanAndSdev[0]);
        features.push_back(tmpMeanAndSdev[1]);
    }

}

void haarReverse(const Mat& image_src, Mat& image_dst)
{
        // rows should equals columns
    int n = image_src.rows;
    int tempData[n];

    // Row
    for(int y=0; y<n; y++)
    {
        for(int x=0; x<n/2; x++)
        {
            int tmp1 = Mpixel(image_src, x, y);
            int tmp2 = Mpixel(image_src, x+n/2, y);
            tempData[2*x] = tmp1 + tmp2;
            tempData[2*x+1] = tmp1 - tmp2;
        }

        for(int x=0; x<n; x++)
        {
            Mpixel(image_dst, x, y) = tempData[x];
        }
    }

    // Column
    for(int x=0; x<n; x++)
    {
        for(int y=0; y<n/2; y++)
        {
            int tmp1 = Mpixel(image_dst, x, y);
            int tmp2 = Mpixel(image_dst, x, y+n/2);
            tempData[2*y] = tmp1 + tmp2;
            tempData[2*y+1] = tmp1 - tmp2;
        }

        for(int y=0; y<n; y++)
        {
            Mpixel(image_dst, x, y) = tempData[y];
        }
    }
}

int main(int argc, char** argv)
{
    Mat image_src, image_dst, image_reverse;

    image_src = imread(argv[1], 0);
    image_dst.create(image_src.size(), CV_8UC1);
    image_reverse.create(image_src.size(), CV_8UC1);

    Mat_<int> src = image_src;
    Mat_<int> result = image_dst;

    vector<double> features;

    if (image_src.empty())
    {
        cout << "Load image file failed." << endl;
        return 0;
    }

//    haarTransform(image_src, image_dst, 2);
    haarTransform(src, result, 3, features);
//    haarReverse(image_dst, image_reverse);

//    cout << src << endl;
//    cout << result << endl;
    for(int i=0; i<features.size(); i++) {
        cout << features[i] << endl;
    }
    image_dst = result;

//    imwrite("dec2.png", image_dst);

    imshow("src", image_src);
    imshow("dst", image_dst);
//    imshow("reverse", image_reverse);

    waitKey(0);
    exit(0);

}
