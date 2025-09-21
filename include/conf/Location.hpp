/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:24 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:25 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

class Location
{

private:
    std::string path;
    std::string root;
    std::vector<std::string> methods;
    bool auto_index;
    std::string index;
    std::string redirect;
    std::string cgi_extension;
    std::string cgi_path;
    std::string upload_root;
    std::string add_header;
    unsigned long upload_limit;

public:
    Location();
    ~Location();

    const std::string &getPath() const;
    const std::string &getRoot() const;
    const std::vector<std::string> &getMethods() const;
    bool getAutoIndex() const;
    const std::string &getIndex() const;
    const std::string &getRedirect() const;
    const std::string &getCgiExtension() const;
    const std::string &getCgiPath() const;
    const std::string &getUploadRoot() const;
    const std::string &getAddHeader() const;
    unsigned long getUploadLimit() const;

    void setPath(const std::string &path);
    void setRoot(const std::string &root);
    void setMethods(const std::vector<std::string> &methods);
    void addMethod(const std::string method);
    void setAutoIndex(bool auto_index);
    void setIndex(const std::string &index);
    void setRedirect(const std::string &redirect);
    void setCgiExtension(const std::string &cgi_extension);
    void setCgiPath(const std::string &cgi_path);
    void setUploadRoot(const std::string &upload_root);
    void setAddHeader(const std::string &add_header);
    void setUploadLimit(unsigned long upload_limit);

};
