#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "SecretsContainer.h"

#pragma GCC diagnostic pop

namespace gdt {

///
// Simple secrets container that loads file contents and returns them on
// getSecret regardless of the key.
///
class FileSecretsContainer : public SecretsContainer
{
public:

    bool initialize(const std::string& config,
                    std::string& errMsg) override;

    bool getSecret(const std::string& key,
                   std::string& value,
                   std::string& errMsg) override;

private:
    std::string m_fileContents  = "";
};

} // namespace gdt