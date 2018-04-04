#include  "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat image, image2;

void bubbleSort(int arr[], int n)
{
	for(int i = 0;i < n;i++){
		for(int j = 0;j < n-i-1;j++){
            if(arr[j] > arr[j+1]){
                int t = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = t;
            }
        }
    }
}

int main(int argc, char** argv)
{
    //read image_in file, 1 load three channels
    image = imread(argv[1], 0);
    image2.create(image.size(), CV_8UC1);


//    int mask[9];
    int mask[25];

    for(int y=1; y<image.rows; y++)
    {
        for(int x=1; x<image.cols; x++)
        {

            for(int i=0; i<5; i++)
            {
                for(int j=0; j<5; j++)
                {
                    mask[i*5+j] = image.at<uchar>(y+j-2, x+i-2);
                }
            }

            bubbleSort(mask, 25);

            image2.at<uchar>(y,x) = mask[12];

        }
    }
    namedWindow("Before_filter", CV_WINDOW_AUTOSIZE);
    namedWindow("After_filter", CV_WINDOW_AUTOSIZE);

    imshow("Before_filter", image);
    imshow("After_filter", image2);

    waitKey(0);
    exit(0);
}
