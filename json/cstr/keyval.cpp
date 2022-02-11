#include <iostream>

using namespace std;


const char *GetJsonStrVal(string &aMsg, string key) {
	size_t pos = aMsg.find(key);
	if (pos == string::npos) return NULL;
	pos += key.size();
	pos = aMsg.find(':', pos);
	if (pos == string::npos) return NULL;
	size_t begin = aMsg.find('"', pos)+1;
	if (begin == string::npos) return NULL;
	size_t end = aMsg.find('"', begin);
	if (end == string::npos) return NULL;
 	aMsg[end] = '\0';
	return aMsg.c_str() + begin;
}

bool GetJsonStrVal(string &aMsg, string key, size_t &begin, size_t &end) {
	size_t pos = aMsg.find(key);
	if (pos == string::npos) return false;
	pos += key.size();
	pos = aMsg.find(':', pos);
	if (pos == string::npos) return false;
	begin = aMsg.find('"', pos)+1;
	if (begin == string::npos) return false;
	end = aMsg.find('"', begin);
	if (end == string::npos) return false;
	return true;
}


int main () {
	string aMsg = R"("{"jsonrpc":"2.0","method":"subscription","params":{"channel":"trades.future.BTC.raw","data":
        [{"trade_seq":125101890,"trade_id":"184082896","timestamp":1634092545692,"tick_direction":2,"price":56285.5,"mark_price":56287.35,
        "instrument_name":"BTC-PERPETUAL","index_price":56260.57,"direction":"buy","amount":1120.0}]})"; 

	size_t begin, end;
	if (GetJsonStrVal(aMsg, "channel", begin, end))
		cout << aMsg.substr(begin, end-begin) << endl;
	cout << GetJsonStrVal(aMsg, "price") << endl;
	cout << GetJsonStrVal(aMsg, "instrument_name") << endl;
}
