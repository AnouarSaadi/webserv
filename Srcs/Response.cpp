/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:49:51 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/07 16:46:02 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req, HttpServer &server /* , Server *serv */) : _status(-1),
                                                                            _request(req),
                                                                            _server(server),
                                                                            _responseMsg(""),
                                                                            _headers(""),
                                                                            _body(""),
                                                                            _indexPath(""),
                                                                            _autoIndexPage(""),
                                                                            _dirPath(""),
                                                                            _dr(""),
                                                                            _autoIndex(false),
                                                                            _notFound(false),
                                                                            _isLocation(false)
{
    this->_errors[200] = "OK";
    this->_errors[301] = "Moved Permanently";
    this->_errors[307] = "Temporary Redirect";
    this->_errors[400] = "Bad Request";
    this->_errors[403] = "Forbidden";
    this->_errors[404] = "Not Found";
    this->_errors[405] = "Not Allowed";
    this->_errors[411] = "Length Required";
    this->_errors[413] = "Payload Too Large";
    this->_errors[414] = "Uri Too Long";
    this->_errors[415] = "Unsupported Media Type";
    this->_errors[500] = "Internal Server Error";
    this->_errors[501] = "Not Implemented";
    this->_errors[502] = "Bad Gateway";
    this->_errors[504] = "Gateway Timeout";
    this->_errors[505] = "Http Version Not Supported";
    this->_status = this->_request.getStatusCode();

    // std::cout << "status >>>>> " << this->_status << std::endl;
}

Response::~Response()
{
}

void Response::clearAll()
{
    _status = 0;
    _server.clearAll();
    _request.clearRequest();
    _location.clearAll();
    _responseMsg.clear();
    _headers.clear();
    _body.clear();
    _indexPath.clear();
    _autoIndexPage.clear();
    _dirPath.clear();
    _dr.clear();
    _autoIndex = false;
    _notFound = false;
    _isLocation = false;
    _errors.clear();
    _dirContent.clear();
}

void Response::getErrorPage(std::string ErrorPagePath)
{
    std::ifstream indexFile(ErrorPagePath);
    if (indexFile)
    {
        std::cout << "ERROR PAGE PATH >>> VALID" << std::endl;
        std::ostringstream ss;
        ss << indexFile.rdbuf();
        _body = ss.str();
    }
}

void Response::manageErrorHeaders(int _status)
{
    time_t rawTime;
    std::string tm;

    this->_headers.clear();
    time(&rawTime);
    tm = ctime(&rawTime);
    tm.pop_back();
    this->_headers.append(this->_request.getStartLineVal("protocol"));
    this->_headers.append(" ");
    this->_headers.append(std::to_string(_status));
    this->_headers.append(" ");
    this->_headers.append(this->_errors[_status]);
    this->_headers.append("\r\n");
    this->_headers.append("Server: webServ\r\n");
    this->_headers.append("Date: " + tm.append(" GMT"));
    this->_headers.append("\r\n");
    this->_headers.append("Connection: keep-alive\r\n");
    this->_headers.append("Content-Type: text/html; charset=UTF-8");
    this->_headers.append("\r\n");
    this->_headers.append("Content-Length: " + std::to_string(_body.length()));
    this->_headers.append("\r\n\r\n");
    this->_headers.append(_body);
}

