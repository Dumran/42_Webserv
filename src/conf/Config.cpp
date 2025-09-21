/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:20:18 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 16:54:30 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conf/Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sys/stat.h>

Config::Config()
{
}

Config::~Config()
{
}

Server& Config::getDefault()
{
    return _default;
}

const std::vector<Server>& Config::getPossibleServers() const
{
    return possible_servers;
}

std::vector<Server>& Config::getPossibleServers()
{
    return possible_servers;
}

void Config::setDefault(const Server& server)
{
    _default = server;
}

void Config::setPossibleServers(const std::vector<Server>& servers)
{
    possible_servers = servers;
}

void Config::addServer(const Server& server)
{
    possible_servers.push_back(server);
}

std::string Config::trim_space(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isspace(*it))
        it++;
    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && std::isspace(*rit))
        rit++;
    return std::string(it, rit.base());
}

bool Config::validate_ip(const std::string& ip)
{
    std::istringstream iss(ip);
    std::string octet;
    int num = std::atoi(octet.c_str());
    int count = 0;
    while (std::getline(iss, octet, '.'))
    {
        if (octet.empty() || octet.size() > 3)
            return false;
        for (size_t i = 0; i < octet.size(); ++i)
        {
            if (!std::isdigit(octet[i]))
                return false;
        }
        if (octet.size() > 1 && octet[0] == '0')
            return false;
        if (num < 0 || num > 255)
            return false;
        ++count;
    }
    return count == 4;
}

static bool is_ignorable_line(const std::string &line) {
    return line.empty() || line[0] == '#';
}

static void accumulate_braces_from_line(const std::string &line,
                                        int &opens,
                                        int &closes)
{
    for (std::string::size_type i = 0; i < line.length(); ++i) {
        if (line[i] == '{')      ++opens;
        else if (line[i] == '}') ++closes;
    }
}

static void scan_file_for_braces(std::ifstream &file,
                                 int &opens,
                                 int &closes)
{
    std::string tab_line;
    while (std::getline(file, tab_line)) {
    std::string line = Config::trim_space(tab_line);
        if (is_ignorable_line(line)) continue;
        accumulate_braces_from_line(line, opens, closes);
    }
}

static bool report_mismatch_and_fail(const std::string &filename,
                                     int opens,
                                     int closes)
{
    std::cerr << "webserv: mismatched braces in " << filename << ": "
              << opens << " opening braces, " << closes << " closing braces" << std::endl;
    return false;
}


bool Config::check_braces(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    int open_braces  = 0;
    int close_braces = 0;

    if (!file.is_open())
    {
        std::cerr << "webserv: could not open configuration file \"" << filename
                  << "\": No such file or directory" << std::endl;
        return false;
    }

    scan_file_for_braces(file, open_braces, close_braces);
    file.close();

    if (open_braces != close_braces)
        return report_mismatch_and_fail(filename, open_braces, close_braces);

    return true;
}





static bool fail_and_clear(std::ifstream &file,
                           std::vector<Server> &servers)
{
    file.close();
    servers.clear();
    return false;
}

static void set_server_defaults(Server &srv)
{
    if (srv.getPort() == -1) srv.setPort(80);
    if (srv.getHost().empty()) srv.setHost("0.0.0.0");
    if (srv.getClientMaxBodySize() == 0) srv.setClientMaxBodySize(1024 * 1024);
    if (srv.getRootLocation() == -1) srv.setRootLocation(-1);
}

static bool start_server_block_if_needed(const std::string &line,
                                         Server &current_server,
                                         bool &in_server_block)
{
    const std::string server_start = "server {";
    if (line.find(server_start) != std::string::npos)
    {
        in_server_block = true;
        current_server = Server();
        current_server.setPort(-1);
        current_server.setHost("");
        current_server.setClientMaxBodySize(1024 * 1024);
        current_server.setRootLocation(-1);
        return true;
    }
    return false;
}

