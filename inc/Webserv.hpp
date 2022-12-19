#pragma once
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
// # include <fstream>
# include <fcntl.h>
# include <cstring>
# include <string> 
# include <unistd.h>
# include <dirent.h>
# include <sstream>
// # include <bits/stdc++.h>
# include <cstdlib>
# include <fstream>
# include <sstream>
# include <cctype>
# include <ctime>
# include <cstdarg>

/* STL Containers */
# include <map>
# include <set>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>

/* System */
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <unistd.h>
// # include <machine/types.h>
# include <signal.h>

/* Network */
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include "ConfigParser.hpp"
# include "ConfigFile.hpp"
# include "ServerConfig.hpp"
# include "Location.hpp"
# include "CgiHandler.hpp"
# include "Mime.hpp"
# include "Logger.hpp"


#define CONNECTION_TIMEOUT 60 // Time in seconds before client get kicked out if no data was sent.
#define MESSAGE_BUFFER 4096 
#define MAX_URI_LENGTH 4096
#define MAX_CONTENT_LENGTH 30000000


/* Utils.c */
std::string statusCodeString(short);
std::string getErrorPage(short);
int buildHtmlIndex(std::string &, std::vector<uint8_t> &, size_t &);


#endif