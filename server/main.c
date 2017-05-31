#include <stdio.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <zconf.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/ioctl.h>

#include "request_handler.h"
#include "configuration.h"
#include <errno.h>







struct sockaddr_in srv_addr;

int connected_clients = 1;
int return_value, on = 1;
int poll_size, i = 0;




int main(int argc, char **argv) {

    // config_load_failure return 1 if file is not readable, file not exist or runing application without config attribute
    if(config_load_failure(argv[1])){
        return -1;
    }


    // get some config data //
    char * server_message  = get_config_option("welcome_message");
    int max_clients = atoi(get_config_option("max_clients"));
    int port = atoi(get_config_option("port"));
    int poll_timeout =atoi(get_config_option("poll_timeout"));

    struct pollfd poll_fds[max_clients];


    int listeningFD = server_listen(port, max_clients, poll_fds);
    int new_client_fd = -1;

    // main server loop
    do {

                printf("Waiting on events \n");

                int poll_status = poll(poll_fds, connected_clients, poll_timeout);
                if (poll_status < 0) {
                    perror("Poll failed");
                    break;
                }


                if (poll_status == 0) {
                    printf("Poll timed out.  End program.\n");
                    break;
                }

                poll_size = connected_clients;

                for (int i = 0; i <= poll_size; i++) {
                    if (poll_fds[i].revents == 0)
                        continue;


                    if (poll_fds[i].revents != POLLIN) {
                        printf("  Error! revents = %d\n", poll_fds[i].revents);

                        break;

                    }
                    // Listening file descriptor
                    if (poll_fds[i].fd == listeningFD) {
                        // i == 0 - listening socket
                        printf("  Listening socket is readable\n");

                        do {

                            new_client_fd = accept(listeningFD, NULL, NULL);
                            if(new_client_fd == -1) break;
                            // if maximum connections reached
                            if(connected_clients > max_clients){
                                strcpy(server_message,"Maximum conections reached, closing connection");
                                send(new_client_fd,server_message,strlen(server_message)*sizeof(char),0);
                                close(new_client_fd);
                                break;
                            }
                            else{

                                send(new_client_fd,server_message,strlen(server_message)*sizeof(char),0);

                               

                                // i > 1 -> connected clients, add client to pollfd structure
                                printf("  New incoming connection - %d\n", new_client_fd);

                                poll_fds[connected_clients].fd = new_client_fd;
                                poll_fds[connected_clients].events = POLLIN;

                                connected_clients++;
                            }

                        } while (new_client_fd != -1);

                    }


                    // Work only for connected CLIENTS
                    else {
                        while(1){
                            // Handle  client request
                            request*  req = handle_client_request(poll_fds[i].fd);

                            if(req->bytes_recv_from_client <=0 ){
                                free_client_request(req);
                                close(poll_fds[i].fd);

                                poll_fds[i].fd = -1;
                                connected_clients--;
                                break;
                            }

                            free_client_request(req);
                            break;

                        }

                    }
                }

    } while (1);

    destroy_configuration();
    return (0);
    }


int server_listen(int port, int max_clients, struct pollfd *poll_fds) {
    int fd  = -1;
    memset(&srv_addr,0,sizeof(srv_addr));
    srv_addr.sin_port = htons(port);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);

    if (fd == -1) {
        printf("%s\n", "Couldnt create socket");
    }
    return_value = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

    if (return_value < 0) {
        perror("setsockopt() failed");
        close(fd);
    }
    return_value = ioctl(fd,FIONBIO,(char*)&on);

    if (return_value < 0)
    {
        perror("ioctl() failed");
        close(fd);
        exit(-1);
    }


    if (bind(fd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error: ");
        printf("%s\n", "Couldnt bind to");
        close(fd);
    }




    if (listen(fd,  max_clients) < 0) {
        puts("Failed to listen");
        close(fd);
        exit(-1);
    }
    memset(poll_fds, 0, max_clients);
        puts("Waiting for clients");
    poll_fds[0].fd = fd;
    poll_fds[0].events = POLLIN;
    return  fd;
}
