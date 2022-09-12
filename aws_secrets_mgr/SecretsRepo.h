#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include <string>
#include "SecretsContainer.h"
#pragma GCC diagnostic pop

namespace gdt {

class SecretsRepo
{
public:

    ///
    // Returns true if successful, false (and error message) otherwise.
    // config format: <type>,<container_config>
    // Supported <type> values:
    // file: "Legacy" key file.  container_config should be path to keyfile.
    // awssm: AWS Secrets Manager.  container_config not used.
    ///
    bool initialize(const std::string& config,
                    std::string& errMsg);

    ///
    // Returns true if successful, false (and error message) otherwise.
    // If successful, stores retrieved value in out parameter.
    ///
    bool getSecret(const std::string& key,
                   std::string& value,
                   std::string& errMsg);

    // Utilities / Testing
    std::string         getTypeString() { return m_typeString; }
    bool                isInitialized() { return (m_container ? true : false); }

private:
    bool createContainer (const std::string& type, const std::string& config);

    std::string         m_typeString;
    SecretsContainerPtr m_container;
    const std::string   m_cfgSeparator = ",";
};



} // namespace gdt