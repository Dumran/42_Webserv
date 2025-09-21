/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:20:25 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:20:26 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf/Location.hpp"

Location::Location() : auto_index(false), upload_limit(0)
{
}

Location::~Location()
{
}

const std::string &Location::getPath() const
{
    return path;
}

const std::string &Location::getRoot() const
{
    return root;
}

const std::vector<std::string> &Location::getMethods() const
{
    return methods;
}

bool Location::getAutoIndex() const
{
    return auto_index;
}

const std::string &Location::getIndex() const
{
    return index;
}

const std::string &Location::getRedirect() const
{
    return redirect;
}

const std::string &Location::getCgiExtension() const
{
    return cgi_extension;
}

const std::string &Location::getCgiPath() const
{
    return cgi_path;
}

const std::string &Location::getUploadRoot() const
{
    return upload_root;
}

const std::string &Location::getAddHeader() const
{
    return add_header;
}

unsigned long Location::getUploadLimit() const
{
    return upload_limit;
}

void Location::setPath(const std::string &path)
{
    this->path = path;
}

void Location::setRoot(const std::string &root)
{
    this->root = root;
}

void Location::setMethods(const std::vector<std::string> &methods)
{
    this->methods = methods;
}

void Location::addMethod(const std::string method)
{
    this->methods.push_back(method);
}

void Location::setAutoIndex(bool auto_index)
{
    this->auto_index = auto_index;
}

void Location::setIndex(const std::string &index)
{
    this->index = index;
}

void Location::setRedirect(const std::string &redirect)
{
    this->redirect = redirect;
}

void Location::setCgiExtension(const std::string &cgi_extension)
{
    this->cgi_extension = cgi_extension;
}

void Location::setCgiPath(const std::string &cgi_path)
{
    this->cgi_path = cgi_path;
}

void Location::setUploadRoot(const std::string &upload_root)
{
    this->upload_root = upload_root;
}

void Location::setAddHeader(const std::string &add_header)
{
    this->add_header = add_header;
}

void Location::setUploadLimit(unsigned long upload_limit)
{
    this->upload_limit = upload_limit;
}