static bool end_server_block_if_needed(const std::string &line,
                                       const std::string &filename,
                                       int line_number,
                                       bool &in_server_block,
                                       bool in_location_block,
                                       Server &current_server,
                                       std::vector<Server> &servers)
{
    const std::string block_end = "}";
    if (line.find(block_end) != std::string::npos && in_server_block && !in_location_block)
    {
        set_server_defaults(current_server);
        in_server_block = false;
        servers.push_back(current_server);
        (void)filename; (void)line_number;
        return true;
    }
    return false;
}

static bool start_location_block_if_needed(const std::string &line,
                                           const std::string &filename,
                                           int line_number,
                                           bool in_server_block,
                                           bool &in_location_block,
                                           Location &current_location,
                                           std::ifstream &file,
                                           std::vector<Server> &servers)
{
    const std::string location_start = "location";
    if (line.find(location_start) != std::string::npos && in_server_block)
    {
        in_location_block = true;
        current_location = Location();

        std::string::size_type path_start = line.find("/");
        std::string::size_type path_end   = line.find("{");

        if (path_end != std::string::npos && path_end < line.length() - 1 && line[path_end + 1] == '}')
        {
            std::cerr << "webserv: unexpected \"}\" in " << filename << ":" << line_number << std::endl;
            fail_and_clear(file, servers);
            return false;
        }
        if (path_start == std::string::npos || path_end == std::string::npos)
        {
            std::cerr << "webserv: invalid location syntax in " << filename << ":" << line_number << std::endl;
            fail_and_clear(file, servers);
            return false;
        }

        std::string path = line.substr(path_start, path_end - path_start);
    current_location.setPath(Config::trim_space(path));
        current_location.setAutoIndex(false);
        current_location.setUploadLimit(0);
        return true;
    }
    return false;
}

static bool end_location_block_if_needed(const std::string &line,
                                         bool &in_location_block,
                                         Server &current_server,
                                         const Location &current_location)
{
    const std::string block_end = "}";
    if (line.find(block_end) != std::string::npos && in_location_block)
    {
        in_location_block = false;
        current_server.addLocation(current_location);
        return true;
    }
    return false;
}

static bool parse_listen(std::stringstream &ss,
                         const std::string &filename,
                         int line_number,
                         std::ifstream &file,
                         std::vector<Server> &servers,
                         Server &current_server)
{
    std::string port_str;
    std::string server_str;
    ss >> port_str;
    port_str = port_str.substr(0, port_str.find(";"));
    server_str = port_str.substr(0, port_str.find(":"));

    if (server_str != port_str)
    {
    if (!Config::validate_ip(server_str))
        {
            std::cerr << "webserv: invalid host in \"listen\" directive in " << filename << ":" << line_number << std::endl;
            return fail_and_clear(file, servers);
        }
        current_server.setHost(server_str);
        port_str = port_str.substr(port_str.find(":") + 1);
    }

    std::stringstream port_ss(port_str);
    int port;
    port_ss >> port;
    current_server.setPort(port);

    if (port_ss.fail() || !port_ss.eof())
    {
        std::cerr << "webserv: invalid port in  \"" << port_str << "\" (line " << line_number << ")" << std::endl;
        return fail_and_clear(file, servers);
    }

    std::string extra;
    if (ss >> extra)
    {
        std::cerr << "webserv: invalid number of arguments in \"listen\" directive in (line " << line_number << ")" << std::endl;
        return fail_and_clear(file, servers);
    }

    if (current_server.getPort() <= 0 || current_server.getPort() > 65535)
    {
        std::cerr << "webserv: invalid port number in  \"" << current_server.getPort() << "\" (line " << line_number << ")" << std::endl;
        return fail_and_clear(file, servers);
    }
    return true;
}

static bool parse_server_name(std::stringstream &ss,
                              Server &current_server)
{
    std::string name;
    while (ss >> name)
    {
        if (name.find(";") != std::string::npos)
        {
            name = name.substr(0, name.find(";"));
            current_server.addServerName(name);
            break;
        }
        current_server.addServerName(name);
    }
    return true;
}

