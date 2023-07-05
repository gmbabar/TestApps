#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include <sstream>

using namespace rapidjson;

/*
 * [{"type":"fill","order_id":"109535955","api_session":"UI","symbol":"btcusd","side":"sell","order_type":"exchange limit","timestamp":"1547743216","timestampms":1547743216580,"is_live":false,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"3592.23","executed_amount":"1","remaining_amount":"0","original_amount":"1","price":"3592.23","fill":{"trade_id":"109535970","liquidity":"Maker","price":"3592.23","amount":"1","fee":"8.980575","fee_currency":"USD"},"socket_sequence":81 }]
 *
 * [{"type":"fill","order_id":556309,"account_name":"primary","api_session":"UI","symbol":"ethbtc","side":"sell","order_type":"exchange limit","timestamp":"1478729284","timestampMs":1478729284169,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0.01514","total_executed_amount":"481.95988631","remaining_amount":"303.06099969","original_amount":"785.020886","original_price":"0.01514",
  "fill" :{"trade_id":"557315","liquidity":"Maker","price":"0.01514","amount":"481.95988631","fee":"0.0182421816968335","fee_currency":"BTC" },"socket_sequence":471177
}]
 */
inline void ProcessFill(rapidjson::Value &rec) {
    // TODO: Add validation check on type of each field like IsString(), IsBool() etc.
    std::cout << ". type: " << rec["type"].GetString() << std::endl;
    std::cout << ". \tsymbol: " << rec["symbol"].GetString() << std::endl;
    std::cout << ". \tside: " << rec["side"].GetString() << std::endl;
    std::cout << ". \tprice: " << (rec.HasMember("price") ? rec["price"].GetString() : " --- MISSING --- ") << std::endl;
    std::cout << ". \torder_id: " << (rec.HasMember("order_id") && rec["order_id"].IsString() ? rec["order_id"].GetString() : " --- MISSING --- ") << std::endl;
    std::cout << ". \tclient_order_id: " << (rec.HasMember("client_order_id") ? rec["client_order_id"].GetString() : "-----") << std::endl;
    std::cout << ". \tis_live: " << rec["is_live"].GetBool() << std::endl;
    std::cout << ". \toriginal_amount: " << rec["original_amount"].GetString() << std::endl;
    std::cout << ". \texecuted_amount: " << (rec.HasMember("executed_amount") ? rec["executed_amount"].GetString() : " --- MISSING ---") << std::endl;
    std::cout << ". \tremaining_amount: " << (rec.HasMember("remaining_amount") ? rec["remaining_amount"].GetString() : " --- MISSING ---") << std::endl;
    std::cout << ". \tavg_execution_price: " << (rec.HasMember("avg_execution_price") ? rec["avg_execution_price"].GetString() : " --- MISSING ---") << std::endl;
    auto fill = rec["fill"].GetObject();
    std::cout << "FILL: \ttrade_id: " << fill["trade_id"].GetString() << std::endl;
    std::cout << "FILL: \tliquidity: " << fill["liquidity"].GetString() << std::endl;
    std::cout << "FILL: \tprice: " << fill["price"].GetString() << std::endl;
    std::cout << "FILL: \tamount: " << fill["amount"].GetString() << std::endl;
    std::cout << "FILL: \tfee: " << fill["fee"].GetString() << std::endl;
    std::cout << "FILL: \tfee_currency: " << fill["fee_currency"].GetString() << std::endl;
    std::cout << std::endl;
}

inline void ProcessOrderUpdates(const std::string& json) {
    // TODO: Add validation check on type of each field like IsString(), IsBool() etc.
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.IsArray()) {
        auto var = doc.GetArray();
        for (auto i = 0; i < var.Size(); i++) {
	    if (!var[i].IsObject())
		    continue;
            auto rec = var[i].GetObject();
	    if (!rec.HasMember("type"))
		    continue;
	    std::string type = rec["type"].GetString();
	    if (type == "fill") {
		    ProcessFill(rec);
		    continue;
	    }
	    if (type != "initial" && type != "accepted" && type != "rejected" && type != "cancelled" && type != "cancel_rejected") {
		    std::cout << "\nUnnecessary Type, SKIPPED: " << json << std::endl << std::endl;
		    continue;
            }

            std::cout << i << ". type: " << rec["type"].GetString() << std::endl;
            std::cout << i << ". \tsymbol: " << rec["symbol"].GetString() << std::endl;
            std::cout << i << ". \tside: " << rec["side"].GetString() << std::endl;
            std::cout << i << ". \tprice: " << rec["price"].GetString() << std::endl;
            std::cout << i << ". \torder_id: " << rec["order_id"].GetString() << std::endl;
            std::cout << i << ". \tclient_order_id: " << (rec.HasMember("client_order_id") ? rec["client_order_id"].GetString() : "-----") << std::endl;
            std::cout << i << ". \tis_live: " << rec["is_live"].GetBool() << std::endl;
            std::cout << i << ". \toriginal_amount: " << rec["original_amount"].GetString() << std::endl;
            std::cout << i << ". \texecuted_amount: " << (rec.HasMember("executed_amount") ? rec["executed_amount"].GetString() : " --- MISSING ---") << std::endl;
            std::cout << i << ". \tremaining_amount: " << (rec.HasMember("remaining_amount") ? rec["remaining_amount"].GetString() : " --- MISSING ---") << std::endl;
	    std::cout << std::endl;
        }
    }
    else 
	std::cout << "\nNOT-ARRAY, SKIPPED: " << json << std::endl << std::endl;
}

