#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _autoindex(false) {}

clsLocationConfig::~LocationConfig() {}


void clsLocationConfig::setPath(const std::string& path) {
    this->_path = path;
}

void clsLocationConfig::setRoot(const std::string& root) {
    this->_root = root;
}

void clsLocationConfig::setMethods(const std::vector<std::string>& methods) {
    this->_methods = methods;
}

void clsLocationConfig::setIndex(const std::queue< std::string > index) {
    this->_index = index;
}

void clsLocationConfig::setAutoindex(bool autoindex) {
    this->_autoindex = autoindex;
}

void clsLocationConfig::setReturnUrl(const std::string& url) {
    this->_return_url = url;
}

void clsLocationConfig::setUploadStore(const std::string& store) {
    this->_upload_store = store;
}

void clsLocationConfig::addCgi(const std::string& extension, const std::string& binPath) {
    this->_cgi_info[extension] = binPath;
}

void clsLocationConfig::addMethod(const std::string& method) {
    this->_methods.push_back(method);
}


const std::string& clsLocationConfig::getPath() const {
    return this->_path;
}

const std::string& clsLocationConfig::getRoot() const {
    return this->_root;
}

const std::vector<std::string>& clsLocationConfig::getMethods() const {
    return this->_methods;
}

const std::string& clsLocationConfig::getIndex() const {
    return this->_index;
}

bool clsLocationConfig::getAutoindex() const {
    return this->_autoindex;
}

const std::string& clsLocationConfig::getReturnUrl() const {
    return this->_return_url;
}

const std::string& clsLocationConfig::getUploadStore() const {
    return this->_upload_store;
}

const std::map<std::string, std::string>& clsLocationConfig::getCgiInfo() const {
    return this->_cgi_info;
}