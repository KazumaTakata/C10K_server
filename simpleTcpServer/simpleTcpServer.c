#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "8889"

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

        char data[2000];

        ssize_t data_read = recv(newfd, &data, 2000, 0);

        printf("_____ \n %s \n ______ \n", data);

        printf("server: got connection from %s\n", inet_ntop(remoteaddr.ss_family,
                                                             get_in_addr((struct sockaddr *)&remoteaddr),
                                                             remoteIP, INET6_ADDRSTRLEN));

        char *body = "sdfkjbsfefewffwfwefwe foiwjf ofijew few";

        	
        /* declare a file pointer */
        FILE    *infile;
        char    *buffer;
        long    numbytes;
        
        /* open an existing file for reading */
        infile = fopen("index.html", "r");
        
        /* quit if the file does not exist */
        if(infile == NULL)
            return 1;
        
        /* Get the number of bytes */
        fseek(infile, 0L, SEEK_END);
        numbytes = ftell(infile);
        
        /* reset the file position indicator to 
        the beginning of the file */
        fseek(infile, 0L, SEEK_SET);	
        
        /* grab sufficient memory for the 
        buffer to hold the text */
        buffer = (char*)calloc(numbytes, sizeof(char));	
        
        /* memory error */
        if(buffer == NULL)
            return 1;
        
        /* copy all the text into the buffer */
        fread(buffer, sizeof(char), numbytes, infile);
        fclose(infile);
        
        /* confirm we have read the file by
        outputing it to the console */
        // printf("The file called test.dat contains this text\n\n%s", buffer);
        
        /* free the memory we used for the buffer */

        char message[1000];

        sprintf( message , "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer), buffer);

        free(buffer);

        // char *message = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 15\n\nsdfkjsdnbfkjbsf";


        if (send(newfd, message, strlen(message), 0) == -1)
            perror("send");
        close(newfd);
    }

    return 0;
}