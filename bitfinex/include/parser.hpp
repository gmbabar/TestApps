#include <iostream>
#include <string>
#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

struct FastParser {

    FastParser(std::string &msg) {}
    FastParser() {}
    ~FastParser() {}

    void parse(std::string &msg) {
        jsonDoc.Parse(msg.c_str());
        auto arr = jsonDoc.GetArray();
        if(arr[1].GetType() == rapidjson::Type::kArrayType) {
            auto innerArr = arr[1].GetArray();
            if(innerArr.Size() == 3) {
                parseBook(innerArr);
            }
            else if(innerArr.Size() > 2 && innerArr[1].IsArray()) {
                parseSnapshot(innerArr);
            }
            else {
                parseTicker(innerArr);
            }
        }
        else if(arr[1].GetType() == rapidjson::Type::kStringType) {
            auto innerArr = arr[2].GetArray();
            parseTrade(innerArr);
        }
        else {
            std::cout << "Unknown Message Received" << std::endl;
        }
    }
private:

    void parseBook(rapidjson::GenericArray<false, rapidjson::Value> &arr) {
        std::cout << __func__ << std::endl;
        double px, qty;
        px = (arr[0].IsNumber() && arr[0].IsDouble() ? arr[0].GetDouble() : 0);
        qty = (arr[2].IsNumber() && arr[1].IsDouble() ? arr[1].GetDouble() : 0);
        std::cout << px << " | " << qty << std::endl;
    }

    void parseSnapshot(rapidjson::GenericArray<false, rapidjson::Value> &arr) {
        std::cout << __func__ << std::endl;
        double qty, px;

        for(auto i=0; i < arr.Size(); i++) {
            auto innerArr = arr[i].GetArray();
            if(innerArr.Size()==4) {
                qty = (innerArr[2].IsNumber() ? innerArr[2].GetDouble() : 0);
                px = (innerArr[3].IsNumber() ? innerArr[3].GetDouble() : 0);
            }
            else if(innerArr.Size()==3) {
                px = (innerArr[0].IsNumber() ? innerArr[0].GetDouble() : 0);
                qty = (innerArr[2].IsNumber() ? innerArr[2].GetDouble() : 0);
            }
            std::cout << px << " | " << qty << std::endl;
        }
    }

    void parseTicker(rapidjson::GenericArray<false, rapidjson::Value> &arr) {
        std::cout << __func__ << std::endl;
        double askQty, bidQty;
        int askPx, bidPx;
        for(auto idx=0; idx<arr.Size(); idx++) {
            switch (idx)
            {
                case 0:
                    bidPx = arr[idx].GetInt();
                    break;
                case 1:
                    bidQty = arr[idx].GetDouble();
                    break;
                case 2:
                    askPx = arr[idx].GetInt();
                    break;
                case 3:
                    askQty = arr[idx].GetDouble();
                    break;
                default:
                    break;
            }
        }
        std::cout << askQty << "|" << askPx << "|" << bidQty << "|" <<  bidPx << std::endl;
    }

    void parseTrade(rapidjson::GenericArray<false, rapidjson::Value> &arr) {
        std::cout << __func__ << std::endl;
        double px, qty;
        px = (arr[2].IsNumber() && arr[2].IsDouble() ? arr[2].GetDouble() : 0);
        qty = (arr[3].IsNumber() ? arr[3].GetDouble() : 0);
        std::cout << px << " | " << qty << std::endl;
    }

    rapidjson::Document jsonDoc;
};