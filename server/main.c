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






// this function parses data sent by client
request *  handle_client_request(int client_fd) {

    request * client_request= calloc(1,sizeof(request));
    client_request->commands_list = calloc(1,sizeof(command));
    client_request->response_text = calloc(1024,sizeof(char));
    client_request->response_status = -1;
    client_request->bytes_recv_from_client = recv(client_fd, client_request->client_input, sizeof(client_request->client_input), 0);

    switch( client_request->bytes_recv_from_client){

        case -1 :
            perror("Failed to receive DATA from client: ");

            client_request->response_status = -1;
            strcpy(client_request->response_text,"");
            break;

        case 0 :
            printf(" Connection closed by client\n");
            client_request->response_status = 0;
            strcpy(client_request->response_text,"Client closed connection");
            break ;



        default:
            // if client send data, try parse
            // number of properly (in case of any ) parsed commands
            client_request->no_of_parsed_commands = parse_xml(client_request, strlen(client_request->client_input));

            if(client_request->no_of_parsed_commands > 0){
                client_request->response_status = client_request->no_of_parsed_commands;
                process_request(client_request, get_config_option("bash_script_path"));

            }



            break;

    }
    // send response to client
    send(client_fd, client_request->response_text, 1024, 0);

    // clean xml lib structures;
    xmlCleanupParser();
    xmlDictCleanup();
    xmlCleanupGlobals();
    xmlCleanupMemory();
    return client_request;

}
int listeningSocket;
struct sockaddr_in srv_addr;

int connected_clients = 1;
int return_value, on = 1;
int poll_size, i = 0;
int new_client_fd = -1;



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


    server_listen(port, max_clients, poll_fds);


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

                for (i = 0; i < poll_size; i++) {
                    if (poll_fds[i].revents == 0)
                        continue;


                    if (poll_fds[i].revents != POLLIN) {
                        printf("  Error! revents = %d\n", poll_fds[i].revents);

                        break;

                    }
                    // Listening file descriptor
                    if (poll_fds[i].fd == listeningSocket) {
                        // i == 0 - listening socket
                        printf("  Listening socket is readable\n");

                        do {

                            new_client_fd = accept(listeningSocket, NULL, NULL);
                            // if maximum connections reached
                            if(connected_clients > max_clients){
                                strcpy(server_message,"Maximum conections reached, closing connection");
                                send(new_client_fd,server_message,strlen(server_message)*sizeof(char),0);
                                close(new_client_fd);
                                break;
                            }
                            else{
                                send(new_client_fd,server_message,strlen(server_message)*sizeof(char),0);

                               if (new_client_fd < 0) {

                                    if (errno != EWOULDBLOCK) {

                                        perror("Accept failed: ");

                                    }
                                    break;
                                }

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

    memset(&srv_addr,0,sizeof(srv_addr));
    srv_addr.sin_port = htons(port);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    listeningSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);

    if (listeningSocket == -1) {
        printf("%s\n", "Couldnt create socket");
    }
    return_value = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

    if (return_value < 0) {
        perror("setsockopt() failed");
        close(listeningSocket);
    }
    return_value = ioctl(listeningSocket,FIONBIO,(char*)&on);

    if (return_value < 0)
    {
        perror("ioctl() failed");
        close(listeningSocket);
        exit(-1);
    }


    if (bind(listeningSocket, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error: ");
        printf("%s\n", "Couldnt bind to");
        close(listeningSocket);
    }




    if (listen(listeningSocket,  max_clients) < 0) {
        puts("Failed to listen");
        close(listeningSocket);
        exit(-1);
    }
    memset(poll_fds, 0, sizeof(poll_fds));
        puts("Waiting for clients");
    poll_fds[0].fd = listeningSocket;
    poll_fds[0].events = POLLIN;

}