void Response::manageErrors()
{
    if (_status == BAD_REQUEST_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    else if (_status == HTTP_VERSION_NOT_SUPPORTED_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    else if (_status == NOT_ALLOWED_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    else if (_status == NOT_FOUND_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    else if (_status == FORBIDDEN_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    else if (_status == INTERNAL_SERVER_ERROR_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            _body = getDefaultErrorPage(_status);
    }
    manageErrorHeaders(_status);
}

void Response::readFile(std::string path)
{
    std::cout << "---------------> " << path << std::endl;
    if (access(path.c_str(), F_OK) != 0)
    {
        std::cout << "OK" << std::endl;
        _status = NOT_FOUND_STATUS;
        manageErrors();
    }
    else
    {
        if (access(path.c_str(), R_OK) == 0)
        {
            std::ifstream file(path);
            if (file)
            {
                std::ostringstream ss;
                ss << file.rdbuf();
                _body = ss.str();
                file.close();
            }
            else
            {
                _status = INTERNAL_SERVER_ERROR_STATUS;
                manageErrors();
            }
        }
        else
        {
            _status = FORBIDDEN_STATUS;
            manageErrors();
        }
    }
}

void Response::indexingFiles()
{
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << getPath(_request.getStartLineVal("url")) << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    DIR *directory = opendir(getPath(_request.getStartLineVal("url")).c_str());
    struct dirent *en;
    std::string fileName;

    _autoIndexPage = "<!DOCTYPE html>\n<html lang=\"en\">\n\
    <head>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
        <title>Bad Request</title>\n\
        </head>\n\
        <body>\n\
            <div style=\"margin-left: 5%; margin-top:10%;\">\n\
            <hr>\n";

    if (directory)
    {
        while ((en = readdir(directory)) != nullptr)
        {
            fileName = en->d_name;
            if (en->d_type == DT_DIR)
                fileName.append("/");
            _autoIndexPage.append("\t\t\t<p><a href=\"" + fileName + "\">" + fileName + "</a></p>\n");
        }
        closedir(directory);
    }
    _autoIndexPage += "\
                <hr>\n\
            </div>\n\
        </body>\n\
    </html>\n";

    std::cout << "+++++++++++++++++++++++++++++++++++++++++---------------------" << std::endl;
    std::cout << _autoIndexPage << std::endl;
    _body = _autoIndexPage;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++---------------------" << std::endl;
}

bool Response::isDirectory(const std::string &path)
{
    std::string s = path;
    DIR *r;

    if ((r = opendir(s.c_str())))
    {
        closedir(r);
        return true;
    }
    return false;
}

std::string &Response::getHeaders()
{
    return this->_headers;
}

std::string Response::getDefaultErrorPage(int status)
{
    std::string errorPage = "<!DOCTYPE html>\n\
    <html>\n\
        <head>\n\
            <meta charset=\"UTF-8\" />\n\
            <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n\
            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n\
            <title>Document</title>\n\
            <style>\n\
            .container {\n\
                margin: 10%;\n\
                text-align: center;\n\
                color: rgba(52, 151, 250, 0.705);\n\
            }\n\
            h1 {\n\
                font-size: 14rem;\n\
                font-family: \"Courier New\", Courier, monospace;\n\
                font-weight: bold;\n\
                margin:-5rem 0 0 0;\n\
            }\n\
            .parag {\n\
                margin:0;\n\
                font-weight: bold;\n\
                font-size: 5rem;\n\
                font-family: \"Courier New\", Courier, monospace;\n\
            }\n\
            </style>\n\
        </head>\n\
        <body>\n\
            <div class=\"container\">\n\
            <h1>$1</h1>\n\
            <p class=\"parag\">$2</p>\n\
            </div>\n\
        </body>\n\
    </html>";
    errorPage.replace(errorPage.find("$1"), 2, std::to_string(status));
    errorPage.replace(errorPage.find("$2"), 2, _errors[status]);
    return errorPage;
}

std::string &Response::getResponse()
{
    return this->_responseMsg;
}

std::string Response::getHtmlTemplate()
{
    std::string htmlTemplate = "<!DOCTYPE html>\n\
    <html>\n\
        <head>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
        <title>WEBSERV</title>\n\
        <style>\n\
            .container{margin:0;top:0;}\n\
            .error{font-size: 2.5rem;text-align: center;margin-top: 10%;color:dodgerblue;}\n\
        </style>\n\
        </head>\n\
        <body>\n\
            <div class=\"container\">\n\
                <div class=\"error\">\n\
                    <h1>Welcome to WeBsErVeReReReReR</h1>\n\
                </div>\n\
            </div>\n\
        </body>\n\
    </html>";

    return htmlTemplate;
}

Location Response::getRedirection(std::string locName)
{
    for (size_t i = 0; i < _server.getLocations().size(); i++)
    {
        if (_location.getLocationName().compare(locName) == 0)
        {
            return _server.getLocations()[i];
            break;
        }
    }
    return _location;
}

Location Response::isLocationExist()
{
    // std::cout << "locName |" << _server.getLocations()[i].getLocationName() << "| - isCGI |" << _server.getLocations()[i].getIsCGI() << "|" << std::endl;
    for (size_t i = 0; i < _server.getLocations().size(); i++)
    {
        if (_request.getStartLineVal("url").find(_server.getLocations()[i].getLocationName()) != std::string::npos)
        {
            _location = _server.getLocations()[i];
            _isLocation = true;
        }
    }
    if (_location.getReturn().size())
    {
        if (_location.getLocationName().compare(_request.getStartLineVal("url")) == 0)
        {
            if (_location.getReturn().begin()->first == MOVED_PERMANENTLY_STATUS)
            {
                _status = MOVED_PERMANENTLY_STATUS;
                _request.setStartLineVal("url", _location.getReturn().begin()->second);
                _redirectedLocation = _location.getReturn().begin()->second;
                _location.clearAll();
                return getRedirection(_location.getLocationName());
            }
        }
        else
            _body = getDefaultErrorPage(NOT_FOUND_STATUS);
    }
    return _location;
}

std::string Response::getUriFilePath(std::string uri)
{
    std::string path = uri;

    if (path.back() != '/')
        path.append("/");
    if ((!_location.getAutoIndex() && uri.compare(_location.getLocationName()) == 0 && _location.getIndex().size()) || path.compare(_location.getIndex()) == 0)
    {
        std::cout << uri << " | " << _location.getLocationName() << std::endl;
        return path.append(_location.getIndex());
    }
    else
        _body = getHtmlTemplate();
    return uri;
}

std::string Response::getRootDirectory()
{
    if (_location.getRoot().size())
        return _location.getRoot();
    return _server.getRoot();
}

std::string Response::getPath(std::string uriFilePath)
{
    std::string path = getRootDirectory();
    std::cout << "dir ___> |" << path << "|" << std::endl;
    if (uriFilePath.front() != '/')
        uriFilePath = "/" + uriFilePath;
    path.append(uriFilePath);
    return path;
}

void Response::getMethod()
{
    std::cout << "GET METHOD" << std::endl;
    std::string fileNameFromUri = getUriFilePath(_request.getStartLineVal("url"));
    std::string directoryPath = getPath(fileNameFromUri);
    std::cout << "uri_> |" << _request.getStartLineVal("url") << "|" << std::endl;
    std::cout << "fileNameFromUri _> |" << fileNameFromUri << "|" << std::endl;
    std::cout << "root _> |" << _server.getRoot() << "|" << std::endl;
    std::cout << "PATH _> |" << directoryPath << "|" << std::endl;

    if (isDirectory(directoryPath))
    {
        std::cout << "is Directory" << std::endl;
        if (_location.getAutoIndex())
            indexingFiles();
        else if (_isLocation && !_location.getIndex().size())
            _body = getHtmlTemplate();
        else
        {
            _status = NOT_FOUND_STATUS;
            manageErrors();
        }
    }
    else if (_isLocation && _request.getStartLineVal("url").compare(_location.getLocationName()) == 0)
    {
        if (_location.getIndex().size() && fileNameFromUri.find(_location.getIndex()) != std::string::npos)
        {
            std::cout << "IS INDEX" << std::endl;
            if (_location.getIndex().size())
            {
                std::cout << "ROOT && INDEX >>> EXIST" << std::endl;
                std::ifstream indexFile(getRootDirectory() + "/" + _location.getIndex());
                if (indexFile)
                {
                    std::cout << "INDEX PATH >>> VALID" << std::endl;
                    std::ostringstream ss;
                    ss << indexFile.rdbuf();
                    _body = ss.str();
                }
                else
                {
                    std::cout << "INDEX PATH >>> INVALID" << std::endl;
                    _body = getHtmlTemplate();
                }
            }
            else
            {
                std::cout << "NO INDEX IN THIS LOCATION" << std::endl;
                _body = getHtmlTemplate();
            }
        }
    }
    else
    {
        std::cout << "FILE READED" << std::endl;
        readFile(directoryPath);
    }
}

void Response::postMethod()
{
    std::cout << "POST METHOD" << std::endl;
}

void Response::deleteMethod()
{
    std::cout << "DELETE METHOD" << std::endl;
    std::string directoryPath = getPath(getUriFilePath(_request.getStartLineVal("url")));

    if (isDirectory(directoryPath))
    {
        _status = NOT_FOUND_STATUS;
        manageErrors();
    }
    else
    {
        if (access(directoryPath.c_str(), F_OK) != 0)
        {
            _status = NOT_FOUND_STATUS;
            manageErrors();
        }
        else
        {
            if (access(directoryPath.c_str(), W_OK) == 0)
            {
                if (std::remove(directoryPath.c_str()) != 0)
                {
                    _status = INTERNAL_SERVER_ERROR_STATUS;
                    manageErrors();
                }
                std::cout << "DELETED" << std::endl;
            }
            else
            {
                _status = FORBIDDEN_STATUS;
                manageErrors();
            }
        }
    }
}

void Response::buildHeaders()
{
    time_t rawTime;
    std::string tm;

    time(&rawTime);
    tm = ctime(&rawTime);
    tm.pop_back();
    this->_headers.append("HTTP/1.1");
    this->_headers.append(" ");
    this->_headers.append(std::to_string(_status));
    this->_headers.append(" ");
    this->_headers.append(this->_errors[_status]);
    this->_headers.append("\r\n");
    if (_status == MOVED_PERMANENTLY_STATUS)
    {
        this->_headers.append("Location: " + _redirectedLocation);
        this->_headers.append("\r\n");
        this->_headers.append("\r\n\r\n");
    }
    else
    {
        this->_headers.append("Server: webServ\r\n");
        this->_headers.append("Date: " + tm.append(" GMT"));
        this->_headers.append("\r\n");
        this->_headers.append("Connection: keep-alive");
        this->_headers.append("\r\n");
        this->_headers.append("Content-Type: text/html; charset=UTF-8");
        this->_headers.append("\r\n");
        this->_headers.append("Content-Length: " + std::to_string(_body.length()));
        this->_headers.append("\r\n\r\n");
        this->_headers.append(_body);
    }
}

void Response::generateResponse()
{
    _location = isLocationExist();
    if (_request.getStartLineVal("method").compare("GET") == 0)
        getMethod();
    else if (_request.getStartLineVal("method").compare("POST") == 0)
        postMethod();
    else if (_request.getStartLineVal("method").compare("DELETE") == 0)
        deleteMethod();
    buildHeaders();
}

void Response::buildResponse()
{
    // check for errors
    if (_status != OK_STATUS)
        manageErrors();
    else
        generateResponse();
}