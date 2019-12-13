#ifndef HHELP
#define HHELP

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void dumpRequest(string filename, string data)
{
    ofstream myfile;
    myfile.open(filename, std::ofstream::out | std::ofstream::trunc);
    myfile << data;
    myfile.close();
}

void printDump(string requestData)
{
    std::cout << "------\n";
    std::cout << requestData;
    std::cout << "------\n";
}

#endif