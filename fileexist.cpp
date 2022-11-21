#include <iostream>
#include <fstream>
#include <string>


bool fileExists (const std::string& f) {
    std::ifstream file(f.c_str());
    return (file.good());
}

int main()
{
    if(fileExists("LOL.EXE"))
        std::cout << "YES IT EXIST" << std::endl;
    else
        std::cout << "Nope IT DOSENT EXIST" << std::endl;
    
}