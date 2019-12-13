#ifndef REQUEST_H
#define REQUEST_H

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

using namespace std;

map<std::string, std::string> parse_header(void *msg);

enum Method
{
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
};

enum HttpVersion
{
    HTTP_09,
    HTTP_10,
    HTTP_11,
    HTTP_20,
};

typedef struct URI
{
    string uri;
} URI;

typedef struct RequestLine
{
    Method method;
    URI uri;
    HttpVersion version;
} RequestLine;

typedef struct Request
{
    RequestLine requestline;
} Request;

#endif