/*
** client.c - a stream socket client demo
*/
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//#define PORT 3490 // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define SIZE 256

using namespace std;





int Connect(char *arg_value[])
{


    struct hostent * serv;

    if ((serv=gethostbyname(arg_value[1])) == NULL)  // Check if given Sever IP is valid
    {
        perror("Unable to resolve server IP/hostname\n");
        exit(1);
    }


    int PortNum = atoi(arg_value[2]); //Get the port Number
    if(PortNum<1024)
    {
        perror("All ports below 1024 are reserved\nYou can use ports above 1024 upto 65535 provided there are not already in use\n");
        exit(1);
    }
    struct sockaddr_in Serv_addr;
    Serv_addr.sin_family = AF_INET; // host byte order
    Serv_addr.sin_port = htons(PortNum); // short, network byte order
    Serv_addr.sin_addr = *((struct in_addr *)serv->h_addr);
    bzero(&(Serv_addr.sin_zero), 8); // zero the rest of the struct
    int sockfd; //to store the socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //Initialise Socket
    {
        perror("socket error");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&Serv_addr,sizeof(struct sockaddr)) == -1) //Connect to the socket
    {
        perror("Unable to connect to server");
        exit(1);
    }
    return sockfd;
}

void Send(string message,int sockfd){
//cout<<"sending message ... "<<endl;
if (send(sockfd,message.c_str()  , message.size(), 0) == -1)
{perror("sending message failed");
close(sockfd);
exit(0);
}
//cout<<"message sent successfully"<<endl;

}

string Recieve(int sockfd){
int numbytes; char buf[SIZE];
if ((numbytes=recv(sockfd, buf, SIZE-1, 0)) == -1) {
perror("recv");
exit(1);
}
buf[numbytes] = '\0';
//printf("Received: %s\n",buf);
string message(buf);
return message;
}



int main(int arg_count, char *arg_value[])
{
	timeval a;
    timeval b;

	string msg;//To store the temporary messages involved in the communication
    if (arg_count != 6) //Check if Input is valid
    {
        perror("Invalid Input!\nFormat : ./worker server ip/host-name> <server-port>\n");
        exit(1);
    }
    int sockfd = Connect(arg_value);

/**sending message**/

msg="user "+string(arg_value[3])+" "+string(arg_value[4])+" "+string(arg_value[5]);
cout<<"Sent: "<<msg<<endl;

    Send(msg, sockfd);
        gettimeofday(&a, 0);

     // start = std::clock();
    msg = Recieve(sockfd);
    cout<<"Received: "<<msg<<endl;
    gettimeofday(&b, 0);
    cout<<"Password length: "<<string(arg_value[5])<<endl;
    std::cout << "Time taken for cracking: " << (b.tv_sec - a.tv_sec)<<'.'<< (b.tv_usec - a.tv_usec)<<" secs" << endl;


// std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
close(sockfd);
return 0;

}