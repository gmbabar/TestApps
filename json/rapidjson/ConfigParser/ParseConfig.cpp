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
    std::string getpath(std::string command) {
        std::ifstream ConfigMapperFile("./configfiles/Configmapper.json");
        std::string line;
        std::vector <std::string> tokens;
        std::string intermediate;
        std::string path;
        int loopcount;
        int value = 1;
        int key = 0;
        Mmap aMapperMap;
        //reads data and store it in vector
        while (getline(ConfigMapperFile, line)) {
            std::stringstream check1(line);
            while(getline(check1, intermediate, ':')) {
                tokens.push_back(intermediate);
                getline(check1, intermediate, ',');
                tokens.push_back(intermediate); 
            }
        }
        loopcount = tokens.size()/2;
        // fill the map with key values from vector
        for (int i = 0; i < loopcount; i++) {
            aMapperMap.emplace(tokens[key],tokens[value]);
            value+=2;
            key+=2;    
        }
        //checks and load path for command from map
        for (auto &itr : aMapperMap)
        {
            if(itr.first.find(command)!=std::string::npos)
            {
                path = itr.second;
            }
        }
        return path;
        
    }


private:
    std::string type;
    rapidjson::Value val;
    typedef std::unordered_map<std::string,std::string> Mmap;

};

typedef std::unordered_map<std::string,ConfigValue> ConfigMap;

static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

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


void ParseConfig(std::string command) {
    ConfigValue obj;
    std::unordered_map<std::string, ConfigValue> umap;
    std::string fqn = obj.getpath(command);
    std::ifstream Md2ConfigFile("./configfiles/MD2config.json");
    rapidjson::IStreamWrapper json(Md2ConfigFile);
    rapidjson::Document doc;
    doc.ParseStream(json);
   
    if (doc.IsObject()) {
        ParseConfigEx(doc.GetObject(), umap, "");
        for(auto &itr : umap) {
            std::string path = itr.first;
            auto value = itr.second.Getval();
            if (value.IsNumber()) { 
                if(fqn.find(path)!= std::string::npos)
                std::cout << itr.first << " : " << itr.second.Gettype() << " = " << value.GetInt() << std::endl;      
            }
            else if(value.IsString()) {
                if(fqn.find(path)!= std::string::npos)
                std::cout << itr.first << " : " << itr.second.Gettype() << " = " << value.GetString() << std::endl;
        
            }
            else if(value.IsBool()) {
                if(fqn.find(path)!= std::string::npos)
                std::cout << itr.first << " : " << itr.second.Gettype() << " = " << value.GetBool() << std::endl;
            }
        }

    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout <<
            "Usage:\n\t" << argv[0] << " <command>\n" <<
            "Example:\n\t" << argv[0] << " pf.host\n";
        return EXIT_FAILURE;
    }
    std::string command = argv[1];
    ParseConfig(command);
    return 0;
}