#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <chrono>

using namespace std;
using namespace rapidjson;

const string JsonParser(const std::string& jsStr, const std::string& keyToFind)
{
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    Document document;
    document.Parse(jsStr.c_str());

    if(document.HasMember(keyToFind.c_str()))
    {    
        if(document[keyToFind.c_str()].IsString())
        {
            return document[keyToFind.c_str()].GetString();
        }
        else if(document[keyToFind.c_str()].IsInt())
        {
            return to_string(document[keyToFind.c_str()].GetInt());
        }
        else if(document[keyToFind.c_str()].IsArray() || document[keyToFind.c_str()].IsObject())
        {
            document[keyToFind.c_str()].Accept(writer);
            return sb.GetString();
        }
        else if(document[keyToFind.c_str()].IsFloat())
        {
            return to_string(document[keyToFind.c_str()].GetFloat());
        }
        else
        {
            return document[keyToFind.c_str()].GetBool() ? "true" : "false";
        }
    }
    else
    {
        return "There's No Key Named \"" + keyToFind + "\"";
    }
}

std::string ParseJsonArr(std::string arr, int index)
{
    Document d;
    StringBuffer sb;
    Writer<StringBuffer> wr(sb);
    d.Parse(arr.c_str());
    if(d.IsArray())
    {
        if(d[index].IsObject())
        {
            d[index].Accept(wr);
            return sb.GetString();
        }
        else if(d[index].IsInt())
        {
            return to_string(d[index].GetInt());
        }
        else if(d[index].IsFloat())
        {
            return to_string(d[index].GetFloat());
        }
        else if(d[index].IsString())
        {
            return d[index].GetString();
        }
        else
        {
            return d[index].GetBool() ? "true":"false";
        }
    }
    else
    {
        return "";
    }
}


int main()
{
    string complexJson = R"({"jsonrpc":"2.0","method":"subscription","params":{"channel":"trades.future.BTC.raw","data":[{"trade_seq":68190894,"trade_id":"101049974","timestamp":1649373917627,"tick_direction":2,"price":43597.5,"mark_price":43596.86,"instrument_name":"BTC-PERPETUAL","index_price":43595.2,"direction":"sell","amount":100.0}]}})";
    string sec = R"({"jsonrpc":"2.0","id":2,"result":["trades.future.BTC.raw"],"usIn":1649398839656428,"usOut":1649398839656555,"usDiff":127,"testnet":true})";
    cout << "Json String : " << complexJson << endl << endl;
    cout << "Second string : " << sec << endl;

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for(int i=0; i<1E4; i++)
    std::string instrument = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "instrument_name");
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(t2.time_since_epoch() - t1.time_since_epoch());
    std::cout << "The time To Parse Single Value using RapidJson: " << timeSpan.count() << " ms" << std::endl;

    t1 = std::chrono::steady_clock::now();
    for(int i=0; i<1E4; i++)
    {
        std::string instrument = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "instrument_name");
        std::string px = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "price");
        std::string size = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "amount");
        std::string trade_id = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "trade_id");
    }
    t2 = std::chrono::steady_clock::now();
    timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(t2.time_since_epoch() - t1.time_since_epoch());
    std::cout << "The time To Parse Multiple Value using RapidJson: " << timeSpan.count() << " ms" << std::endl;

}
