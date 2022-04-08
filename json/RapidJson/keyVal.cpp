#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace std;
using namespace rapidjson;

string JsonParser(std::string jsStr, std::string keyToFind)
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
		else if(document[keyToFind.c_str()].IsNumber())
		{
			return to_string(document[keyToFind.c_str()].GetInt());
		}
		else if(document[keyToFind.c_str()].IsArray() || document[keyToFind.c_str()].IsObject())
		{
			document[keyToFind.c_str()].Accept(writer);
			return sb.GetString();
		}
		else
		{
			std::string str = document[keyToFind.c_str()].GetBool() ? "true" : "false";
			return str;
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
		else if(d[index].IsNumber())
		{
			return to_string(d[index].GetInt());
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
	std::string val = JsonParser(ParseJsonArr(JsonParser(JsonParser(complexJson, "params"), "data"), 0), "instrument_name");
	cout << "The Value Of Trande_seq Is : " << val << endl;
}
