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
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/time.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

#define TRUE    1
#define FALSE   0

int main() {
    
    char input[256] = {};
    int on = 1;

    //socket的建立
    int socket_client;
    socket_client = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_client == -1){
        printf("Fail to create a socket.");
    }

    // Allow socket descriptor to be reuseable
    int rc = setsockopt(socket_client, SOL_SOCKET,  SO_REUSEADDR,
                  (char *)&on, sizeof(on));
    if (rc < 0){
        perror("setsockopt() failed");
        close(socket_client);
        exit(-1);
    }

    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    rc = ioctl(socket_client, FIONBIO, (char *)&on);
    if (rc < 0){
        perror("ioctl() failed");
        close(socket_client);
        exit(-1);
    }

    // Bind the socket
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(9002);
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");

    rc = bind(socket_client, (struct sockaddr *) &server_info,sizeof(server_info));
    if (rc < 0){
        perror("bind() failed");
        close(socket_client);
        exit(-1);
    }

    // int connection_status = connect(socket_client, (struct sockaddr *) &server_info, sizeof(server_info));
    // if(connection_status == -1){
    //     printf("Connection Error");
    // }

    // Set the listen back log
    rc = listen(socket_client, 16);
    if (rc < 0){
        perror("listen() failed");
        close(socket_client);
        exit(-1);
    }

    // Initialize the pollfd structure
    struct pollfd fds[200];  
    int timeout;
    int nfds = 1, current_size = 0;
    int new_sd = -1;
    int close_conn;
    int end_server = FALSE, compress_array = FALSE;
    

    memset(fds, 0 , sizeof(fds));
    
    // Set up the initial listening socket
    fds[0].fd = socket_client;
    fds[0].events = POLLIN;

    timeout = (3 * 60 * 1000); // milliseconds

    do{
        // Call poll() and wait 3 minutes for it to complete.
        printf("Waiting on poll()...\n");
        rc = poll(fds, nfds, timeout);

        // Check to see if the poll call failed.
        if (rc < 0){
            perror("poll() failed");
            break;
        }

        // Check to see if the 3 minute time out expired.
        if (rc == 0){
        printf("poll() timed out.  End program.\n");
        break;
        }
        
        // One or more descriptors are readable.
        // Need to determine which ones they are.
        current_size = nfds;
        for (int i = 0; i < current_size; i++){
            // Loop through to find the descriptors that returned POLLIN 
            // and determine whether it's the listening or the active connection.                          
            if(fds[i].revents == 0)
                continue;

            // If revents is not POLLIN, it's an unexpected result,
            // log and end the server.
            if(fds[i].revents != POLLIN){
                printf("  Error! revents = %d\n", fds[i].revents);
                end_server = TRUE;
                break;
            }

            if (fds[i].fd == socket_client){
                // Listening descriptor is readable.
                printf("  Listening socket is readable\n");

                // Accept all incoming connections that are queued up on 
                // the listening socket before we loop back and call poll again.
                do{
                // Accept each incoming connection. If accept fails with EWOULDBLOCK, 
                // then we have accepted all of them. Any other failure on accept will
                // cause us to end the server.
                new_sd = accept(socket_client, NULL, NULL);
                if (new_sd < 0){
                    if (errno != EWOULDBLOCK){
                        perror("accept() failed");
                        end_server = TRUE;
                    }
                    break;
                }

                // Add the new incoming connection to the pollfd structure
                printf("New incoming connection - %d\n", new_sd);
                fds[nfds].fd = new_sd;
                fds[nfds].events = POLLIN;
                nfds++;

                // Loop back up and accept another incoming connection
                } while (new_sd != -1);
            }

            // This is not the listening socket,
            // therefore an existing connection must be readable
            else{
                printf("Descriptor %d is readable\n", fds[i].fd);
                close_conn = FALSE;
                // Receive all incoming data on this socket
                // before we loop back and call poll again.

                do{
                // Receive data on this connection until the recv fails with EWOULDBLOCK.
                // If any other failure occurs, we will close the connection.
                rc = recv(fds[i].fd, input, sizeof(input), 0);
                if (rc < 0){
                    if (errno != EWOULDBLOCK){
                        perror("recv() failed");
                        close_conn = TRUE;
                    }
                    break;
                }

                // Check to see if the connection has been closed by the client 
                if (rc == 0){
                    printf("Connection closed\n");
                    close_conn = TRUE;
                    break;
                }
                // Data was received
                int len = rc;
                printf("%d bytes received\n", len);
                cout<<"message from server:\n"<<input<<endl;

                auto message = json::parse(input);
                auto tmp = message["action"].get<std::string>();
                const char* action = tmp.data();
                cout << "action: " << action << endl;

                int pipe_fd[2];
                pipe(pipe_fd); // child weite to parent read

                pid_t pid = fork();

                /****child process****/
                if(pid == 0){
                    close(pipe_fd[0]); // close child read on pipe

                    if(strcmp(action,"command") == 0){
                        auto tmp0 = message["payload"][0].get<std::string>();
                        auto tmp1 = message["payload"][1].get<std::string>();
                        auto tmp2 = message["payload"][2].get<std::string>();
                        const char* command = tmp0.data();
                        const char* arg1 = tmp1.data();
                        const char* arg2 = tmp2.data();

                        cout << command << " " << arg1 << " " << arg2 << endl;

                        dup2(pipe_fd[1], STDOUT_FILENO);

                        int result = execl(command, arg1, arg2, NULL);

                        // write(pipe_fd[1], result, sizeof(result);

                        cout << result;
                        close(pipe_fd[1]);

                    }
                    else if(strcmp(action,"upload") == 0){
                        json payload_control = message["payload"];
                        ofstream o("payload.json");
                        o << std::setw(4) << payload_control << std::endl;
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

                // Echo the data back to the client
                // rc = send(fds[i].fd, input, len, 0);

                // if (rc < 0){
                //     perror("send() failed");
                //     close_conn = TRUE;
                //     break;
                // }

                } while(TRUE);

                // If the close_conn flag was turned on, we need to clean up 
                // this active connection. This clean up process includes 
                // removing the descriptor.
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }
            }  // End of existing connection is readable
        } // End of loop through pollable descriptors

        /***********************************************************/
        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        /***********************************************************/
        if (compress_array){
            compress_array = FALSE;
            for (int i = 0; i < nfds; i++){
                if (fds[i].fd == -1){
                    for(int j = i; j < nfds; j++){
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end_server == FALSE); /* End of serving running.    */

    // Clean up all of the sockets that are open
    for (int i = 0; i < nfds; i++){
        if(fds[i].fd >= 0)
        close(fds[i].fd);
    }
    
    return 0;
}