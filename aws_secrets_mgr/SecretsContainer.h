#pragma once

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include <string>
#include <memory>

#pragma GCC diagnostic pop

namespace gdt {

class SecretsContainer
{
public:
    virtual ~SecretsContainer() {}

    virtual bool initialize(const std::string& config,
                            std::string& errMsg) = 0;

    virtual bool getSecret(const std::string& key,
                           std::string& value,
                           std::string& errMsg) = 0;
};

typedef std::unique_ptr <SecretsContainer> SecretsContainerPtr;

} // namespace gdt