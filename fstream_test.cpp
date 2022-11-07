#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>


int main()
{
    std::ifstream fstream_object("hehe.txt");
    uint8_t byte;
    std::stringstream ss;
    if(fstream_object.fail())
    {
        std::cerr << "Fialed to open!" << std::strerror(errno) << std::endl;
        return 1;
    }
    // ss << fstream_object.rdbuf();
    while(!fstream_object.read())
    {
        fstream_object >> byte;
        std::cout << byte;
    }
    std::cout  << std::endl;
}