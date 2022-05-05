// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <chrono>

using namespace rapidjson;

static const char* kTypeNames[] = 
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

void CheckError(const char *json) {

    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << __func__ << ": " << json << std::endl;
    Document doc;
    doc.Parse(json);

    if (doc.HasParseError()) {
	std::cout << __func__ << ": found parsing error." << std::endl;
        std::cout << "Invalid json message: " << json << std::endl;
for (int i=0; i<doc.GetErrorOffset(); ++i)
    std::cout << '-';
std::cout << '^' << std::endl;
	std::cout << "ErrorCode: " << doc.GetParseError() << std::endl;
	std::cout << "ErrorOffset: " << doc.GetErrorOffset() << std::endl;
	return;
    }
    else {
        std::cout << __func__ << ": no parsing error found. Object? : " << doc.IsObject() << std::endl;
    }
    std::cout << std::endl;

    for (auto& m : doc.GetObject())
    printf("Type of member %s is %s\n",
        m.name.GetString(), kTypeNames[m.value.GetType()]);
}

int main() {
    const char* json = R"(
{ "key1" : "stringValue", "key2" : 1234, "key3" : 123.45, "key4" : {1, 2, 3, 4} }
)";
    CheckError(json);

    json = R"(
{"type":"security_list_request"}
)";
    CheckError(json);

    json = R"(
{\"type\":\"security_list_request\"}
)";
    CheckError(json);

    json = R"(
"{\"type\":\"security_list_request\"}"
)";
    CheckError(json);
    return 0;
}
