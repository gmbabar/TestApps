#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <rapidjson/document.h>

static const char* kTypeNames[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};

int main()
{
    boost::asio::ip::tcp::iostream s("172.21.2.170", "9004");
    if(!s)
        std::cout << "Could not connect to 172.21.2.170 \n";
    s << "GET /api/v1/aliases/1 HTTP/1.0\r\n"
      << "Host: 172.21.2.170 \r\n"
      << "Accept: */*\r\n"
      << "Connection: close\r\n\r\n" ;
    bool body = false;
    std::ostringstream oss;
    for(std::string line; getline(s, line); ) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        //std::cout << "line: ("<< line.size() << ") ^" << line << "$\n";
        if (line.size() == 0)
            body = true;
        else if (body) {
            oss << line;
        }
    }
    std::cout << oss.str() << std::endl << std::endl;
    std::string jsonStr(oss.str());
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() && !doc.IsArray()) {
        std::cout << "Failed to parse json string " << std::endl;
        return 0;
    }
    if (doc.IsArray()) {
        for (auto& arr : doc.GetArray())
            if (arr.IsObject())
                for (auto& obj : arr.GetObject())
                    std::cout << "name: " << obj.name.GetString() << "\t type: " << kTypeNames[obj.value.GetType()] << std::endl;
    }
}
