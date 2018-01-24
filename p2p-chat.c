/* 
 * File:   main.c
 * Author: tapin13
 *
 * Created on January 5, 2018, 11:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ERROR -1
#define PORT 51013

int main(int argc, char *argv[]) {
    int socket_file_descriptor;
    
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_file_descriptor == ERROR) {
        printf("Can't create socket file descriptor\n");
        return EXIT_FAILURE;
    }
    
    if(fcntl(socket_file_descriptor, F_SETFL, O_NONBLOCK, 1) == ERROR) {
        printf("Can't set socket NON BLOCK\n");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in local_socket_address;
    struct sockaddr_in remote_socket_address;
    socklen_t remote_socket_address_length = sizeof(remote_socket_address);
    
    if(argc > 1) {
        remote_socket_address.sin_family = AF_INET;
        remote_socket_address.sin_port = htons(PORT);
        remote_socket_address.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        local_socket_address.sin_family = AF_INET;
        local_socket_address.sin_port = htons(PORT);
        local_socket_address.sin_addr.s_addr = INADDR_ANY;
        if(bind(socket_file_descriptor, (struct sockaddr *) &local_socket_address, sizeof(local_socket_address)) == ERROR) {
            printf("Socket bind fail. Client mode.\n");
            return (EXIT_FAILURE);
        }
            
        printf("Socket bind success. Server mode. Listen on port %hu...\n", ntohs(local_socket_address.sin_port));
    }
    
    char send_buffer[255];
    int send_bytes = 0;
    char receive_buffer[255];
    int receive_bytes;
    int retval;

    fd_set in;
    
    unsigned char max_fd = socket_file_descriptor;
    
    memset(receive_buffer, 0, sizeof(receive_buffer));
    memset(send_buffer, 0, sizeof(send_buffer));
    
    while(1) {
        FD_ZERO(&in);
        FD_SET(0, &in);
        FD_SET(socket_file_descriptor, &in);

        retval = select(max_fd + 1, &in, NULL, NULL, NULL);
        //printf("retval: %d\n", retval);
                
        if(retval == -1) {
            printf("select()\n");
            return (EXIT_FAILURE);
        } else if(retval) {
            if(FD_ISSET(socket_file_descriptor, &in)) {
                //printf("data avaliable...\n");
                receive_bytes = recvfrom(socket_file_descriptor, receive_buffer, sizeof(receive_buffer), 0, (struct sockaddr*)&remote_socket_address, &remote_socket_address_length);
                
                if(strlen(receive_buffer) > 0) {
                    printf("Receive: %s\n", receive_buffer);
                    memset(receive_buffer, 0, sizeof(receive_buffer));
                }
            }
            
            if(FD_ISSET(0, &in)) {
                //printf("input avaliable...\n");

                fgets(send_buffer, sizeof(send_buffer), stdin);
                send_buffer[strlen(send_buffer)-1] = 0;
                send_bytes = sendto(socket_file_descriptor, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)&remote_socket_address, sizeof(struct sockaddr_in));
                memset(send_buffer, 0, sizeof(send_buffer));
            }
        } else {
            printf("timeout\n");
        }
    }
    
    close(socket_file_descriptor);
    
    return (EXIT_SUCCESS);
}

