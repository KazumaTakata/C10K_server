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

std::map<std::string, std::string> http_request;

#define PORT "8889"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void parse_header(void *msg)
{
    char *head = (char *)msg;
    char *mid;
    char *tail = head;

    if (sizeof(msg) == 0)
    {
        return;
    }

    // Find request type
    while (*head++ != ' ')
        ;
    http_request["Type"] = std::string((char *)msg).substr(0, (head - 1) - tail);

    // Find path
    tail = head;
    while (*head++ != ' ')
        ;
    http_request["Path"] = std::string((char *)msg).substr(tail - (char *)msg, (head - 1) - tail);

    // Find HTTP version
    tail = head;
    while (*head++ != '\r')
        ;
    http_request["Version"] = std::string((char *)msg).substr(tail - (char *)msg, (head - 1) - tail);

    // Map all headers from a key to a value
    while (true)
    {
        tail = head + 1;
        while (*head++ != '\r')
            ;
        mid = strstr(tail, ":");

        // Look for the failed strstr
        if (tail > mid)
            break;

        http_request[std::string((char *)msg).substr(tail - (char *)msg, (mid)-tail)] = std::string((char *)msg).substr(mid + 2 - (char *)msg, (head - 3) - mid);
    }

    // std::cout << std::endl;

    // for (std::map<std::string, std::string>::iterator it = http_request.begin(); it != http_request.end(); ++it)
    // {
    //     std::cout << it->first << ":" << http_request[it->first] << std::endl;
    // }

    // std::cout << std::endl;
}

std::string read_file(std::string path)
{
    /* open an existing file for reading */
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    // std::cout << buffer.str();

    return buffer.str();
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

        char data[2000];

        ssize_t data_read = recv(newfd, &data, 2000, 0);

        parse_header(data);

        std::cout << "path:" << http_request["Path"].substr(1) << std::endl;

        // std::string buffer = read_file(http_request["Path"].substr(1));

        std::string buffer = read_file(http_request["Path"].substr(1));

        char message[1000];

        if (http_request["Path"].substr(1) == "index.html")
        {
            sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
        }
        else if (http_request["Path"].substr(1) == "main.css")
        {
            sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
        }
        else
        {
            sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
        }

        if (send(newfd, message, strlen(message), 0) == -1)
            perror("send");
        close(newfd);
    }

    return 0;
}