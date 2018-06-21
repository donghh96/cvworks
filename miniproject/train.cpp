/**
 *
 * g++ -std=c++11 train.cpp -o train `pkg-config --cflags --libs opencv`
 *
 * Train:
 *
 * ./train -save example.xml -data number_alphabet.data
 *
 *
 * Test:
 *
 * ./train -load example.xml number_alphabet.data
 *
 */

#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

#include <cstdio>
#include <vector>
#include <iostream>
#include <string>
#include <istream>
#include <fstream>


using namespace std;
using namespace cv;
using namespace cv::ml;

// Split a sting by ','
void splitByComma(const string &in, vector<string> &out)
{
    string tmp;
    istringstream tokenStream(in);
    while (getline(tokenStream, tmp, ','))
    {
        out.push_back(tmp);
    }
}

// This function reads data and responses from the file <filename>
static bool
read_num_class_data( const string& filename, int var_count,
                     Mat* _data, Mat* _responses )
{
//    const int M = 1024;
//    char buf[M+2];

    Mat el_ptr(1, var_count, CV_32F);
    int i;
    vector<int> responses;

    _data->release();
    _responses->release();

    ifstream file(filename);
    string currentline;
    vector<string> splitedline;

    // Read a line frome datafile
    // Assume the classes are number, 0, 1, 2, ... store class in responses
    // Split all features values, and store them in _data.
    while(getline(file, currentline))
    {
        splitedline.clear();
        splitByComma(currentline, splitedline);
        responses.push_back(stoi(splitedline[0]));

        for( i = 0; i < var_count; i++ )
        {
            el_ptr.at<float>(i) = stof(splitedline[i+1]);
        }

        _data->push_back(el_ptr);
    }

    file.close();

    Mat(responses).copyTo(*_responses);

    cout << "The database " << filename << " is loaded.\n";

    return true;
}

template<typename T>
static Ptr<T> load_classifier(const string& filename_to_load)
{
    // load classifier from the specified file
    Ptr<T> model = StatModel::load<T>( filename_to_load );
    if( model.empty() )
        cout << "Could not read the classifier " << filename_to_load << endl;
    else
        cout << "The classifier " << filename_to_load << " is loaded.\n";

    return model;
}

inline TermCriteria TC(int iters, double eps)
{
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}

static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save)
{
    int i, nsamples_all = data.rows;
    double train_hr = 0, test_hr = 0;
    int training_correct_predict=0;

    //
    int totalOfOneClass[15] = {0};
    int correctOfOneClass[15] = {0};
    // compute prediction error on training data
    // Compute prediction correct for each class

    for( i = 0; i < nsamples_all; i++ )
    {
        Mat sample = data.row(i);

        cout << "Sample: " << responses.at<int>(i) << " row " << data.row(i) << endl;
        float r = model->predict( sample );
        cout << "Predict:  r = " << r << endl;

        totalOfOneClass[(int)(responses.at<int>(i))]++;
        if( (int)r == (int)(responses.at<int>(i)) ) //prediction is correct
        {
            training_correct_predict++;

            correctOfOneClass[(int)(responses.at<int>(i))]++;
        }

        //r = std::abs(r + rdelta - responses.at<int>(i)) <= FLT_EPSILON ? 1.f : 0.f;


        //if( i < ntrain_samples )
        //    train_hr += r;
        //else
        //    test_hr += r;
    }

    //test_hr /= nsamples_all - ntrain_samples;
    //train_hr = ntrain_samples > 0 ? train_hr/ntrain_samples : 1.;
    for(int i=0; i<15; i++)
    {
        cout << "Total count of class " << i << " is: " <<totalOfOneClass[i] << endl;
        cout << "Correct count of class " << i << " is: " << correctOfOneClass[i] << endl;
        printf( "Test Recognition of class %d = %.1f%% \n", i, correctOfOneClass[i]*100.0/totalOfOneClass[i]);
//        cout << "Rate for class "  << i << " is: " << rate << endl;
    }
    printf("ntrain_samples %d training_correct_predict %d \n",ntrain_samples, training_correct_predict);
    if( filename_to_save.empty() )  printf( "\nTest Recognition rate: training set = %.1f%% \n\n", training_correct_predict*100.0/ntrain_samples);


    if( !filename_to_save.empty() )
    {
        model->save( filename_to_save );
    }
    /*************   Example of how to predict a single sample ************************
// Use that for the assignment3, for every frame after computing the features, r is the prediction given the features listed in this format
    //Mat sample = data.row(i);
    Mat sample1 = (Mat_<float>(1,9) << 1.52101, 13.64, 4.4899998, 1.1, 71.779999, 0.059999999, 8.75, 0, 0);// 1
    float r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;
    sample1 = (Mat_<float>(1,9) << 1.518, 13.71, 3.9300001, 1.54, 71.809998, 0.54000002, 8.21, 0, 0.15000001);//2
    r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;
    sample1 = (Mat_<float>(1,9) << 1.51694,12.86,3.58,1.31,72.61,0.61,8.79,0,0);//3
    r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;
//    sample1 = (Mat_<float>(1,9) << );//4
//    r = model->predict( sample1 );
//    cout << "Prediction: " << r << endl;
    sample1 = (Mat_<float>(1,9) << 1.5151401, 14.01, 2.6800001, 3.5, 69.889999, 1.6799999, 5.8699999, 2.2, 0);//5
    r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;
    sample1 = (Mat_<float>(1,9) << 1.51852, 14.09, 2.1900001, 1.66, 72.669998, 0, 9.3199997, 0, 0);//6
    r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;
    sample1 = (Mat_<float>(1,9) << 1.51131,13.69,3.2,1.81,72.81,1.76,5.43,1.19,0);//7
    r = model->predict( sample1 );
    cout << "Prediction: " << r << endl;

    **********************************************************************/

}

