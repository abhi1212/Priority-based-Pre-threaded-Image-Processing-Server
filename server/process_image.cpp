#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#define SMOOTH_FACTOR 30

int threshold_value = 128;
int threshold_type = 3; 
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;


using namespace cv; 

char processimage(int argc, char* argv){
    Mat image;
    Mat output_image; 

    if(!argv[1]){
        std::cerr << "No image data!" << std::endl;
        return -1; 
    }

    image = imread(argv, 1);
    while(image.empty()){
        printf("cannot access image.....empty!!! :(");
    }
    // char output[20];
    // sprintf(output,"output_%s.jpg",argv);
    // printf("otput:%s\n",output );
    switch(argc){
        case 1:
            cvtColor(image, output_image, CV_BGR2GRAY);
            imwrite(argv, output_image);
            break;

        case 2:
            bitwise_not ( image, output_image );
            imwrite( argv, output_image );
            break;
        
        case 3:
            //smooth image
            for ( int i = 1; i < SMOOTH_FACTOR; i = i + 2 ){
                GaussianBlur( image, output_image, Size( i, i ), 0, 0 );
             }
             imwrite( argv, output_image );

            break;

        case 4:
            //threshold image
            Mat gray_image;
            cvtColor( image, gray_image, COLOR_BGR2GRAY );
            threshold( gray_image, output_image, threshold_value, max_BINARY_value,threshold_type );
            imwrite( argv, output_image );

            break;

        default:

            printf("Error, No such Transformation found");
                



    }
    
    // namedWindow(argv, CV_WINDOW_AUTOSIZE);
    // namedWindow("Output", CV_WINDOW_AUTOSIZE);

    // imshow(argv, image);
    // imshow("Output", output);

    //waitKey(0);
    
    return 0;
}