/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/01 17:19:23 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "Server.hpp"
class Server;

struct Bodies
{
    std::string contentType;
    std::string contentLength;
    std::string body;
};

class Request
{
private:
    std::map<std::string, std::string> headers;
    // std::string method; // GET POST DELETE
    // std::string url;
    // std::string pVersion; // http/1.1
    // std::string host;
    // std::string userAgent;
    // std::string connection;
    // std::string contentType;
    // std::string contentLength;
    // std::string transferEncoding;
    // std::string boundry;
    std::string body;
    std::vector<Bodies> bodies;
    std::string content;

public:
    Request(); 
    // {
        // std::istringstream s(content);
        // while (getline(s, line)) {
            
        // }
    // }
    Request(std::string const &reqData);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void parseRequest();
    void printRequest();
};

#endif