static void read_size_token(std::stringstream &ss, std::string &size_out) {
    ss >> std::ws;
    std::getline(ss, size_out, ';');
}

static void trim_token_inplace(std::string &s) {
    std::string::size_type first = s.find_first_not_of(" \t");
    std::string::size_type last  = s.find_last_not_of(" \t");
    if (first != std::string::npos && last != std::string::npos)
        s = s.substr(first, last - first + 1);
    else
        s.clear();
}

static bool parse_numeric(std::string &num_str, unsigned long &value, char* &endptr) {
    value = std::strtoul(num_str.c_str(), &endptr, 10);
    return true;
}

static void apply_unit_multiplier(char unit, unsigned long &value) {
    if (unit == 'K' || unit == 'k')       value *= 1024UL;
    else if (unit == 'M' || unit == 'm')  value *= 1024UL * 1024UL;
    else if (unit == 'G' || unit == 'g')  value *= 1024UL * 1024UL * 1024UL;
}

static bool convert_size_to_bytes(std::string &size_trimmed,
                                  unsigned long &value,
                                  char* &endptr)
{
    const char lastch = size_trimmed[size_trimmed.length() - 1];

    if (lastch == 'K' || lastch == 'k' ||
        lastch == 'M' || lastch == 'm' ||
        lastch == 'G' || lastch == 'g')
    {
        size_trimmed.erase(size_trimmed.length() - 1);
        parse_numeric(size_trimmed, value, endptr);
        if (endptr != size_trimmed.c_str() && *endptr == '\0' && value > 0)
            apply_unit_multiplier(lastch, value);
        return true;
    }

    parse_numeric(size_trimmed, value, endptr);
    return true;
}


static bool parse_client_max_body_size(std::stringstream &ss,
                                       const std::string &filename,
                                       int line_number,
                                       std::ifstream &file,
                                       std::vector<Server> &servers,
                                       Server &current_server)
{
    std::string size;
    read_size_token(ss, size);
    trim_token_inplace(size);

    if (size.empty())
    {
        std::cerr << "webserv: invalid value \"\" in \"client_max_body_size\" directive in "
                  << filename << ":" << line_number << std::endl;
        return fail_and_clear(file, servers);
    }

    unsigned long value = 0;
    char* endptr = 0;
    const std::string original_size = size;

    convert_size_to_bytes(size, value, endptr);

    if (endptr == size.c_str() || *endptr != '\0' || value == 0)
    {
        std::cerr << "webserv: invalid value \"" << original_size << "\" in \"client_max_body_size\" directive in "
                  << filename << ":" << line_number << std::endl;
        return fail_and_clear(file, servers);
    }

    current_server.setClientMaxBodySize(value);
    return true;
}


static bool parse_error_page(std::stringstream &ss,
                             const std::string &filename,
                             int line_number,
                             std::ifstream &file,
                             std::vector<Server> &servers,
                             Server &current_server)
{
    int code;
    std::string path;
    ss >> code >> path;
    if (code < 100 || code > 599)
    {
        std::cerr << "webserv: invalid error_page code \"" << code << "\" in " << filename << ":" << line_number << std::endl;
        return fail_and_clear(file, servers);
    }
    current_server.addErrorPage(code, path.substr(0, path.find(";")));
    return true;
}

static void apply_root(Server &current_server,
                       Location &current_location,
                       const std::string &server_in_root,
                       bool in_location_block)
{
    if (in_location_block)
    {
        current_location.setRoot(server_in_root);
        if (current_location.getPath() == "/")
            current_server.setRootLocation(current_server.getLocations().size());
    }
    else
    {
        current_server.setServerInRoot(server_in_root);
    }
}

static bool parse_root(std::stringstream &ss,
                       Server &current_server,
                       Location &current_location,
                       bool in_location_block)
{
    std::string server_in_root;
    ss >> server_in_root;
    server_in_root = server_in_root.substr(0, server_in_root.find(";"));
    apply_root(current_server, current_location, server_in_root, in_location_block);
    return true;
}

