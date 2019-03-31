#ifndef RESPONCE_H
#define RESPONCE_H

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
#include "./file.hpp"

void http_responce(int fd, std::map<std::string, std::string> http_request);

#endif