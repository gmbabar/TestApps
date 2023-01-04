// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

void ParseKeys(const std::string &exchId) {
    const std::string awsSecretId = "test/dmatest";
    const std::string awsRegion = "us-east-1";
//     const char* json = R"(
// {
//     "ARN": "arn:aws:secretsmanager:us-east-1:521170775997:secret:test/dmatest-fAOZPd",
//     "Name": "test/dmatest",
//     "VersionId": "b9c4504e-21a0-42d6-8159-a46cc7822544",
//     "SecretString": "{\"BMEX\":{\"api_key\":\"Ga0hRRTcuVjAQKecknmc8eVs\",\"secret_key\":\"-jW8NSh-9qI4JvKUsOA7liwkPasCS0YawVbtUMuyPvicQ6TZ\"},\"GEM\":{\"api_key\":\"account-DOg0jQYwTa5cqWAcMjmY\",\"secret_key\":\"xFf2j6QzYMRAt5MsMztGgMGi41\"},\"GDAX\":{\"api_key\":\"2eeb89146a290c312150cb7551358e31\",\"secret_key\":\"X1CYzciXDsnQ2BNvBs6GBnRTZkDjULjV0V2STvGMb67A0mOkmph6z2rXB8E4QRufrV9pRxyaajhOBQdcLM3fzA==\",\"passphrase\":\"0t88evo1ikrk\"},\"HUOBI\":{\"api_key\":\"34d811df-289a1500-dqnh6tvdf3-3a175\",\"secret_key\":\"54635bfc-0a9366a6-fea5b3d6-727b6\"},\"HUOBIF\":{\"api_key\":\"34d811df-289a1500-dqnh6tvdf3-3a175\",\"secret_key\":\"54635bfc-0a9366a6-fea5b3d6-727b6\"},\"HUOBICS\":{\"api_key\":\"34d811df-289a1500-dqnh6tvdf3-3a175\",\"secret_key\":\"54635bfc-0a9366a6-fea5b3d6-727b6\"},\"HUOBIUS\":{\"api_key\":\"34d811df-289a1500-dqnh6tvdf3-3a175\",\"secret_key\":\"54635bfc-0a9366a6-fea5b3d6-727b6\"},\"KRKN\":{\"api_key\":\"Ili3G7OerouSZeVcAjpzC8HU+4mHJ35miRkLRPZJS6BxS7JVRbEF/sE+\",\"secret_key\":\"7PfjLkygBrMIPx0lz6LkElMvQ3SSfutw74kItBg6o1+8X+G3KUEU1xTic7552bRmJF4U2FWNt2LgpUdQVNI3lA==\"},\"OKEX\":{\"api_key\":\"9ac22f5f-a180-4fe0-94c0-94a2c6a19ccb\",\"secret_key\":\"2C642A43FD725F36B1E9D1E6ED3AF0FC\",\"passphrase\":\"TradeKey8*\"},\"FLYR\":{\"api_key\":\"Sozrieosy8kajjFXtcaFyx\",\"secret_key\":\"eAO2baniEBWNrqCfTkEa6jHm4hS5PX/a9qVHPu1tezg=\"},\"BITS\":{\"cust_id\":\"2682496\",\"api_key\":\"3G28M4n4K8e8ZFzE3orYYLe3ygzq9X99\",\"secret_key\":\"ZxqhQjZJo4BpO1ZBXaeT8ioSqg25aW6k\"},\"FTXS\":{\"api_key\":\"\",\"secret_key\":\"\"},\"FTXF\":{\"api_key\":\"\",\"secret_key\":\"\"},\"FTX\":{\"api_key\":\"1I-9fkc_igrcd1CV31dhQPdIOPVhXRSSDc8RRRur\",\"secret_key\":\"YbQPs5rqBFbSJ7Z16aHud5unIHdEssiM11XF9Cav\"},\"JUMP\":{\"api_key\":\"glxy0001\",\"secret_key\":\"x9GOmO48nRoz\"},\"LMAX\":{\"user\":\"galaxydigitalUAT1MTF\",\"password\":\"galaxydigitalUAT1MTF\",\"ws_user\":\"galaxydigitalUAT1\",\"ws_password\":\"galaxydigitalUAT1\"},\r\n    \"BLFR\": {\r\n        \"counterparty_id\":\"galaxyotc\",\r\n        \"secret_key\":\"\"\r\n    }}",
//     "VersionStages": [
//         "AWSCURRENT"
//     ],
//     "CreatedDate": 1667843845.091
// }
// )";

    std::string awsCliCmd = "aws secretsmanager get-secret-value --secret-id ";
    awsCliCmd += awsSecretId;
    awsCliCmd += " --region ";
    awsCliCmd += awsRegion;

    auto *aPipe = popen(awsCliCmd.c_str(), "r");
    char buff[1024*1024];
    std::string jsonData;
    while (std::fgets(buff, sizeof(buff), aPipe)) {
        jsonData += buff;
    }
    int status = pclose(aPipe);

    std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << jsonData << std::endl;

    Document doc;
    doc.Parse(jsonData.c_str());

    if (doc.HasParseError() || !doc.HasMember("SecretString")) {
        std::cout << __func__ << ": parse error..." << std::endl;
        return;
    }

    const char *buf = doc["SecretString"].GetString();
    Document secDoc;
    secDoc.Parse(buf);
    if (secDoc.HasParseError() || !secDoc.HasMember(exchId.c_str())) {
        std::cout << __func__ << ": parse error..." << std::endl;
        return;
    }

    auto exchKeys = secDoc[exchId.c_str()].GetObject();
    if (!exchKeys.HasMember("api_key") || !exchKeys["api_key"].IsString()) {
        std::cout << __func__ << ": Missing api_key for " << exchId << std::endl;
        return;
    }
    if (!exchKeys.HasMember("secret_key") || !exchKeys["secret_key"].IsString()) {
        std::cout << __func__ << ": Missing secret_key for " << exchId << std::endl;
        return;
    }
    if (!exchKeys.HasMember("passphrase") || !exchKeys["passphrase"].IsString()) {
        std::cout << __func__ << ": Missing passphrase for " << exchId << std::endl;
        return;
    }
    std::cout << __func__ << ": secret-string(api_key): " << exchKeys["api_key"].GetString() << std::endl;
    std::cout << __func__ << ": secret-string(sec_key): " << exchKeys["secret_key"].GetString() << std::endl;
    std::cout << __func__ << ": secret-string(passphrase): " << exchKeys["passphrase"].GetString() << std::endl;
}

int main() {
    const std::string exchangeId = "GDAX";
    ParseKeys(exchangeId);
    return 0;
}
