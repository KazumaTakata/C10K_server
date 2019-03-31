#include "responce.hpp"

void http_responce(int fd, std::map<std::string, std::string> http_request)
{
    char message[1000];
    if (http_request["Path"] == "/" || http_request["Path"].substr(1) == "index.html")
    {
        std::string buffer = read_file("index.html");
        sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
    }
    else if (http_request["Path"].substr(1) == "main.css")
    {
        std::string buffer = read_file(http_request["Path"].substr(1));
        sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
    }
    else
    {
        std::string buffer = read_file(http_request["Path"].substr(1));
        sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(buffer.c_str()), buffer.c_str());
    }

    if (send(fd, message, strlen(message), 0) == -1)
        perror("send");
    close(fd);
}