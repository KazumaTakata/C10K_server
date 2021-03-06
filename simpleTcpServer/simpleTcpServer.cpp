#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "responce.hpp"
#include "file.hpp"
#include "request.hpp"
#include "help.hpp"

// std::map<std::string, std::string> http_request;

#define PORT "8889"
#define BUFFERSIZE 65507

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;
    fd_set read_fds;
    int fdmax;

    int listener;
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[256];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL)
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1)
    {
        perror("listen");
        exit(3);
    }

    printf("server: waiting for connections at port %s\n", PORT);

    while (1)
    {
        addrlen = sizeof remoteaddr;
        newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
        if (newfd == -1)
        {
            perror("accept");
            continue;
        }

        char data[BUFFERSIZE] = {'\0'};

        ssize_t data_read = recv(newfd, &data, BUFFERSIZE, 0);

        std::string requestData(data);

        dumpRequest("requestDump.txt", requestData);

        std::map<std::string, std::string> http_request = parse_header(data);

        std::cout << "path:" << http_request["Path"].substr(1) << std::endl;

        http_responce(newfd, http_request);
    }

    return 0;
}