#include <string>
#include <map>


int main(int argc, char** argv) 
{
    printf("Content-Type: text/plain\n\n");

    // if (argc != 2)
    //     return 1;
    for (int i = 0; argv[1][i]; i++) {
        if (!isdigit(argv[1][i]))
            return 1;
    }
    int num = atoi(argv[1]);
    std::string res;
    std::map<int, std::string> roman; // = {{1,"I"},{4,"IV"}, {5, "V"}, {9, "IX"}, {10, "X"}, {40, "XL"}, {50, "L"}, {90, "XC"}, {100, "C"}, {400, "CD"}, {500, "D"}, {900, "CM"}, {1000, "M"}};
    roman[1] = "I";
    roman[4] = "IV";
    roman[5] = "V";
    roman[9] = "IX";
    roman[10] = "X";
    roman[40] = "XL";
    roman[50] = "L";
    roman[90] = "XC";
    roman[100] = "C";
    roman[400] = "CD";
    roman[500] = "D";
    roman[900] = "CM";
    roman[1000] = "M";
    std::map<int, std::string>::const_iterator it = roman.end();
    --it;
    while (num) {
        if (num >= (it->first)) {
            res += it->second;
            num -= it->first;
        }
         else
             --it;
    }
    printf("%s\n", res.c_str());
    return 0;
}