/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */

/************************************************************************************************************

Initilization and Header Files and Function Declarations
*************************************************************************************************************/
#include "csapp.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void openImage(char *name);

/************************************************************************************************************
Function Name-  Main
Function Purpose- Simulation of an Echo Client
Arguments-  int argc, char **argv
Return Type- Int
*************************************************************************************************************/


int main(int argc, char **argv) 
{
    int clientfd, port, processNumber;            
    char *host,*image; 
    // char *buf[MAXLINE];
    rio_t rio;                                   // Object for Rio Package.

    if (argc != 4) {                             // Throw an error if not enough arguments provided
    	fprintf(stderr, "usage: %s <host> <port> <image> \n", argv[0]);
    	exit(0);
    }
    printf("Case 1:  Gray Image\n");
    printf("Case 2:  Invert Image\n");
    printf("Case 3:  Smooth Image\n");
    printf("Case 4:  Thresholding Image\n");

    printf("Enter your choice :");
    cin>> processNumber;


    host = argv[1];
    port = atoi(argv[2]);
	image = argv[3];
    //processNumber = atoi(argv[4]);

    printf("Process Number: %d", processNumber);    // Process number deceides the functionality


    clientfd = Open_clientfd(host, port);           // Open Client connection
    Rio_readinitb(&rio, clientfd);                  // Initiliazation of Rio

    /*while (Fgets(buf, MAXLINE, stdin) != NULL) {
	Rio_writen(clientfd, buf, strlen(buf));
	Rio_readlineb(&rio, buf, MAXLINE);
	Fputs(buf, stdout);
    }*/
    FILE *fp = fopen(image,"rb");                    // Opening a file
        if(fp==NULL)                                 // Checking for No file condition
        {
            printf("File open error");
            return 1;   
        }  

    fseek(fp, 0, SEEK_END);                          // Traversing till End of file
    int size = ftell(fp);                            // Get size of the file

    //send the size of image
    Rio_writen(clientfd, &size, sizeof(size));       // Writing the contents to the connection descriptor

    //send process number for image
    Rio_writen(clientfd, &processNumber, sizeof(processNumber));  

    fseek(fp, 0, SEEK_SET);                          //  Traversing till Start of file

    //sending image file
    while(1)
    {
        /* First read file in chunks of 256 bytes */
        //printf("%d\n",size );
        unsigned char buff[256];

        //fseek(fp, 0, SEEK_SET);

        int nread = fread(buff,1,256,fp);             // Copies the file content to buffer
        printf("File read :%d bytes\n", nread);       // nread gives us the number of bytes read 

        /* If read was success, send data. */
        if(nread > 0)
        {
            Rio_writen(clientfd, buff, nread);       // Sending the files in section of 256 bytes
            //
            printf("Sending %d\n",nread);
            // write(clientfd, buff, nread);
        }

        /*
         * There is something tricky going on with read .. 
         * Either there was error, or we reached end of file.
         */
        if (nread < (256))
        {
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }



    }
    fclose(fp);                             // Close the file


    //receive image
    int rx_fsize;
    int bytesReceived = 0; 
    char recvBuff[256]; 
    char rx_image[20];
    sprintf(rx_image,"%d_rx.jpg",clientfd);
    //printf("rx image:%s",rx_image);
    Rio_readnb(&rio,&rx_fsize,sizeof(rx_fsize));
    //rx_fsize = atoi(recvBuff);
    printf("Size of rx file %d\n",rx_fsize );


    FILE *rx_fp;
    rx_fp = fopen(rx_image, "wb");

    if(NULL == rx_fp)
    {
        printf("Error opening file");
        return -1;
    }

    // fseek(fp, 0, SEEK_END);
    // int size = ftell(fp);
    fseek(rx_fp, 0, SEEK_SET);

    int recData = 256;

    // Receive data in chunks of 256 bytes untill rx_fsize data is received
    // while((bytesReceived = read(connfd, recvBuff, 256)) > 0)
    while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && rx_fsize>0)
    {
        printf("Bytes received %d\n",bytesReceived);    
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,rx_fp);
        // printf("%s \n", recvBuff);
        rx_fsize -= bytesReceived;
        if (rx_fsize<256)       //if remaining bytes is less than 256, read only remaining bytes of data
            recData = rx_fsize;
    }

    fclose(rx_fp);
    printf("File received\n");



    Close(clientfd); //line:netp:echoclient:close

    openImage(image);
    openImage(rx_image);
    waitKey(0);
    exit(0);
}
/* $end echoclientmain */

void openImage(char *name){
    Mat image;
    image = imread(name, 1);
    namedWindow(name, CV_WINDOW_AUTOSIZE);
    imshow(name,image);
    //waitKey(0);
}
