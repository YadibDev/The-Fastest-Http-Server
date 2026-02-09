#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _autoindex(false) {}

LocationConfig::~LocationConfig() {}


void LocationConfig::setPath(const std::string& path) {
    this->_path = path;
}

void LocationConfig::setRoot(const std::string& root) {
    this->_root = root;
}

void LocationConfig::setMethods(const std::vector<std::string>& methods) {
    this->_methods = methods;
}

void LocationConfig::setIndex(const std::string& index) {
    this->_index = index;
}

void LocationConfig::setAutoindex(bool autoindex) {
    this->_autoindex = autoindex;
}

void LocationConfig::setReturnUrl(const std::string& url) {
    this->_return_url = url;
}

void LocationConfig::setUploadStore(const std::string& store) {
    this->_upload_store = store;
}

void LocationConfig::addCgi(const std::string& extension, const std::string& binPath) {
    this->_cgi_info[extension] = binPath;
}

void LocationConfig::addMethod(const std::string& method) {
    this->_methods.push_back(method);
}


const std::string& LocationConfig::getPath() const {
    return this->_path;
}

const std::string& LocationConfig::getRoot() const {
    return this->_root;
}

const std::vector<std::string>& LocationConfig::getMethods() const {
    return this->_methods;
}

const std::string& LocationConfig::getIndex() const {
    return this->_index;
}

bool LocationConfig::getAutoindex() const {
    return this->_autoindex;
}

const std::string& LocationConfig::getReturnUrl() const {
    return this->_return_url;
}

const std::string& LocationConfig::getUploadStore() const {
    return this->_upload_store;
}

const std::map<std::string, std::string>& LocationConfig::getCgiInfo() const {
    return this->_cgi_info;
}