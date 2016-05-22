#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <crypt.h>
#include <thread>
#include <stdexcept>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SIZE 256
using namespace std;

char c1;
char c2;
bool stop_decrypt;
string hash_value,bnry_str,password;
int pwd_len;

void mypart(int part){
    if(part>=0&&part<9)c1=part+'0';        
    if(part>=9&&part<34)c1=part+'a'-9;        
    if(part>=34&&part<60)c1=part+'A'-34;        
    c2=c1+1;

    // switch(part){
    //     case 0 :
    //         c1='0';
    //         c2='9';
    //         break;
    //     case 1 :
    //         c1='a';
    //         c2='j';
    //         break;
    //     case 2 :
    //         c1='k';
    //         c2='t';
    //         break;
    //     case 3 :
    //         c1='u';
    //         c2='z';
    //         break;
    //     case 4 :
    //         c1='A';
    //         c2='J';
    //         break;
    //     case 5 :
    //         c1='K';
    //         c2='T';
    //         break;
    //     case 6 :
    //         c1='U';
    //         c2='Z';
    //         break;        
    // }
    cout<<c1<<" "<<c2<<endl;
    return;
}




int next(string& test){

    int i,carry;
    carry=1;
    i=pwd_len;
    while(true){
        i--;
        if(test[i]=='9'){
            if(bnry_str[2]=='0'){
                return -1;
            }
            if(bnry_str=="001"){
                test[i]='0';
                continue;
            }
            if(bnry_str=="011"||bnry_str=="111"){
                test[i]='a';
                break;
            }
            if(bnry_str=="101"){
                test[i]='A';
                break;
            }
        }
        if(test[i]=='z'){
            if(bnry_str[1]=='0'){
                return -1;
            }
            if(bnry_str=="010"){
                test[i]='a';
                continue;
            }
            if(bnry_str=="110"||bnry_str=="111"){
                test[i]='A';
                break;
            }
            if(bnry_str=="011"){
                test[i]='0';
                continue;
            }
        }

        if(test[i]=='Z'){
            if(bnry_str[0]=='0'){
                return -1;
            }
            if(bnry_str=="100"){
                test[i]='A';
                continue;
            }
            if(bnry_str=="101"||bnry_str=="111"){
                test[i]='0';
                continue;
            }
            if(bnry_str=="110"){
                test[i]='a';
                continue;
            }
        }
        test[i]+=1;
        break;
    }

    return 0;
}

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
if (send(sockfd,message.c_str()  , message.size(), 0) == -1)
{perror("sending message failed");
close(sockfd);
exit(0);
}
}

string Recieve(int sockfd){
int numbytes; char buf[SIZE];
if ((numbytes=recv(sockfd, buf, SIZE-1, 0)) == -1) {
perror("recv");
exit(1);
}
buf[numbytes] = '\0';
printf("Received: %s\n",buf);
string message(buf);
return message;
}


void get_info(string message){
int i=0;int j=0;
while(message[i+j]!=' '){
j++;
}
pwd_len = message[i+j+1]-'0';
hash_value=message.substr(i,j);
bnry_str=message.substr(i+j+3,3);
char temp;
temp = bnry_str[0];
bnry_str[0] = bnry_str[1];
bnry_str[1] =temp;
int part = message[i+j+7]-'0';
mypart(part);
// c1 = message[i+j+7];
// c2 = message[i+j+9];

return;
}


void decrypt( int sockfd){
    cout<<"started decrypt"<<endl;
    int n;
    n= pwd_len;
    string salt;
    salt[0]=hash_value[0];
    salt[1]=hash_value[1];

    string test="";
    string last=""  ;
    /**initialization**/
    
    for(int i=0;i<n;i++){
        test.push_back(c1);
        last.push_back(c2);

    }
bool found = false;
    while(test!=last){
    if(stop_decrypt){
        found =true;
                break;

    }
        if(hash_value==string(crypt(test.c_str(),salt.c_str()))){
            password=test;
            found = true;
            cout<<password<<endl;
            Send(password,sockfd);
            break;

        }
        // cout<<test<<endl;
        if(next(test)==-1){
        	break;
        }
    }

    if(!found){
    	if(!stop_decrypt)
        Send("!found",sockfd);
    }
    return;
}

void check_for_stop(int sockfd){
string stop = "stop";
string message = Recieve(sockfd);
//If server sends stop message then this thread stops the decryption process
if(strcmp(message.c_str(),stop.c_str())==0){
stop_decrypt =true;
Send("stoped",sockfd);
}


return;
}


int main(int arg_count, char *arg_value[])
{
    if (arg_count != 3) //Check if Input is valid
    {
        perror("Invalid Input!\nFormat : ./worker server ip/host-name> <server-port>\n");
        exit(1);
    }
    int sockfd = Connect(arg_value);

    string message; //To store the temporary messages involved in the communication
    message = "worker";
    Send(message, sockfd);
while(true){
    message  = Recieve(sockfd);
    cout<<"task:"<<message<<endl;
    get_info(message);
    Send("doing",sockfd);
    stop_decrypt = false;
    thread decryption(decrypt,sockfd);
    thread stop_check(check_for_stop,sockfd);
    // check_for_stop(sockfd);
     decryption.join();
    stop_check.join();
    }
    close(sockfd);

    return 0;
}
