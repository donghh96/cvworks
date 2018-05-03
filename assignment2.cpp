#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "decoding.h"

using namespace cv;
using namespace std;


#define Mpixel(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)]
#define MpixelB(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())+2]


//
char *readBarcode(const Mat& image) {

    vector <vector <int> > bitsArray;

    vector <int> tmpBit;
    bool firstBlock = true;
    int totalBlocks = 0;


    for (int i=40; i<=960; i=i+20) {
        for ( int j=40; j<=960; j=j+20) {

            if ( (i <= 150 && j <= 150) || (i > 850 && j <= 150) || (i > 850 && j > 850)) {
                continue;
            } else {

                if (firstBlock) {

                    tmpBit.clear();

                    if (MpixelR(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelG(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelB(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    firstBlock = false;
                } else {
                    if (MpixelR(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelG(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelB(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    bitsArray.push_back(tmpBit);

                    firstBlock = true;

                    totalBlocks++;
                }

           }

        }
    }

    cout << "total blocks is: " << totalBlocks << endl;

    char *output = new char[1050];
    for ( int i = 0; i < bitsArray.size(); i++) {
        int value = 32 * bitsArray[i][0] + 16 * bitsArray[i][1] + 8 * bitsArray[i][2] + 4 * bitsArray[i][3] + 2 * bitsArray[i][4] + bitsArray[i][5];

        output[i] = encodingarray[value];
    }

    return output;
}


float rotateAngle(Point &pLeftTop, Point &pLeftBottom, Point &pRightBottom, Float &legLength) {

}


int main(int argc, char** argv) {

    Mat image, image_grey, image_rotated, image_final;

    image = imread(argv[1], 1);

    namedWindow("image", 1);
    namedWindow("image_rotated", 1);
//    namedWindow("image_grey", CV_8UC1);

    image_final.create(1000, 1000, CV_8UC3);


    // Rotate
    cvtColor(image, image_grey, CV_BGR2GRAY);
    vector<Vec3f> circles;
    HoughCircles(image_grey, circles, CV_HOUGH_GRADIENT, 1, 100, 100, 30, 1, 30);

    for (size_t i=0; i<circles.size(); i++) {
        Vec3i c = circles[i];
        circle(image, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 1, 1);
        cout << "c[0]: " << c[0] << "  c[1]: " << c[1] << "  c[2]: " << c[2] << endl;
    }

//    imshow("image_grey", image_grey);
    // Find the center of circle -- done, we've found 3 circles
    // find the lines, vector calculation?

    // calculate the length, find the two points
    float length01 = sqrt( pow((circles[0][0] - circles[1][0]), 2) + pow((circles[0][1] - circles[1][1]), 2));
    float length02 = sqrt( pow((circles[0][0] - circles[2][0]), 2) + pow((circles[0][1] - circles[2][1]), 2));

    cout << "length01 " << length01 << endl;
    cout << "length02 " << length02 << endl;

    Point p1, p2, p3;
    float legLength;

    if (abs(length01 - length02) < 5 ) {
        cout << "3" << endl;
        p1 = Point(circles[1][0], circles[1][1]);
        p2 = Point(circles[2][0], circles[2][1]);
        p3 = Point(circles[0][0], circles[0][1]);

        legLength = length01;
    } else if (length01 > length02) {
        cout << "1" << endl;
        p1 = Point(circles[0][0], circles[0][1]);
        p2 = Point(circles[1][0], circles[1][1]);
        p3 = Point(circles[2][0], circles[2][1]);

        legLength = length02;
    } else if (length01 < length02) {
        cout << "2" << endl;
        p1 = Point(circles[0][0], circles[0][1]);
        p2 = Point(circles[2][0], circles[2][1]);
        p3 = Point(circles[1][0], circles[1][1]);

        legLength = length01;
    }


    // rotate p1-p3 90 degree, got another point and check if it is p2???

    int x = p1.x - p3.x;
    int y = p1.y - p3.y;
    Point p4 = Point((p3.x - y), (x + p3.y));

    // if p2 p4 are not the same point
    if(abs(p2.x-p4.x)>5 || abs(p2.y-p4.y)>5) {

        cout << "here I am" << endl;
        Point tmp = p1;
        p1 = p2;
        p2 = tmp;
    }


    // We need to cauculate the angle between vector (P1 P2), and vector (1, 1)
    float theta = atan2((p2.y - p1.y), (p2.x - p1.x)) - atan2(1,1);

    theta = theta * 180 / CV_PI;
//
    cout << "theta is: " << theta << endl;


    image_rotated.create(image.size(), CV_8UC3);

    Point center = Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);

    float scale = 1;
    if( abs (legLength - 824) > 5) {
        scale = 824 / legLength;
    }

    cout << scale <<endl;
    // calculate the rotate matrix
//    Mat rotateMatrix = getRotationMatrix2D(center, theta, scale);

    // rotate it
//    warpAffine(image, image_rotated, rotateMatrix, image.size());



//    Mat image_bigger = Mat

    // cut the image, to make it 1000 * 1000

    cout << "image.rows"<<image.rows << endl;
    cout << "image.cols"<<image.cols << endl;
    cout << "image_rotated.rows"<<image_rotated.rows << endl;
    cout << "image_rotated.cols"<<image_rotated.cols << endl;
    cout << "image_final.rows"<<image_final.rows << endl;
    cout << "image_final.cols"<<image_final.cols << endl;


    // Now
    // Scale
//    if( abs(legLength - 824) > 5) {
//
//        // scale
//        Mat image_scale;
//        image_scale.create(image_rotated.size(), CV_8UC3);
//
//        warpAffine()

//    }

//    image_final.create(1000, 1000, CV_8UC3);
//    int x0 = center.x - 499;
//    int y0 = center.y - 499;
//    for (int i=0; i<1000; i++) {
//        for (int j=0; j<1000; j++) {
//            MpixelB(image_final, j, i) = MpixelB(image_rotated, y0 + j, x0 + i);
//            MpixelG(image_final, j, i) = MpixelG(image_rotated, y0 + j, x0 + i);
//            MpixelR(image_final, j, i) = MpixelR(image_rotated, y0 + j, x0 + i);
//        }
//    }

    // if
    resize(image, image_final, image_final.size(), INTER_LINEAR);

    char *output = readBarcode(image_final);
    cout << "This color 2D barcode says" << endl;
    cout << output << endl;

     imshow("image", image);
//     imshow("image_rotated", image_rotated);
     imshow("image_final", image_final);

//    int bits[6] = {0, 1, 1, 1, 0, 0};

//    // covert to int
//    int value = 32 * bits[0] + 16 * bits[1] + 8 * bits[2] + 4 * bits[3] + 2 * bits[4] + bits[5];
//
//    cout << "value is: " << value << endl;
//
//    char ch = encodingarray[value];
//
//    cout << "char is: " << ch << endl;

    waitKey(0);
    exit(0);

}