static bool
build_mlp_classifier( const string& data_filename,
                      const string& filename_to_save,
                      const string& filename_to_load )
{

    // You need to change at least the number of classes and the number of features.
    const int class_count = 15;//CLASSES
    Mat data;
    Mat responses;

    bool ok = read_num_class_data( data_filename, 16, &data, &responses );//third parameter: FEATURES
    if( !ok )
        return ok;

//    cout << "Tuzi. data.size" << data.size <<endl;
//    cout << "Tuze. responses.size" <<responses.size << endl;

    Ptr<ANN_MLP> model;

    int nsamples_all = data.rows;

    // In line 181 there is a parameter split. This is to split the set into training and test.
    // Keep it at 1.0, as this will use all the available data to train the gestures.
    int ntrain_samples = (int)(nsamples_all*1.0);//SPLIT

    // Create or load MLP classifier
    if( !filename_to_load.empty() )
    {
        model = load_classifier<ANN_MLP>(filename_to_load);
        if( model.empty() )
            return false;
        //ntrain_samples = 0;
    }
    else
    {
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //
        // MLP does not support categorical variables by explicitly.
        // So, instead of the output class label, we will use
        // a binary vector of <class_count> components for training and,
        // therefore, MLP will give us a vector of "probabilities" at the
        // prediction stage
        //
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        Mat train_data = data.rowRange(0, ntrain_samples);
        Mat train_responses = Mat::zeros( ntrain_samples, class_count, CV_32F );

        cout << "tuzi. train_data.cols:" << train_data.cols << "train_data.rows:" << train_data.rows << endl;
        cout << "tuzi. train_responses.cols:" << train_responses.cols << "train_responses.rows:" << train_responses.rows << endl;

        // 1. unroll the responses
        cout << "Unrolling the responses...\n";
        for( int i = 0; i < ntrain_samples; i++ )
        {
//            int cls_label = responses.at<int>(i) - 48;// - 'A'; //change to numerical classes, still they read as chars
            int cls_label = responses.at<int>(i);
            cout << "labels " << cls_label << endl;
            train_responses.at<float>(i, cls_label) = 1.f;
        }

        // 2. train classifier
        // If you know about neural networks you can also change the number of hidden layers
        // (in the program it is a function of the layer_sz[]). The number of inputs is the number
        // of features (data.cols) and the number of outputs is the number of classes (class_count).
        // 2 layers default, the number is to define number of hidden nodes of each hidden layer.
//        int layer_sz[] = { data.cols, 15, 50, 15,class_count };
        int layer_sz[] = { data.cols, 100, 100, class_count };
        cout <<  " sizeof layer_sz " << sizeof(layer_sz) << " sizeof layer_sz[0]) " << sizeof(layer_sz[0]) << endl;
        int nlayers = (int)(sizeof(layer_sz)/sizeof(layer_sz[0]));
        cout << " nlayers  " << nlayers << endl;
        Mat layer_sizes( 1, nlayers, CV_32S, layer_sz );

#if 1
        int method = ANN_MLP::BACKPROP;
        double method_param = 0.001;
//        int max_iter = 10000;
        int max_iter = 300;
#else
        int method = ANN_MLP::RPROP;
        double method_param = 0.1;
        int max_iter = 1000;
#endif

        Ptr<TrainData> tdata = TrainData::create(train_data, ROW_SAMPLE, train_responses);

        cout << "Training the classifier (may take a few minutes)...\n";
        model = ANN_MLP::create();
        model->setLayerSizes(layer_sizes);
        model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0, 0);
        model->setTermCriteria(TC(max_iter,0));
        model->setTrainMethod(method, method_param);
        model->train(tdata);
        cout << endl;
    }

    //test_and_save_classifier(model, data, responses, ntrain_samples, 'A', filename_to_save);
    test_and_save_classifier(model, data, responses, ntrain_samples, 0, filename_to_save);
    return true;
}



int main( int argc, char *argv[] )
{
    string filename_to_save = "";
    string filename_to_load = "";
    string data_filename = "letter-recognition.data";
    int method = 0;

    int i;
    for( i = 1; i < argc; i++ )
    {
        if( strcmp(argv[i],"-data") == 0 ) // flag "-data letter_recognition.xml"
        {
            i++;

            data_filename = argv[i];
        }
        else if( strcmp(argv[i],"-save") == 0 ) // flag "-save filename.xml"
        {
            i++;
            filename_to_save = argv[i];
            cout << "filename to save is "<< filename_to_save << endl;
        }
        else if( strcmp(argv[i],"-load") == 0) // flag "-load filename.xml"
        {
            i++;
            filename_to_load = argv[i];
        }
    }
    build_mlp_classifier( data_filename, filename_to_save, filename_to_load );
}