int main () {
	// Initial message.
	std::string msg = R"([{"type":"initial","order_id":"2303082623","account_name":"primary","client_order_id":"cc6dmkgb","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1673545546","timestampms":1673545546971,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.1","socket_sequence":0},{"type":"initial","order_id":"2294410909","account_name":"primary","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672238494","timestampms":1672238494725,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.0016","socket_sequence":1},{"type":"initial","order_id":"2294410889","account_name":"primary","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672238480","timestampms":1672238480144,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.0016","socket_sequence":2},{"type":"initial","order_id":"2294410858","account_name":"primary","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672238419","timestampms":1672238419679,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.0016","socket_sequence":3},{"type":"initial","order_id":"2294409973","account_name":"primary","client_order_id":"bsr83zoj","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672236324","timestampms":1672236324470,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.1","socket_sequence":4},{"type":"initial","order_id":"2294408805","account_name":"primary","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672233972","timestampms":1672233972283,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.0016","socket_sequence":5},{"type":"initial","order_id":"2294408759","account_name":"primary","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"ethbtc","side":"buy","order_type":"exchange limit","timestamp":"1672233901","timestampms":1672233901624,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"0.0016","socket_sequence":6}])"; 
	ProcessOrderUpdates(msg);

	// Heartbeat - (ignore)
	msg = R"({"type":"heartbeat","timestampms":1683742950463,"sequence":0,"trace_id":"aecf3eba16bedfd598fc6d3edfc5c7a2","socket_sequence":10})";
	ProcessOrderUpdates(msg);

	// Accepted
	msg = R"([{"type":"accepted","order_id":"2329463034","event_id":"2329463043","account_name":"primary","client_order_id":"4aukltbw","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"btcusd","side":"buy","order_type":"exchange limit","timestamp":"1683743074","timestampms":1683743074294,"is_live":true,"is_cancelled":false,"is_hidden":false,"original_amount":"0.1","price":"10001.10","socket_sequence":35} ])";
	ProcessOrderUpdates(msg);

	// Booked - (ignore)
	msg = R"([{"type":"booked","order_id":"2329463034","event_id":"2329463044","account_name":"primary","client_order_id":"4aukltbw","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"btcusd","side":"buy","order_type":"exchange limit","timestamp":"1683743074","timestampms":1683743074294,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0.00","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"10001.10","socket_sequence":36}])";
	ProcessOrderUpdates(msg);

	// Cancelled
	msg = R"([{"type":"cancelled","order_id":"2329463034","event_id":"2329463138","cancel_command_id":"2329463137","reason":"Requested","account_name":"primary","client_order_id":"4aukltbw","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"btcusd","side":"buy","order_type":"exchange limit","timestamp":"1683743107","timestampms":1683743107671,"is_live":false,"is_cancelled":true,"is_hidden":false,"avg_execution_price":"0.00","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"10001.10","socket_sequence":44}])";
	ProcessOrderUpdates(msg);

	// Closed - (ignore)
	msg = R"([{"type":"closed","order_id":"2329463034","event_id":"2329463139","account_name":"primary","client_order_id":"4aukltbw","api_session":"account-8BDIOhP2VnBAzQHAn6DT","symbol":"btcusd","side":"buy","order_type":"exchange limit","timestamp":"1683743107","timestampms":1683743107671,"is_live":false,"is_cancelled":true,"is_hidden":false,"avg_execution_price":"0.00","executed_amount":"0","remaining_amount":"0.1","original_amount":"0.1","price":"10001.10","socket_sequence":45}])";
	ProcessOrderUpdates(msg);

	// FILL
	msg = R"([{"type":"fill","order_id":"109535955","api_session":"UI","symbol":"btcusd","side":"sell","order_type":"exchange limit","timestamp":"1547743216","timestampms":1547743216580,"is_live":false,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"3592.23","executed_amount":"1","remaining_amount":"0","original_amount":"1","price":"3592.23",
  "fill": {"trade_id":"109535970","liquidity":"Maker","price":"3592.23","amount":"1","fee":"8.980575","fee_currency":"USD"},"socket_sequence":81 }])";
	ProcessOrderUpdates(msg);

	// Partial FILL
	msg = R"([{"type":"fill","order_id":556309,"account_name":"primary","api_session":"UI","symbol":"ethbtc","side":"sell","order_type":"exchange limit","timestamp":"1478729284","timestampMs":1478729284169,"is_live":true,"is_cancelled":false,"is_hidden":false,"avg_execution_price":"0.01514","total_executed_amount":"481.95988631","remaining_amount":"303.06099969","original_amount":"785.020886","original_price":"0.01514",
  "fill" :{"trade_id":"557315","liquidity":"Maker","price":"0.01514","amount":"481.95988631","fee":"0.0182421816968335","fee_currency":"BTC" }, "socket_sequence":471177}])";
	ProcessOrderUpdates(msg);

	// Reject
	msg = R"([{"type":"rejected","order_id":"104246","event_id":"104247","reason":"InvalidPrice","account_name": "primary","api_session":"UI","symbol":"btcusd","side":"buy","order_type":"exchange limit","timestamp":"1478205545","timestampms":1478205545047,"is_live":false,"original_amount":"5","price":"703.14444444","socket_sequence":310311}])";
	ProcessOrderUpdates(msg);

	// Cancel Reject
	msg = R"([{"type":"cancel_rejected","order_id":"6425","event_id":"6434","cancel_command_id":"6433","reason":"OrderNotFound","account_name": "primary","api_session":"UI","symbol":"btcusd","side":"buy","order_type":"limit","timestamp":"1478204773","timestampms":1478204773113,"is_live":true,"is_cancelled":false,"is_hidden":true,"avg_execution_price":"0.00","executed_amount":"0","remaining_amount":"5","original_amount":"5","price":"721.24","socket_sequence":312300}])";
	ProcessOrderUpdates(msg);
}

