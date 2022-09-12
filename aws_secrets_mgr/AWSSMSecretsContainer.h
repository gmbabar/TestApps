#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "SecretsContainer.h"
#include <aws/core/Aws.h>

#pragma GCC diagnostic pop

namespace gdt {

///
// Retrieves secrets from AWS Secrets Manager service.
// Uses ARN provided in key.
///
class AWSSMSecretsContainer : public SecretsContainer
{
public:
    AWSSMSecretsContainer();
    ~AWSSMSecretsContainer();

    bool initialize(const std::string& config,
                    std::string& errMsg) override;

    bool getSecret(const std::string& key,
                   std::string& value,
                   std::string& errMsg) override;

private:
    Aws::SDKOptions m_awsOptions;

};

} // namespace gdt