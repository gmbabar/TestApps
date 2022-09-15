#include "SecretManager.hpp"

int main() {
    std::stringstream command;
    command << "aws secretsmanager get-secret-value --secret-id DeribitApiKeys";
    SecretManager secMang(command);
    std::cout << secMang.GetSecretID() << std::endl;
    std::cout << secMang.GetSecret() << std::endl;
}