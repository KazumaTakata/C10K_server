#include "request.hpp"

std::map<std::string, std::string> parse_header(void *msg)
{
    std::map<std::string, std::string> http_request;
    char *head = (char *)msg;
    char *mid;
    char *tail = head;

    if (sizeof(msg) == 0)
    {
        return http_request;
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

    return http_request;
}
