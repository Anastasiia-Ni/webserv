#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location
{
    private:
        std::string         _path;
        std::string         _root;
        bool                _autoindex;
        std::string         _index;
		std::string			_cgi_pass;

    public:
        std::vector<short>  _methods; // GET+ POST- DELETE-
        Location();
        Location(const Location &other);
        Location &operator=(const Location &rhs);
        ~Location();

		void setRoot(std::string parametr);
		void setMethods(std::vector<std::string> methods);
		void setAutoindex(std::string parametr);
		void setIndex(std::string parametr);
		void setCgiPass(std::string parametr);

		const std::string &getPath() const;
		const std::string &getRoot() const;
		const std::string &getIndex() const;
		const std::string &getCgiPass() const;
		const std::vector<short> &getMethods() const;
		const bool &getAutoindex() const;

		std::string getPrintMethods(); // for checking only

};

#endif
