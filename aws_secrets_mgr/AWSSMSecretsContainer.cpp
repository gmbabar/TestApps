#include "AWSSMSecretsContainer.h"
#include <aws/secretsmanager/SecretsManagerClient.h>
#include <aws/secretsmanager/model/GetSecretValueRequest.h>

namespace gdt {

AWSSMSecretsContainer::AWSSMSecretsContainer()
{
    // AWS SDK initialization and shutdown must be done on constructor /
    // destructor to ensure any resources (including threads) are cleaned up
    // on termination.
    Aws::InitAPI(m_awsOptions);
}

AWSSMSecretsContainer::~AWSSMSecretsContainer()
{
    Aws::ShutdownAPI(m_awsOptions);
}

bool AWSSMSecretsContainer::initialize(const std::string& config, std::string& errMsg)
{
    // Nothing to do here, really.  SDK init happens on constructor, and SM client
    // object created when the secret is requested.
    return true;
}

bool AWSSMSecretsContainer::getSecret(const std::string& key, std::string& value,
                                      std::string& errMsg)
{
    Aws::SecretsManager::SecretsManagerClient smClient;

    Aws::SecretsManager::Model::GetSecretValueRequest getReq;
    getReq.SetSecretId(key);

    auto outcome = smClient.GetSecretValue(getReq);

    if (!outcome.IsSuccess())
    {
        errMsg = outcome.GetError().GetMessage();
        return false;
    }

    value = outcome.GetResult().GetSecretString();
    return true;
}

} // namespace gdt