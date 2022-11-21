#include <iostream>
#include <dirent.h>



void    listDir(const char *dir_name)
{
    struct dirent *entityStruct;

    DIR *directory;
    directory = opendir(dir_name);
    std::string dirListPage;
    dirListPage.append("<html>\n");
    dirListPage.append("<head>\n");
    dirListPage.append("<title> Index of");
    dirListPage.append(dir_name);
    dirListPage.append("</title>\n");
    dirListPage.append("</head>\n");

    entityStruct = readdir(directory);
    while(entityStruct != NULL)
    {
        std::cout << entityStruct->d_name << std::endl;

        entityStruct = readdir(directory);
    }
    dirListPage.append("</html>\n");

}

int main()
{
    listDir(".");

    return (0);
}
          