static bool parse_location_add_header(std::stringstream &ss,
                                      Location &current_location)
{
    std::string add_header;
    std::string temp;
    ss >> add_header;
    if (add_header.find(";") == std::string::npos)
    {
        ss >> temp;
        add_header.append(" " + temp);
    }
    add_header = add_header.substr(0, add_header.find(";"));
    current_location.setAddHeader(add_header);
    return true;
}

static bool parse_location_methods(std::stringstream &ss,
                                   Location &current_location)
{
    std::string method;
    while (ss >> method)
    {
        if (method.find(";") != std::string::npos)
        {
            method = method.substr(0, method.find(";"));
            current_location.addMethod(method);
            break;
        }
        current_location.addMethod(method);
    }
    return true;
}

static bool parse_location_autoindex(std::stringstream &ss,
                                     const std::string &filename,
                                     int line_number,
                                     std::ifstream &file,
                                     std::vector<Server> &servers,
                                     Location &current_location)
{
    std::string value;
    ss >> value;
    value = value.substr(0, value.find(";"));
    if (value != "on" && value != "off")
    {
        std::cerr << "webserv: invalid value \"" << value << "\" in \"autoindex\" directive in "
                  << filename << ":" << line_number << std::endl;
        return fail_and_clear(file, servers);
    }
    current_location.setAutoIndex((value == "on"));
    return true;
}

static bool parse_location_index(std::stringstream &ss,
                                 Location &current_location)
{
    std::string index;
    ss >> index;
    index = index.substr(0, index.find(";"));
    current_location.setIndex(index);
    return true;
}

static bool parse_location_return(std::stringstream &ss,
                                  Location &current_location)
{
    std::string redirect;
    ss >> redirect;
    redirect = redirect.substr(0, redirect.find(";"));
    current_location.setRedirect(redirect);
    return true;
}

static bool parse_location_cgi_extension(std::stringstream &ss,
                                         Location &current_location,
                                         Server &current_server)
{
    std::string ext;
    ss >> ext;
    ext = ext.substr(0, ext.find(";"));
    current_location.setCgiExtension(ext);
    current_server.setCgiExtensionInServer(ext);
    return true;
}

static bool parse_location_cgi_path(std::stringstream &ss,
                                    Location &current_location,
                                    Server &current_server)
{
    std::string path;
    ss >> path;
    path = path.substr(0, path.find(";"));
    current_location.setCgiPath(path);
    current_server.setCgiPathInServer(path);
    return true;
}

static bool process_location_directive(const std::string &key,
                                       std::stringstream &ss,
                                       const std::string &filename,
                                       int line_number,
                                       std::ifstream &file,
                                       std::vector<Server> &servers,
                                       Server &current_server,
                                       Location &current_location)
{
    if (key == "root")
        return parse_root(ss, current_server, current_location, true);
    else if (key == "add_header")
        return parse_location_add_header(ss, current_location);
    else if (key == "methods")
        return parse_location_methods(ss, current_location);
    else if (key == "autoindex")
        return parse_location_autoindex(ss, filename, line_number, file, servers, current_location);
    else if (key == "index")
        return parse_location_index(ss, current_location);
    else if (key == "return")
        return parse_location_return(ss, current_location);
    else if (key == "cgi_extension")
        return parse_location_cgi_extension(ss, current_location, current_server);
    else if (key == "cgi_path")
        return parse_location_cgi_path(ss, current_location, current_server);

    std::cerr << "webserv: unknown directive \"" << key << "\" in " << filename << ":" << line_number << std::endl;
    return fail_and_clear(file, servers);
}

