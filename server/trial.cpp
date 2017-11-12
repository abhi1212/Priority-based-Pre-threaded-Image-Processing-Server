	

#include <opencv2/opencv.hpp>

using namespace cv;

int threshold_value = 128; int threshold_type = 3; int const max_value = 255;
int const max_type = 4; int const max_BINARY_value = 255;

int main( int argc, char** argv )
{
 char* imageName = argv[1];

 Mat image,dst;
 image = imread( imageName, 1 );

 if( argc != 2 || !image.data )
 {
   printf( " No image data \n " );
   return -1;
 }

 Mat gray_image;
 cvtColor( image, gray_image, COLOR_BGR2GRAY );

 threshold( gray_image, dst, threshold_value, max_BINARY_value,threshold_type );

 //imwrite( "Gray_Image.jpg", gray_image );

 namedWindow( imageName, WINDOW_AUTOSIZE );
 namedWindow( "threshold", WINDOW_AUTOSIZE );

 imshow( imageName, image );
 imshow( "threshold", dst );

 waitKey(0);

 return 0;
}
