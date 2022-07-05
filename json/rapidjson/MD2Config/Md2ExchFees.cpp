// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace rapidjson;

static const char* kTypeNames[] = 
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

void ParseExchFees(std::string filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cout << "Error: failed to open file '" << filename << "'" << std::endl;
        return;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    std::string json = ss.str();
     
    Document doc;
    doc.Parse(json.c_str());
     
    if (doc.IsObject()) {
        for (auto& m : doc.GetObject()) {
            std::cout << "exchange: " << m.name.GetString()
                      << ", makerFee: " << (m.value.HasMember("m") && m.value["m"].IsDouble() ? m.value["m"].GetDouble() : 0) 
                      << ", takerFee: " << (m.value.HasMember("t") && m.value["t"].IsDouble() ? m.value["t"].GetDouble() : 0) 
                      << std::endl;
        }    
    }
    else { // parsing failure.
        std::cout << "Invalid type." << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " <file> \n" << std::endl;
        return 0;
    }

    ParseExchFees(argv[1]);
    return 0;
}

