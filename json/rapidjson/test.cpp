// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>

using namespace rapidjson;

int main() {
    std::string msg = R"({"DATA":0.01,"DATA2":"-10", "DATA3":10})";
    Document doc;
    doc.Parse(msg.c_str());
    std::cout << fabs(doc["DATA"].GetDouble()) << std::endl;
    std::cout << doc["DATA2"].GetString() << std::endl;
    std::cout << doc["DATA3"].GetDouble() << std::endl;
    return 0;
}