static bool process_server_directive_line(const std::string &line,
                                          const std::string &filename,
                                          int line_number,
                                          std::ifstream &file,
                                          std::vector<Server> &servers,
                                          Server &current_server,
                                          Location &current_location,
                                          bool in_location_block)
{
    std::stringstream ss(line);
    std::string key;
    ss >> key;

    if (key == "listen")
        return parse_listen(ss, filename, line_number, file, servers, current_server);
    else if (key == "server_name")
        return parse_server_name(ss, current_server);
    else if (key == "client_max_body_size")
        return parse_client_max_body_size(ss, filename, line_number, file, servers, current_server);
    else if (key == "error_page")
        return parse_error_page(ss, filename, line_number, file, servers, current_server);
    else if (key == "root")
        return parse_root(ss, current_server, current_location, in_location_block);
    else if (in_location_block)
        return process_location_directive(key, ss, filename, line_number, file, servers, current_server, current_location);

    std::cerr << "webserv: unknown directive \"" << key << "\" in " << filename << ":" << line_number << std::endl;
    return fail_and_clear(file, servers);
}

static bool validate_allowed_methods(const std::vector<Server> &servers,
                                     std::ifstream &file,
                                     std::vector<Server> &mutable_servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        const std::vector<Location> &locs = servers[i].getLocations();
        for (size_t x = 0; x < locs.size(); x++)
        {
            const std::vector<std::string> &methods = locs[x].getMethods();
            for (size_t j = 0; j < methods.size(); j++)
            {
                if (methods[j] != "GET" && methods[j] != "POST" && methods[j] != "DELETE")
                {
                    std::cerr << "webserv: unexpected methods" << std::endl;
                    file.close();
                    mutable_servers.clear();
                    return false;
                }
            }
        }
    }
    return true;
}


std::vector<Server> Config::parse(const std::string& filename)
{
    std::vector<Server> servers;

    Server   current_server;
    Location current_location;

    bool in_server_block = false;
    bool in_location_block = false;

    std::ifstream file(filename.c_str());
    std::string tab_line;
    int line_number = 0;

    if (!file.is_open())
    {
        std::cerr << "webserv: failed to open configuration file '" << filename
                  << "': no such file or directory" << std::endl;
        return servers;
    }

    while (std::getline(file, tab_line))
    {
        line_number++;
        std::string line = trim_space(tab_line);
        if (line.empty())
            continue;

        if (start_server_block_if_needed(line, current_server, in_server_block))
            continue;

        if (end_server_block_if_needed(line, filename, line_number, in_server_block, in_location_block, current_server, servers))
            continue;

        if (start_location_block_if_needed(line, filename, line_number, in_server_block, in_location_block, current_location, file, servers) == false)
        {
            if (servers.empty() && !in_server_block && !in_location_block && !file.good())
                return servers;
        }
        if (in_location_block && line.find("location") != std::string::npos)
            continue;

        if (end_location_block_if_needed(line, in_location_block, current_server, current_location))
            continue;

        if (in_server_block)
        {
            if (!process_server_directive_line(line, filename, line_number, file, servers, current_server, current_location, in_location_block))
                return servers;
        }
    }

    if (in_server_block || in_location_block)
    {
        std::cerr << "webserv: unexpected end of file, expecting \"}\" in "
                  << filename << ":" << line_number << std::endl;
        file.close();
        servers.clear();
        return servers;
    }

    if (!check_braces(filename))
    {
        std::vector<Server> empty_servers;
        return empty_servers;
    }

    file.close();

    if (servers.empty())
        return servers;

    std::ifstream dummy(filename.c_str());
    if (!validate_allowed_methods(servers, dummy, servers))
        return servers;

    return servers;
}


std::vector<Config> Config::set(std::vector<Server> &servers)
{
    std::vector<Config> grouped;

    std::map<int, int> port_counts;
    for (size_t i = 0; i < servers.size(); ++i)
        ++port_counts[servers[i].getPort()];

    for (std::map<int, int>::const_iterator it = port_counts.begin(); it != port_counts.end(); ++it) {
        if (it->second > 1) {
            std::cerr << "webserv: conflicting listen directive, multiple server blocks use port " << it->first << std::endl;
            std::vector<Config> empty;
            return empty;
        }
    }

    while (!servers.empty()) {
        Server base = servers.front();
        Config group;
        group.setDefault(base);
        group.possible_servers.insert(group.possible_servers.begin(), base);
        servers.erase(servers.begin());
        grouped.push_back(group);
    }
    return grouped;
}
