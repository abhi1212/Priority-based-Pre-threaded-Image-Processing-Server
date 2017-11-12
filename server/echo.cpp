/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"
char imageRcvd = 1;
int processimage(int, char*);

void echo(int connfd) 
{
    size_t n;
    int bytesReceived = 0; 
    char recvBuff[256]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
 
    //receive size of image
    int rx_fsize;
    Rio_readnb(&rio,&rx_fsize,sizeof(rx_fsize));
    printf("Size of file %d\n",rx_fsize );

    //receive process of image
    int process;
    Rio_readnb(&rio,&process,sizeof(process));
    printf("Process %d\n",process );

    char client_image[20];
    sprintf(client_dddimage,"%d_rx.png",connfd);

    FILE *fp;
    fp = fopen(client_image, "wb");

    if(NULL == fp)
    {
        printf("Error opening file");
        return;
    }

    fseek(fp, 0, SEEK_SET);

    int recData = 256;

    /* Receive data in chunks of 256 bytes untill rx_fsize data is received*/
    while(((bytesReceived = Rio_readnb(&rio,recvBuff,recData))>0) && rx_fsize>0)
    {
        //printf("Bytes received %d\n",bytesReceived);    
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        // printf("%s \n", recvBuff);
        rx_fsize -= bytesReceived;
        //printf("remaining bytes :%d\n",rx_fsize);
        if (rx_fsize<256)	//if remaining bytes is less than 256, read only remaining bytes of data
        	recData = rx_fsize;
    }

    fclose(fp);
    printf("File received\n");

    /****************************process image***************************************/
    processimage(process,client_image);


    /****************************Send file back to client************************/

    //printf("sending file%s\n",send );
    FILE *fp2;
    printf("Sending Back\n");
    fp2 = fopen(client_image, "rb");

    if(NULL == fp2)
    {
        printf("Error opening file");
        return;
    }

    fseek(fp2, 0, SEEK_END);
    int tx_size = ftell(fp2);
    printf("tx_size = %d\n",tx_size);
    fseek(fp2, 0, SEEK_SET);

    //send the size of image
    Rio_writen(connfd, &tx_size, sizeof(tx_size));

    fseek(fp2, 0, SEEK_SET);

    //sending back image file to client
    while(1)
    {
        /* First read file in chunks of 256 bytes */
        //printf("%d\n",size );
        unsigned char buff[256];

        int nread = fread(buff,1,256,fp2);
        // printf("File read :%d bytes\n", nread);        

        /* If read was success, send data. */
        if(nread > 0)
        {
            // printf("Sending back\n");
            
            Rio_writen(connfd, buff, nread);
        }

        /*
         * There is something tricky going on with read .. 
         * Either there was error, or we reached end of file.
         */
        if (nread < (256))
        {
            if (feof(fp2))
                printf("End of file\n");
            if (ferror(fp2))
                printf("Error reading\n");
            break;
        }

        
    }
    fclose(fp2);                        //close image pointer

    remove(client_image);


    // if(bytesReceived < 0)
    // {
    //     printf("\n Read Error \n");
    // }

    // if (bytesReceived < 256)
    // {
    // 	printf("bytesReceived<256\n");
    //     if (feof(fp)){
    //         printf("End of file\n");
    //     // 	printf("File Receive completed\n");
    //     // 	//imageRcvd =2;
    //     }
    //     if (ferror(fp))
    //         printf("Error reading\n");
    // }


}
/* $end echo */

