// This code does not handle the exceptions

#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <rapidjson/istreamwrapper.h>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>


using namespace rapidjson;


struct ConfigValue
{

    //default ctor
    ConfigValue() {}

    //argument ctor
    ConfigValue(const std::string &atype, rapidjson::Value &aval) {
        type = atype;
        val = aval;
    }

    //copy ctor
    ConfigValue(ConfigValue &obj) {
        type = obj.type;
        val = obj.val;
    }

    //assignment operator
    void operator=(ConfigValue &obj) {
        type = obj.type;
        val = obj.val;
    }

   //getters
    rapidjson::Value Getval() {
        rapidjson::Value aval;
        aval = val;
        return aval;
    }
    std::string Gettype() {
        return type;
    }
    //not for use
    rapidjson::Value* getval() {
        return &val;
    }

    //setters
    void setval(rapidjson::Value &aval) {
        val = aval;
    }
    void settype(std::string atype) {
        type = atype;
    }
    //returns the FNQ against the command from mapperfile
    std::string getpath(const char *command) {
        std::ifstream ConfigMapperFile("./configfiles/Configmapper.json");
        rapidjson::IStreamWrapper json(ConfigMapperFile);
        rapidjson::Document doc;
        std::string path;
        doc.ParseStream(json);
        path = doc[command].GetString();
        std::cout << __func__ << path << std::endl;
        return path;
    }


private:
    std::string type;
    rapidjson::Value val;
    typedef std::unordered_map<std::string,std::string> Mmap;

};

typedef std::unordered_map<std::string,ConfigValue> ConfigMap;

static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

struct MD2ConfigParser {

    MD2ConfigParser(std::string &configFile) {
        this->ParseConfig(configFile);
    }

    std::string asStr(const std::string &value, const std::string &defaultVal="") {
        auto val = valueMap[value.c_str()].Getval();
        if(!val.IsString()) {
            return defaultVal;
        }
        return val.GetString();
    }

    int asInt(const std::string &value, const int &defaultVal=1) {
        auto val = valueMap[value.c_str()].Getval();
        if(!val.IsInt()) {
            return defaultVal;
        }
        return val.GetInt();
    }

    bool asBool(const std::string &value, const bool &defaultVal=false) {
        auto val = valueMap[value.c_str()].Getval();
        if(!val.IsBool()) {
            return defaultVal;
        }
        return val.GetBool();
    }

    double asDouble(const std::string &value, const double &defaultVal=0.0) {
        auto val = valueMap[value.c_str()].Getval();
        if(!val.IsDouble()) {
            return defaultVal;
        }
        return val.GetDouble();
    }
private:

    void ParseConfigEx(rapidjson::Value val, ConfigMap &umap, std::string prefix) {
        if (!prefix.empty())
            prefix += ".";
        for (auto& vitr : val.GetObject()) {
            if (vitr.value.IsObject()) {
                ParseConfigEx(vitr.value.GetObject(), umap, prefix + vitr.name.GetString());
            }
            if((kTypeNames[vitr.value.GetType()] == kTypeNames[4])) {
                auto arr = vitr.value.GetArray();
                for (SizeType i = 0; i < arr.Size(); i++) {
                    if(arr[i].IsObject()) {
                        ParseConfigEx(arr[i].GetObject(), umap, prefix + vitr.name.GetString());
                    }
                }
            }
            else {
                ConfigValue configValue(std::string(kTypeNames[vitr.value.GetType()]), vitr.value);
                umap.emplace(prefix + vitr.name.GetString(), configValue);
            }
        }
    }

    void ParseConfig(std::string &configFile) {
        ConfigValue obj;
        std::ifstream Md2ConfigFile(configFile);
        rapidjson::IStreamWrapper json(Md2ConfigFile);
        rapidjson::Document doc;
        rapidjson::Value value;
        doc.ParseStream(json);

        if (doc.IsObject()) {
            ParseConfigEx(doc.GetObject(), valueMap, "");
            // return umap[command].Getval();
        }
    }


    std::unordered_map<std::string, ConfigValue> valueMap;
};


int main()
{
    std::string file = "./configfiles/MD2config.json";
    const std::string cmd = "CONNECTORS.configs.port";
    MD2ConfigParser parser(file);
    std::cout << parser.asInt(cmd) << std::endl;
    return 0;
}