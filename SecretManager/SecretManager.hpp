#include <iostream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <rapidjson/document.h>
#include "BufRange.hpp"


struct SecretManager
{
    SecretManager(std::stringstream &aCommand, std::string aFile="data.txt") {
        aCommand << " > " << aFile;
        int status = std::system(aCommand.str().c_str());
        if (WEXITSTATUS(status)) {
            std::cerr << "Make Sure \"AWS Cli\" is installed on your system and configured correctly" << std::endl;
            return;
        }
        ReadFile(aFile);
    }

    nebula::data::ConstBufRange GetSecretID() {
        nebula::data::ConstBufRange secretID(&m_buf[1], &m_buf[m_delim]);
        return secretID;
    }

    nebula::data::ConstBufRange GetSecret() {
        nebula::data::ConstBufRange secret(&m_buf[m_delim]+1, &m_buf[strlen(m_buf)]-1);
        return secret;
    }

private:
    void ReadFile(std::string &aFile) {
        std::fstream file(aFile);
        while (!file.eof()) {
            std::string line;
            std::getline(file, line);
            m_jsonData += line;
        }

        ParseResponse();
    }

    void ParseResponse() {
        rapidjson::Document doc;
        doc.Parse(m_jsonData.c_str());
        if (doc.HasParseError()) {
            std::cerr << "Error Occured While Parsing Json" << std::endl;
            return;
        }

        if (!doc.HasMember("SecretString")) {
            std::cerr << "Json has no member \"SecretString\"" << std::endl;
            return;
        }
        m_buf = doc["SecretString"].GetString();
        m_delim = std::string(m_buf).find(":");
    }

    std::string m_jsonData;
    const char *m_buf;
    int m_delim;

};