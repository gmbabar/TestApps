#include "SecretsRepo.h"
#include "SecretsContainer.h"
#include "FileSecretsContainer.h"
#include "AWSSMSecretsContainer.h"

namespace gdt {

bool SecretsRepo::initialize(const std::string& config, std::string& errMsg)
{
    // "Parse" config string.  Format should be:
    // <type>,<container_config>
    auto sepIdx = config.find(m_cfgSeparator);
    if (sepIdx == std::string::npos)
    {
        errMsg = "Config separator not found";
        return false;
    }

    auto containerType      = config.substr(0, sepIdx);
    auto containerConfig    = config.substr(sepIdx + 1);

    if (!createContainer(containerType, containerConfig))
    {
        errMsg = "Create failed for container type: \"" + containerType + "\", config: \"" + containerConfig + "\"";
        return false;
    }

    m_typeString = containerType;
    return true;
}

bool SecretsRepo::getSecret(const std::string& key, std::string& value,
                            std::string& errMsg)
{
    if (!m_container)
    {
        errMsg = "Not initialized";
        return false;
    }

    return m_container->getSecret(key, value, errMsg);
}

bool SecretsRepo::createContainer(const std::string& type, const std::string& config)
{
    if (type == "file")
    {
        auto fileSC =
            std::unique_ptr<FileSecretsContainer>(new FileSecretsContainer);

        std::string errMsg;
        if (!fileSC->initialize(config, errMsg))
        {
            return false;
        }

        m_container = std::move(fileSC);
        return true;
    }
    else if (type == "awssm")
    {
        auto awsSC =
            std::unique_ptr<AWSSMSecretsContainer>(new AWSSMSecretsContainer);

        std::string errMsg;
        if (!awsSC->initialize(config, errMsg))
        {
            return false;
        }

        m_container = std::move(awsSC);
        return true;
    }

    return false;
}

} // namespace gdt