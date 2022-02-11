#include <iostream>

using namespace std;


bool GetJsonStrVal(const string &aMsg, string key, size_t &begin, size_t &end) {
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

// inline
bool GetJsonNumVal(const std::string &aMsg, const std::string& key, size_t &begin, size_t &end) {
    size_t pos = aMsg.find(key, (begin > 0 && begin < aMsg.size() ? begin : 0));
    if (pos == std::string::npos) 
        return false;
    pos += key.size();
    pos = aMsg.find(':', pos);
    if (pos == std::string::npos) 
        return false;
    begin = pos+1;
    end = pos;
    size_t len = aMsg.size();
    while(++end < len && (isdigit(aMsg[end]) || aMsg[end] == '.'));
    return true;
}

bool ProcessTrade(const std::string &aMsg) {
    // {"jsonrpc":"2.0","method":"subscription","params":{"channel":"trades.future.BTC.raw","data":
    //  [{"trade_seq":125101890,"trade_id":"184082896","timestamp":1634092545692,"tick_direction":2,"price":56285.5,"mark_price":56287.35,
    //    "instrument_name":"BTC-PERPETUAL","index_price":56260.57,"direction":"buy","amount":1120.0}]} 
cout << __func__ << ": " << aMsg << endl;

    size_t beg = 0, end = 0;
    if(!GetJsonStrVal(aMsg, "data", beg, end)) {  // just to get start pos
        return false;
    }
    size_t tbeg = end;
    // TODO: Handle Array:

    bool retval = false;
    while (true) {
        // price
        beg = tbeg;
        if(!GetJsonNumVal(aMsg, "price", beg, end)) {
            return retval;
        }
        auto exchPxBuf = aMsg.substr(beg, end-beg);

        // instrument
        if(!GetJsonStrVal(aMsg, "instrument_name", beg, end)) {
            std::cout << "------> Instrument Not Found After Price" << std::endl;
            beg = tbeg;     // order of values might has been changed.
            if(!GetJsonStrVal(aMsg, "instrument_name", beg, end))
                return false;
        }
        auto exchInstrBuf = aMsg.substr(beg, end-beg);

        // quantity
        beg = tbeg;
        if(!GetJsonNumVal(aMsg, "amount", beg, end)) {
            std::cout << "------> Qauntity Not Found After Instrument" << std::endl;
            beg = tbeg;     // order of values might has been changed.
            if(!GetJsonNumVal(aMsg, "amount", beg, end))
                return false;
        }
        auto exchQtyBuf = aMsg.substr(beg, end-beg);
        tbeg = end;
        retval = true;
        std::cout << __func__ << "(): Instrument: " << exchInstrBuf << ", Price: " << exchPxBuf << ", Qty: " << exchQtyBuf << std::endl;
    }
}


int main () {
    string aMsg = R"("{"jsonrpc":"2.0","method":"subscription","params":{"channel":"trades.future.BTC.raw","data":
        [{"trade_seq":125101890,"trade_id":"184082896","timestamp":1634092545692,"tick_direction":2,"price":56285.5,"mark_price":56287.35,
        "instrument_name":"BTC-PERPETUAL","index_price":56260.57,"direction":"buy","amount":1120.0}
        ,{"trade_seq":125101891,"trade_id":"184082997","timestamp":1634092545697,"tick_direction":1,"price":56245.3,"mark_price":56280.05,
        "instrument_name":"BTC-PERPETUAL","index_price":56261.51,"direction":"buy","amount":1121.0}
        ]})"; 

    if(!ProcessTrade(aMsg)) {
        std::cout << "ERROR: Fast Parsing Failed..." << std::endl;
    }
}


