# Webserv

Develop a server in C++.

///here paste the screenshot of the log from the terminal

///insert project scheme here

## About project: 

/// write about the main idea with listening sockets and so on

This project was code for MacOS.

The program is working with a configuration file default or pass by the argument.
You can setup the port, the host, the allowed methods and some other basic paramaters for a webserver.

The program capable of running an HTTP server, handling requests, and parsing configuration files. 

The server handles the following HTTP features: 
- the basics of HTTP protocol;
- support GET, POST and DELETE methods;
- HTTP response status codes;
- error pages for the appropriate status code (by default or pass in the configuration file);
- handle multiply CGI (extensions *.py and *.sh);
- cookies and  session management.

#### Configuration file

We were inspired by the "server" part of NGINX configuration file according to the subject.

The format of our configuration file is [here](https://github.com/Anastasiia-Ni/.../format_config_file.conf)

#### CGI (Common Gateway Interface) 
In computing, Common Gateway Interface (CGI) is an interface specification that enables web servers to execute an external program, typically to process user requests.
It written in a scripting language (CGI scripts), but they may include compiled programs.

To run CGI using fork().

A model showing how CGI works:
![](https://github.com/Anastasiia-Ni/.../CGI.jpg)

## Index:
- contains headers
- contains the source code of the program
- CGI scripts
- Website 

## Installation:
```
$ git clone git@github.com:Anastasiia-Ni/
$ cd webserv
$ make
```

## Usage:
- `./webserv`    - take default basic configuration files

- `./webserv conf.conf`    - take path to a configuration file

## Resources:

Webserv on C 

- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

HTTP

- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- RFC 2616 https://www.rfc-editor.org/rfc/rfc2616.html#section-4
- https://developer.mozilla.org/en-US/docs/Web/HTTP
- RFC 9112 https://www.rfc-editor.org/rfc/rfc9112.html

Nginx
- https://nginx.org/en/
- https://github.com/nginx/nginx
- Interviews with Igor Sysoev

Sockaddr

CGI
- RFC 3875 https://datatracker.ietf.org/doc/html/rfc3875
- https://www.youtube.com/watch?v=_j5spdsJdV8&t=562s
- https://www.tutorialspoint.com/python/python_cgi_programming.htm
- https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm

Boundary

- https://www.youtube.com/watch?v=_j5spdsJdV8&t=562s

Cookies

- RFC 6265


Will delete after submition
- [Checklist for evaluation](https://github.com/mharriso/school21-checklists/blob/master/ng_5_webserv.pdf)
