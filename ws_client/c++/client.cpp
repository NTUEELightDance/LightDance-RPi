#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;


int main() {
    
    char input[256] = {};
    // json input;

    //socket的建立
    int socket_client;
    socket_client = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_client == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;

    server_info.sin_port = htons(9002);
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");


    int connection_status = connect(socket_client, (struct sockaddr *) &server_info, sizeof(server_info));
    if(connection_status == -1){
        printf("Connection Error");
    }

    struct pollfd pollfds;  
    int timeout;  
    
    timeout = 5000;  
    pollfds.fd = socket_client;
    pollfds.events = POLLIN|POLLPRI;

    while(1){  
        bool quit = 0;
        switch(poll(&pollfds,1,timeout)){  
        case -1: 
            printf("poll error \r\n");  
        break;  
        case 0:  
            printf("time out \r\n");  
        break;  
        default: 
            printf("sockfd have some event \r\n");  
            //printf("event value is 0x%x",pollfds.revents);

            recv(socket_client, &input, sizeof(input),0);
            cout<<"message from server:\n"<<input<<endl;
            quit = 1;

            auto j3 = json::parse(input);
            auto tmp = j3["action"].get<std::string>();
            const char* action = tmp.data();
            cout << action << endl;

            // ofstream o("payload.json");
            // o << std::setw(4) << j3 << std::endl;

            int pipe_fd[2];
            pipe(pipe_fd); // child weite to parent read

            pid_t pid = fork();

            /****child process****/
            if(pid == 0){
                close(pipe_fd[0]); // close child read on pipe

                if(strcmp(action,"command") == 0){
                    auto tmp0 = j3["payload"][0].get<std::string>();
                    auto tmp1 = j3["payload"][1].get<std::string>();
                    auto tmp2 = j3["payload"][2].get<std::string>();
                    const char* command = tmp0.data();
                    const char* arg1 = tmp1.data();
                    const char* arg2 = tmp2.data();

                    cout << command << "\n" << arg1 << "\n" << arg2 << endl;

                    dup2(pipe_fd[1], STDOUT_FILENO);

                    int result = execl(command, arg1, arg2, NULL);

                    // write(pipe_fd[1], result, sizeof(result);

                    cout << result;
                    close(pipe_fd[1]);

                }
                else if(strcmp(action,"upload") == 0){
                    cout<<"upload"<<endl;
                }
                else if(strcmp(action,"sync") == 0){
                    cout<<"sync"<<endl;
                }
                
                exit(0);
            }
            /********************/

            /****parent process****/
            else{
                close(pipe_fd[1]); // close parent write on pipe
                dup2(pipe_fd[0], STDIN_FILENO);

                int result_from_child;
                // read(pipe_fd[0], result_from_child, sizeof(result_from_child));
                
                cin >> result_from_child;

                cout << result_from_child << endl;
                close(pipe_fd[0]);
                

                int status;
                wait(&status);
            }
            /**********************/

            break;  
        }
        break;
    }
    
    close(socket_client);
    
    return 0;
}