#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <queue>
#include <list>
#include <stdexcept>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SIZE 256
using namespace std;






int bind_to_socket(int PortNum)
{
    if (PortNum < 1024) {
        perror("All ports below 1024 are reserved\nYou can use ports above 1024 upto 65535 provided there are not already in use\n");
        exit(1);
    }

    int sockfd; //to store the socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //Initialise Socket
    {
        perror("socket error");
        exit(1);
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt failed");
        exit(1);
    }
    struct sockaddr_in Serv_addr;
    Serv_addr.sin_family = AF_INET; // host byte order
    Serv_addr.sin_port = htons(PortNum); // short, network byte order
    Serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(Serv_addr.sin_zero), 8); // zero the rest of the struct
    if (bind(sockfd, (struct sockaddr*)&Serv_addr, sizeof(struct sockaddr)) == -1) {
        perror("binding socket to Port failed");
        exit(1);
    }
    return sockfd;
}
void Listen(int sockfd, int backlog)
{

    if (listen(sockfd, backlog) == -1) {
        perror("error occured while listening to the given port");
        exit(1);
    }
    return;
}

void Send(string message, int sockfd)
{
    //cout<<"sending message ... "<<endl;
    if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
        perror("sending message failed");
        close(sockfd);
        exit(0);
    }
}

string Recieve(int sockfd)
{
    int numbytes;
    char buf[SIZE];
    if ((numbytes = recv(sockfd, buf, SIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    //printf("Received: %s\n",buf);
    string message(buf);
    return message;
}

class request {
public:
    int user_sockfd;
    string req;
    request(int sockfd, string s)
    {
        user_sockfd = sockfd;
        req = s;
    };
};

class worker {
public:
    int worker_sockfd;
    int user_sockfd;
    worker(int sockfd)
    {
        worker_sockfd = sockfd;
        user_sockfd = -1;
    };
};

queue<request> requests;
list<worker> workers;

void create_reqs(int new_fd, string msg)
{
    string s;
    for (char i = '0'; i < '0' + 60; i++) {
        s = msg.substr(5, msg.size() - 5) + " " + i;
        request r(new_fd, s);
        requests.push(r);
    }
    return;
}

void stop_reqs_of_curr_user(list<worker>::iterator it)
{

    string temp;
    while (true) {
        if ((requests.front()).user_sockfd != it->user_sockfd)
            break;
        requests.pop();
    }

    for (list<worker>::iterator jt = workers.begin(); jt != workers.end(); jt++) {
        if (jt->user_sockfd == it->user_sockfd && (jt != it)) {
            Send("stop", jt->worker_sockfd);
             temp=Recieve(jt->worker_sockfd);
            jt->user_sockfd = -1;
        }
    }
    Send("stop", it->worker_sockfd);
               temp = Recieve(it->worker_sockfd);
        it->user_sockfd = -1;
}

int main(int arg_count, char* arg_value[])
{	string temp;
    while (!requests.empty())
        requests.pop();
    workers.clear();
    int max_no_connections = 8;
    if (arg_count != 2) //Check if Input is valid
    {
        perror("Invalid Input!\nFormat : ./server <server-port>\n");
        exit(1);
    }
    int sockfd = bind_to_socket(atoi(arg_value[1]));
    Listen(sockfd, 10);
    fd_set readfds;
    fd_set master;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    FD_ZERO(&readfds);
    FD_ZERO(&master);

    FD_SET(sockfd, &master);
    int max_fd = sockfd;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int new_fd;
    struct sockaddr_in their_addr;
    string message;
    while (1) {
        readfds = master;
        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &readfds)) {

            new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
            if (new_fd == -1) {
                printf("accepting connection from %s failed", inet_ntoa(their_addr.sin_addr));
                continue;
            }
           
            message = Recieve(new_fd);

            if (message == "worker") {
            	 if (new_fd > max_fd) {
                max_fd = new_fd;
            }
                cout << "worker connected" << endl;
                workers.push_back(worker(new_fd));
                FD_SET(new_fd, &master);
            }
            else {
                if (message.substr(0, 4) == "user") {
                    cout << "user connected" << endl;
                    create_reqs(new_fd, message);
                }
            }
        }

        for (list<worker>::iterator it = workers.begin(); it != workers.end(); it++) {
            if (FD_ISSET(it->worker_sockfd, &readfds)) {
                string worker_msg = Recieve(it->worker_sockfd);
                if(worker_msg == ""){
                        FD_CLR(it->worker_sockfd, &master); 
                        workers.erase(it);
                        it=workers.begin();
                        continue;
                }
                if(worker_msg == "stoped" || worker_msg == "doing"){
                	continue;
                }
                if (worker_msg == "!found") {
                    if (it->user_sockfd != -1) {
                        /* code */
                        Send("stop", it->worker_sockfd);
                        temp = Recieve(it->worker_sockfd);
                    }
                    it->user_sockfd = -1;
                }
                else {
                    //Send("stop",it->worker_sockfd);
                    cout << "found this password:" << worker_msg << endl;

                    Send(worker_msg, it->user_sockfd);
                    cout<<"message sent to user"<<endl;
                    //if(Send(worker_msg,it->user_sockfd)==-1)workers.erase(it);

                    /**stoping the works of all workers corresponding to the user whose password is found**/
                    stop_reqs_of_curr_user(it);
                }
            }
        }

        
        /**loop for assigning tasks**/
        for (list<worker>::iterator it = workers.begin(); it != workers.end(); it++) {
            if (requests.empty()) {
                break;
            }
            if (it->user_sockfd == -1) {
                it->user_sockfd = (requests.front()).user_sockfd;
                Send((requests.front()).req, it->worker_sockfd);
                
                temp = Recieve(it->worker_sockfd);
                requests.pop();
            }
        }
    }

    return 0;
}
