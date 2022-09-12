#include "FileSecretsContainer.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace gdt {

bool FileSecretsContainer::initialize(const std::string& config, std::string& errMsg)
{
    fs::path filePath(config);

    if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
    {
        errMsg = "Could not find or load file: " + config;
        return false;
    }

    std::ifstream secFStream(config);
    std::ostringstream secBuf;
    secBuf << secFStream.rdbuf();

    m_fileContents = secBuf.str();

    return true;
}

bool FileSecretsContainer::getSecret(const std::string& key, std::string& value,
                                     std::string& errMsg)
{
    if (m_fileContents.empty())
    {
        errMsg = "Container empty";
        return false;
    }

    value = m_fileContents;
    return true;
}

} // namespace gdt