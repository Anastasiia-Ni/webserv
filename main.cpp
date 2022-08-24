#include "inc/Webserv.hpp"

int main(int argc, char **argv) {
	if (argc == 1 || argc == 2) {
		try {
			std::string		config;
			ConfigParser	cluster;

			/* configuration file as argument or default path */
			config = (argc == 1 ? "config.txt" : argv[1]);
			cluster.createCluster(config);
			cluster.print();

			//runServers();	
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
    }
    else {
		std::cout << "error: wrong arguments" << std::endl;
		return (1);
	}
    return (0);
}
