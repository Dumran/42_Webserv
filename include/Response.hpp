/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:08 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:09 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "http/http.hpp"
#include "conf/Server.hpp"

class Response
{

private:
    std::string form_data;
    std::string file;
    std::string pure_link;
    RequestType request_type;
    int response_code;
    std::string content;
    bool is_cgi;
    std::string cgi_path;
    std::string cgi_extension;
    size_t content_length;
    std::string content_type;
    bool is_chunked;
    std::string response_code_str;
    bool method_not_allowed;
    bool auto_index;
    std::string add_header;
    std::vector<std::string> methods;
    std::string redirect;
    std::string cgi_root;

public:
    Response();
    ~Response();

    std::string getFormData() const;
    std::string getFile() const;
    std::string getPureLink() const;
    RequestType getRequestType() const;
    int getResponseCode() const;
    std::string getContent() const;
    bool getIsCgi() const;
    std::string getCgiPath() const;
    std::string getCgiExtension() const;
    size_t getContentLength() const;
    std::string getContentType() const;
    bool getIsChunked() const;
    std::string getResponseCodeStr() const;
    bool getMethodNotAllowed() const;
    bool getAutoIndex() const;
    std::string getAddHeader() const;
    std::vector<std::string> getMethods() const;
    std::string getRedirect() const;
    std::string getCgiRoot() const;

    void setFormData(const std::string &formData);
    void setFile(const std::string &file);
    void setPureLink(const std::string &pureLink);
    void setRequestType(RequestType requestType);
    void setResponseCode(int responseCode);
    void setContent(const std::string &content);
    void setIsCgi(bool isCgi);
    void setCgiPath(const std::string &cgiPath);
    void setCgiExtension(const std::string &cgiExtension);
    void setContentLength(size_t contentLength);
    void setContentType(const std::string &contentType);
    void setIsChunked(bool isChunked);
    void setResponseCodeStr(const std::string &responseCodeStr);
    void setMethodNotAllowed(bool methodNotAllowed);
    void setAutoIndex(bool autoIndex);
    void setAddHeader(const std::string &addHeader);
    void setMethods(const std::vector<std::string> &methods);
    void setRedirect(const std::string &redirect);
    void setCgiRoot(const std::string &cgiRoot);

    void configureFile(std::string file, Server &server);
};
