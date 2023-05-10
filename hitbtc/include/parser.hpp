#include <iostream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include "bufrange.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <vector>

using namespace rapidjson;
/*
    -----------Top-Of-Book

    {"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669908019703,"a":"16984.70","A":"0.00352","b":"16977.15","B":"0.01041"}}}
    {"ch":"orderbook/full","snapshot":{"ETHBTC":{"t":1681300780018,"s":1204702,"a":[["0.062390","424.7196"],["0.063710","175.4782"],["0.064228","167.5480"],["0.065725","142.5326"],["0.066002","344.9795"],["0.066106","305.4681"],["0.066118","136.0912"],["0.066565","280.2104"],["0.066781","262.0664"],["0.072152","175.5805"],["0.072298","172.8993"],["0.072810","170.3870"],["0.072883","168.0256"],["0.073196","165.7998"],["0.073708","163.6963"],["0.073894","161.7038"],["0.074386","159.8123"],["0.074699","158.0131"],["0.074726","156.2985"],["0.075111","154.6617"],["0.075391","153.0967"],["0.075796","151.5981"],["0.075996","150.1610"],["0.076388","148.7812"],["0.076761","147.4547"],["0.076993","146.1779"],["0.077452","144.9477"],["0.077612","143.7612"],["0.077818","142.6156"],["0.078184","141.5086"],["0.078550","140.4379"],["0.078756","139.4014"],["0.079301","138.3972"],["0.079600","137.4237"],["0.079940","136.4792"],["0.080279","135.5622"],["0.080525","134.6712"],["0.080618","133.8052"],["0.081043","132.9627"],["0.081502","132.1428"],["0.081542","131.3443"],["0.081848","130.5663"],["0.082274","129.8079"],["0.082739","129.0683"],["0.082946","128.3465"],["0.083192","127.6419"],["0.083531","126.9538"],["0.083817","126.2815"],["0.084316","125.6243"],["0.084522","124.9817"],["0.084801","124.3532"],["0.084961","123.7380"],["0.085546","123.1359"],["0.085672","122.5462"],["0.085905","121.9686"],["0.086351","121.4027"],["0.086790","120.8479"],["0.086896","120.3039"],["0.087228","119.7704"],["0.087441","119.2470"],["0.087987","118.7334"],["0.088126","118.2292"],["0.088552","117.7341"],["0.088858","117.2480"],["0.089190","116.7704"],["0.089436","116.3011"],["0.089816","115.8399"],["0.089962","115.3865"],["0.090348","114.9407"],["0.090687","114.5023"],["0.090966","114.0710"],["0.091312","113.6467"],["0.091511","113.2291"],["0.092057","112.8181"],["0.092223","112.4135"],["0.092589","112.0151"],["0.092841","111.6227"],["0.093254","111.2363"],["0.093407","110.8555"],["0.093832","110.4804"],["0.094165","110.1107"],["0.094384","109.7463"],["0.094850","109.3870"],["0.095043","109.0328"],["0.095182","108.6835"],["0.095741","108.3391"],["0.095927","107.9993"],["0.096273","107.6640"],["0.096692","107.3333"],["0.096912","107.0069"],["0.097138","106.6847"],["0.097603","106.3667"],["0.097703","106.0528"],["0.098002","105.7428"],["0.098501","105.4368"],["0.098681","105.1345"],["0.099146","104.8360"],["0.099452","104.5411"],["0.099605","104.2498"],["0.099991","103.9619"],["0.100376","103.6775"],["0.100490","103.3964"],["0.101002","103.1185"],["0.101115","102.8439"],["0.101647","102.5724"],["0.101760","102.3040"],["0.102252","102.0387"],["0.102378","101.7762"],["0.102717","101.5167"],["0.102937","101.2600"],["0.103409","101.0061"],["0.103802","100.7550"],["0.103861","100.5065"],["0.104174","100.2607"],["0.104772","100.0174"],["0.105012","99.7767"],["0.105291","99.5385"],["0.105517","99.3027"],["0.105817","99.0693"],["0.106189","98.8383"],["0.106475","98.6096"],["0.106881","98.3832"],["0.107213","98.1590"],["0.107546","97.9370"],["0.107592","97.7171"],["0.107938","97.4994"],["0.108231","97.2838"],["0.108736","97.0702"],["0.108956","96.8587"],["0.109301","96.6491"],["0.109528","96.4414"],["0.109993","96.2357"],["0.110133","96.0319"],["0.110499","95.8300"],["0.110745","95.6298"],["0.111024","95.4315"],["0.111583","95.2349"],["0.111855","95.0401"],["0.112194","94.8470"],["0.112241","94.6556"],["0.112580","94.4658"],["1.000000","0.5296"],["2.000000","0.0212"],["38.172201","0.0020"],["208.500670","0.0206"],["210.000000","0.0001"],["17000.000000","0.4355"]],"b":[["0.060606","172.8993"],["0.060557","167.5480"],["0.060326","160.9687"],["0.060260","170.3870"],["0.060195","155.3802"],["0.059981","168.0256"],["0.059655","165.7998"],["0.059216","163.6963"],["0.059141","142.5326"],["0.059050","161.7038"],["0.058891","139.1507"],["0.058804","159.8123"],["0.058498","158.0131"],["0.058193","156.2985"],["0.057787","154.6617"],["0.057574","153.0967"],["0.057354","178.4516"],["0.057180","175.5805"],["0.057176","151.5981"],["0.056737","150.1610"],["0.056464","148.7812"],["0.056305","147.4547"],["0.055893","146.1779"],["0.055527","144.9477"],["0.055308","143.7612"],["0.054915","142.6156"],["0.054696","141.5086"],["0.054523","140.4379"],["0.054204","139.4014"],["0.053892","138.3972"],["0.053313","137.4237"],["0.053267","136.4792"],["0.052755","135.5622"],["0.052502","134.6712"],["0.052103","133.8052"],["0.051957","132.9627"],["0.051485","132.1428"],["0.051385","131.3443"],["0.051106","130.5663"],["0.050648","129.8079"],["0.050229","129.0683"],["0.050116","128.3465"],["0.049837","127.6419"],["0.049524","126.9538"],["0.049006","126.2815"],["0.048746","125.6243"],["0.048574","124.9817"],["0.048161","124.3532"],["0.047796","123.7380"],["0.047596","123.1359"],["0.047324","122.5462"],["0.047111","121.9686"],["0.046499","121.4027"],["0.046406","120.8479"],["0.046008","120.3039"],["0.045615","119.7704"],["0.045389","119.2470"],["0.045190","118.7334"],["0.044705","118.2292"],["0.044332","117.7341"],["0.044246","117.2480"],["0.043874","116.7704"],["0.043408","116.3011"],["0.043406","0.0357"],["0.043388","115.8399"],["0.042890","115.3865"],["0.042624","114.9407"],["0.042506","0.0380"],["0.042371","114.5023"],["0.042152","114.0710"],["0.041605","0.0380"],["0.041547","113.6467"],["0.041461","113.2291"],["0.041221","112.8181"],["0.040783","112.4135"],["0.040705","0.0380"],["0.040390","112.0151"],["0.040038","111.6227"],["0.039812","111.2363"],["0.039805","0.0360"],["0.039466","110.8555"],["0.039347","110.4804"],["0.038934","110.1107"],["0.038905","0.0360"],["0.038509","109.7463"],["0.038170","109.3870"],["0.038090","109.0328"],["0.038005","0.0360"],["0.037665","108.6835"],["0.037379","108.3391"],["0.037104","0.0360"],["0.037033","107.9993"],["0.036621","107.6640"],["0.036289","107.3333"],["0.036204","0.0360"],["0.036149","107.0069"],["0.035757","106.6847"],["0.035498","106.3667"],["0.035305","106.0528"],["0.035304","0.0360"],["0.034747","105.7428"],["0.034587","105.4368"],["0.034404","0.0360"],["0.034122","105.1345"],["0.033856","104.8360"],["0.033596","104.5411"],["0.033504","0.0380"],["0.033284","104.2498"],["0.032918","103.9619"],["0.032603","0.0380"],["0.032593","103.6775"],["0.032393","103.3964"],["0.031941","103.1185"],["0.031755","102.8439"],["0.031703","0.0380"],["0.031383","102.5724"],["0.031296","102.3040"],["0.030803","0.0380"],["0.030771","102.0387"],["0.030572","101.7762"],["0.030359","101.5167"],["0.029980","101.2600"],["0.029903","0.0380"],["0.029561","101.0061"],["0.029315","100.7550"],["0.029003","0.0380"],["0.028870","100.5065"],["0.028717","100.2607"],["0.028292","100.0174"],["0.028102","0.0380"],["0.027919","99.7767"],["0.027813","99.5385"],["0.027434","99.3027"],["0.027202","0.0380"],["0.027188","99.0693"],["0.026896","98.8383"],["0.026643","98.6096"],["0.026302","0.0380"],["0.026051","98.3832"],["0.025845","98.1590"],["0.025533","97.9370"],["0.025402","0.0380"],["0.025254","97.7171"],["0.024948","97.4994"],["0.024749","97.2838"],["0.024502","0.0380"],["0.024476","97.0702"],["0.024031","96.8587"],["0.023711","96.6491"],["0.023601","0.0380"],["0.023293","96.4414"],["0.023093","96.2357"],["0.022801","96.0319"],["0.022701","0.0380"],["0.022542","95.8300"],["0.022289","95.6298"],["0.022023","95.4315"],["0.021801","0.0380"],["0.021531","95.2349"],["0.021365","95.0401"],["0.020939","94.8470"],["0.020901","0.0380"],["0.020627","94.6556"],["0.020434","94.4658"],["0.020000","0.0380"],["0.014467","1.0000"],["0.010000","1.4278"],["0.006016","0.0002"],["0.006000","0.0003"],["0.004000","1.0000"],["0.003578","2.4364"],["0.003000","0.0500"],["0.001600","0.0012"],["0.001000","20.0001"],["0.000060","0.0002"],["0.000007","100.0000"]]}}}
    {"ch":"orderbook/full","update":{"ETHBTC":{"t":1681300781478,"s":1204703,"a":[["0.064534","217.5052"],["0.064752","209.9539"],["0.064964","203.4214"]],"b":[["0.061543","267.7804"],["0.061162","250.4412"]]}}}
    {"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}

    */


namespace SlowParser {
    /*
    -----------Top-Of-Book

    {"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669908019703,"a":"16984.70","A":"0.00352","b":"16977.15","B":"0.01041"}}}

    */

    inline void parseTopOfBook(rapidjson::Document &doc) {
        std::string askPX, bidPX, askQty, bidQty;
        auto json = doc["data"].GetObject();
        std::cout << __func__ << std::endl;
        std::string symbol = json.MemberBegin()->name.GetString();
        auto innerData = json[symbol.c_str()].GetObject();
        askPX = innerData["a"].GetString();
        askQty = innerData["A"].GetString();
        bidPX = innerData["b"].GetString();
        bidQty = innerData["B"].GetString();

        std::cout << askPX << " | " << askQty << " | " << bidPX << " | " << bidQty << std::endl;
    }

    inline void parseOrderBook(rapidjson::Document &doc) {
        std::string askPx, bidPx, askQty, bidQty;
        auto json = doc["data"].GetObject();
        std::cout << __func__ << std::endl;
        std::string symbol = json.MemberBegin()->name.GetString();
        auto innerData = json[symbol.c_str()].GetObject();
        auto asks = innerData["a"].GetArray();
        for (SizeType idx=0; idx<asks.Size(); i++) {
            auto askArr = asks[idx].GetArray();
            for (SizeType indx=0; indx<askArr.Size(); indx++) {
                switch(indx) {
                    case 0:
                        askQty = askArr[indx].GetString();
                        break;
                    case 1:
                        askPx = askArr[indx].GetString();
                        break;
                    default:
                        continue;
                }
            }

        }
        // askQty = innerData["A"].GetString();
        auto bids = innerData["b"].GetString();
        for (SizeType idx=0; idx<bids.Size(); i++) {
            auto bidArr = bids[idx].GetArray();
            for (SizeType indx=0; indx<bidArr.Size(); indx++) {
                switch(indx) {
                    case 0:
                        askQty = bidArr[indx].GetString();
                        break;
                    case 1:
                        askPx = bidArr[indx].GetString();
                        break;
                    default:
                        continue;
                }
            }

        }

        // bidQty = innerData["B"].GetString();

        std::cout << askPX << " | " << askQty << " | " << bidPX << " | " << bidQty << std::endl;
    }

    /*
    ----------Trade-Update
    {"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}
    */
a
    inline void parseTrade(rapidjson::Document &document) {
        // Document document;
        // document.Parse(json.c_str());
        std::cout << "channel: " << document["trades"].GetString() << std::endl;
        auto obj = document["update"].IsObject() ? document["update"].GetObject() : document["snapshot"].GetObject();
        std::cout << "symbol: " << obj["symbol"].GetString() << std::endl;
        const char* symbol = obj["symbol"].GetString();
        auto var = document[symbol].GetArray();
        document["update"].IsObject() ? std::cout << "update : [" << std::endl : std::cout << "snapshot : [" << std::endl;
        // std::cout << "update : [" << std::endl;
        for (SizeType i = 0; i < var.Size(); i++) {
            auto arrVal = var[i].GetObject();
            std::cout << "time_ms: " << arrVal["t"].GetInt64() << std::endl;
            std::cout << "Trade identifier: " << arrVal["i"].GetInt64() << std::endl;
            std::cout << "price: " << arrVal["p"].GetString() << std::endl;
            std::cout << "quantity: " << arrVal["q"].GetString() << std::endl;
            std::cout << "side: " << arrVal["s"].GetString() << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    // {"ch": "trades","snapshot": {"BTCUSDT": [{"t": 1626861109494,"i": 1555634969,"p": "30881.96","q": "12.66828","s": "buy"}]}}

    /*
    ----------Trade-Snapshot
    */

    inline void parseTradeSnapshot(rapidjson::Document &doc) {
        std::string side, px, qty, symbol;
        symbol = doc.MemberBegin()->name.GetString();
        auto innerArr = doc[symbol.c_str()].GetArray();
        for(auto idx=0; innerArr.Size(); idx++) {
            auto data = innerArr[idx].GetObject();
            px = data["p"].GetString();
            qty = data["q"].GetString();
            side = data["s"].GetString();
            std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << px << ", " << qty << ", " << side << std::endl;
        }
    }


    // {"ch":"ticker/1s","data":{"BTCUSDT":{"t":1670238331387,"a":"17290.25","A":"0.10000","b":"17280.80","B":"0.07226","o":"16997.28","c":"17285.54","h":"17413.05","l":"16905.89","v":"19842.66231","q":"340540322.4741842","p":"288.26","P":"1.695918405768452","L":1998467391}}}

    void parseTicker(rapidjson::Document &doc) {
        std::string askPX, bidPX, askQty, bidQty;
        auto json = doc["data"].GetObject();
        std::cout << __func__ << std::endl;
        std::string symbol = json.MemberBegin()->name.GetString();
        auto innerData = json[symbol.c_str()].GetObject();
        askPX = innerData["a"].GetString();
        askQty = innerData["A"].GetString();
        bidPX = innerData["b"].GetString();
        bidQty = innerData["B"].GetString();

        std::cout << askPX << " | " << askQty << " | " << bidPX << " | " << bidQty << std::endl;
    }
}

namespace FastParser {
    /*
    -----------Top-Of-Book

    {"ch":"orderbook/top/100ms","data":{"BTCUSDT":{"t":1669908019703,"a":"16984.70","A":"0.00352","b":"16977.15","B":"0.01041"}}}

    */

    inline void parseTopOfBook(std::string &json) {
        const char *aBuf = json.c_str();
        auto sPos = json.find("data");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        Parse::data::ConstBufRange symbol(&aBuf[symS]+1, &aBuf[symE]);
        // auto sym = json.substr(symS+1, symE-symS-1);

        auto dataS = json.find('{', symE+1);
        auto askPxS = json.find("a", dataS);
        auto askPxC = json.find(':', askPxS+1);
        askPxS = json.find('"', askPxC);
        auto askPxE = json.find('"', askPxS+1);
        Parse::data::ConstBufRange askPx(&aBuf[askPxS]+1, &aBuf[askPxE]);
        // auto askPx = json.substr(askPxS+1, askPxE-askPxS-1);

        auto askQtyCol = json.find(':', askPxE);
        auto askQtyS = json.find('"', askQtyCol);
        auto askQtyE = json.find('"', askQtyS+1);

        Parse::data::ConstBufRange askQty(&aBuf[askQtyS]+1, &aBuf[askQtyE]);
        // auto askQty = json.substr(askQtyS+1, askQtyE-askQtyS-1);

        auto bidPxCol = json.find(':', askQtyE);
        auto bidPxS = json.find('"', bidPxCol+1);
        auto bidPxE = json.find('"', bidPxS+1);
        Parse::data::ConstBufRange bidPx(&aBuf[bidPxS]+1, &aBuf[bidPxE]);
        // auto bidPx = json.substr(bidPxS+1, bidPxE-bidPxS-1);

        auto bidQtyCol = json.find(':', bidPxE);
        auto bidQtyS = json.find('"', bidQtyCol+1);
        auto bidQtyE = json.find('"', bidQtyS+1);
        Parse::data::ConstBufRange bidQty(&aBuf[bidQtyS]+1, &aBuf[bidQtyE]);
        // auto bidQty = json.substr(bidQtyS+1, bidQtyE-bidQtyS-1);
        // std::cout << askPx << " | " << askQty << " | " << bidPx << " | " << bidQty << std::endl;
        std::cout << askPx.asStr() << " | " << askQty.asStr() << " | " << bidPx.asStr() << " | " << bidQty.asStr() << std::endl;
    }


    // {"ch":"ticker/1s","data":{"BTCUSDT":{"t":1670238331387,"a":"17290.25","A":"0.10000","b":"17280.80","B":"0.07226","o":"16997.28","c":"17285.54","h":"17413.05","l":"16905.89","v":"19842.66231","q":"340540322.4741842","p":"288.26","P":"1.695918405768452","L":1998467391}}}

    void parseTicker(std::string &json) {
        auto sPos = json.find("data");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        const char *aBuf = json.c_str();

        Parse::data::ConstBufRange symbol(&aBuf[symS]+1, &aBuf[symE]);
        // auto sym = json.substr(symS+1, symE-symS-1);

        auto dataS = json.find('{', symE+1);
        auto askPxS = json.find("a", dataS);
        auto askPxC = json.find(':', askPxS+1);
        askPxS = json.find('"', askPxC);
        auto askPxE = json.find('"', askPxS+1);
        Parse::data::ConstBufRange askPx(&aBuf[askPxS]+1, &aBuf[askPxE]);

        // auto askPx = json.substr(askPxS+1, askPxE-askPxS-1);

        auto askQtyCol = json.find(':', askPxE);
        auto askQtyS = json.find('"', askQtyCol);
        auto askQtyE = json.find('"', askQtyS+1);
        Parse::data::ConstBufRange askQty(&aBuf[askQtyS]+1, &aBuf[askQtyE]);

        // auto askQty = json.substr(askQtyS+1, askQtyE-askQtyS-1);

        auto bidPxCol = json.find(':', askQtyE);
        auto bidPxS = json.find('"', bidPxCol+1);
        auto bidPxE = json.find('"', bidPxS+1);
        Parse::data::ConstBufRange bidPx(&aBuf[bidPxS]+1, &aBuf[bidPxE]);

        // auto bidPx = json.substr(bidPxS+1, bidPxE-bidPxS-1);

        auto bidQtyCol = json.find(':', bidPxE);
        auto bidQtyS = json.find('"', bidQtyCol+1);
        auto bidQtyE = json.find('"', bidQtyS+1);
        Parse::data::ConstBufRange bidQty(&aBuf[bidQtyS]+1, &aBuf[bidQtyE]);

        // auto bidQty = json.substr(bidQtyS+1, bidQtyE-bidQtyS-1);
        // std::cout << askPx << " | " << askQty << " | " << bidPx << " | " << bidQty << std::endl;
        std::cout << askPx.asStr() << " | " << askQty.asStr() << " | " << bidPx.asStr() << " | " << bidQty.asStr() << std::endl;
    }

    /*
    ----------Trade-Update
    {"ch":"trades","update":{"BTCUSDT":[{"t":1661352113934,"i":1872478761,"p":"21460.55","q":"0.00105","s":"buy"},{"t":1661352113934,"i":1872478762,"p":"21460.56","q":"0.07000","s":"buy"}]}}
    */

    inline void parseTradeUPD(std::string &json) {
        Parse::data::ConstBufRange side, px, qty;
        // std::string side, px, qty, symbol;
        const char *aBuf = json.c_str();
        int dataS, dataE;
        auto sPos = json.find("update");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        Parse::data::ConstBufRange symbol(&aBuf[symS+1], &aBuf[symE]);
        // symbol = json.substr(symS+1, symE-symS-1);
        auto arrS = json.find('[', symE);
        auto arrE = json.find(']', arrS);

        auto arr = json.substr(arrS+1, arrE-arrS-1);
        aBuf = arr.c_str();
        int valStart, valEnd;
        // std::cout << arr << std::endl;
        for(int i=0; i<arr.size(); i++) {

            if(arr[i] == 'p') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                px.buf = &aBuf[valStart+1];
                px.bufEnd = &aBuf[valEnd];
                // px = arr.substr(valStart+1, valEnd-valStart-1);
            }
            if(arr[i] == 'q') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                qty.buf = &aBuf[valStart+1];
                qty.bufEnd = &aBuf[valEnd];
                // qty = arr.substr(valStart+1, valEnd-valStart-1);
            }
            if(arr[i] == 's') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // side = arr.substr(valStart+1, valEnd-valStart-1);
                side.buf = &aBuf[valStart+1];
                side.bufEnd = &aBuf[valEnd];
                std::cout << "|" << side << " for " << px << "@" << qty << " on " << symbol << "|" << std::endl;
                // std::cout << symbol.asStr() << std::endl;
                if(arr.find("s", i+1) == std::string::npos) {
                    break;
                }
            }
        }
    }


    // {"ch": "trades","snapshot": {"BTCUSDT": [{"t": 1626861109494,"i": 1555634969,"p": "30881.96","q": "12.66828","s": "buy"}]}}

    /*
    ----------Trade-Snapshot
    */

    inline void parseTradeSnapshot(std::string &json) {
        Parse::data::ConstBufRange side, px, qty;
        // std::string side, px, qty, symbol;
        const char *aBuf = json.c_str();
        int dataS, dataE;
        auto sPos = json.find("snapshot");
        auto col = json.find(':', sPos);
        auto start = json.find('{', col);
        auto symS = json.find('"', start+1);
        auto symE = json.find('"', symS+1);
        // symbol = json.substr(symS+1, symE-symS-1);
        Parse::data::ConstBufRange symbol(&aBuf[symS+1], &aBuf[symE]);
        auto arrS = json.find('[', symE);
        auto arrE = json.find(']', arrS);

        auto arr = json.substr(arrS+1, arrE-arrS-1);
        aBuf = arr.c_str();
        int valStart, valEnd;
        // std::cout << arr << std::endl;
        for(int i=0; i<arr.size(); i++) {

            if(arr[i] == 'p') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // px = arr.substr(valStart+1, valEnd-valStart-1);
                px.buf = &aBuf[valStart+1];
                px.bufEnd = &aBuf[valEnd];
            }
            if(arr[i] == 'q') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // qty = arr.substr(valStart+1, valEnd-valStart-1);
                qty.buf = &aBuf[valStart+1];
                qty.bufEnd = &aBuf[valEnd];
            }
            if(arr[i] == 's') {
                valStart = arr.find('"', arr.find(":", i));
                valEnd = arr.find('"', valStart+1);
                // side = arr.substr(valStart+1, valEnd-valStart-1);
                side.buf = &aBuf[valStart+1];
                side.bufEnd = &aBuf[valEnd];
                std::cout << "|" << side << " for " << px << "@" << qty << " on " << symbol << "|" << std::endl;
                if(arr.find("s", i+1) == std::string::npos) {
                    break;
                }
            }
        }

        // symbol = doc.MemberBegin()->name.GetString();
        // auto innerArr = doc[symbol.c_str()].GetArray();
        // for(auto idx=0; innerArr.Size(); idx++) {
        //     auto data = innerArr[idx].GetObject();
        //     px = data["p"].GetString();
        //     qty = data["q"].GetString();
        //     side = data["s"].GetString();
        //     std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << px << ", " << qty << ", " << side << std::endl;
        // }
    }
}

namespace FormattersPF {

/*
    {"method": "subscribe","ch": "ticker/1s","params": {"symbols": ["ETHBTC","BTCUSDT"]},"id": 123}
*/

    void formatSubscribeTicker(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"ticker/1s\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }

/*
    {"method": "subscribe","ch": "trades","params": {"symbols": ["ETHBTC", "BTCUSDT"],"limit": 1},"id": 123}
*/

    void formatSubscribeTrade(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"trades\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }

/*
{"method": "subscribe","ch": "orderbook/top/1000ms","params": {"symbols": ["ETHBTC", "BTCUSDT"]},"id": 123}
*/

    void formatSubscribeTopOfBook(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id, std::string &bookInterval) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"orderbook/top/" << bookInterval << "\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }

    void formatSubscribeOrderBook(std::ostringstream &oss, std::vector<std::string> &symbols, std::string &id, std::string &bookInterval) {
        oss << "{"
            << "\"method\":\"subscribe\""
            << ",\"ch\":\"orderbook/full\""
            << ",\"params\":{\"symbols\":[";
            for(auto &ele : symbols) {
                oss << '"' << ele << '"' << ",";
            }
            oss << "\b]},";
            oss << "\"id\":" << id
            << "}";
        std::cout << oss.str() << std::endl;
    }
}


namespace SpotGatewayParsers {

    /*
        {"jsonrpc":"2.0","result":true}
    */
    void parseLogin(boost::property_tree::ptree &jsonTree){
        auto result = jsonTree.get<bool>("result");
        if(result) {
            std::cout << "LOGIN SUCCESSFULL" << std::endl;
        }
        else {
            std::cout << "LOGIN FAILED" << std::endl;
        }
    }

    /*
        {"jsonrpc":"2.0","method":"spot_balance","params":[{"currency":"USDT","available":"1999.999999799978","reserved":"0.000000200022","reserved_margin":"0"}]}
    */
    void parseBalance(boost::property_tree::ptree &jsonTree) {
        std::string ccy, balance;
        auto arr = jsonTree.get_child("params");
        for(auto &obj : arr) {
            ccy = obj.second.get<std::string>("currency");
            balance = obj.second.get<std::string>("available");
        }
    }

/*
    {
        "jsonrpc":"2.0",
        "result":{
            "id":19584723200,
            "client_order_id":"harry222",
            "symbol":"BTCUSDT",
            "side":"buy",
            "status":"new",
            "type":"limit",
            "time_in_force":"GTC",
            "quantity":"0.00150",
            "quantity_cumulative":"0",
            "price":"9500.00",
            "post_only":false,
            "created_at":"2022-12-27T10:35:52.737Z",
            "updated_at":"2022-12-27T10:35:52.737Z",
            "report_type":"new"
        },
        "id":123
    }
*/

    void parseNewOrderResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
    }

/*

    {
        "jsonrpc":"2.0",
        "result":{
            "id":19584723200,
            "client_order_id":"harry222",
            "symbol":"BTCUSDT",
            "side":"buy",
            "status":"canceled",
            "type":"limit",
            "time_in_force":"GTC",
            "quantity":"0.00150",
            "quantity_cumulative":"0",
            "price":"9500.00",
            "post_only":false,
            "created_at":"2022-12-27T10:35:52.737Z",
            "updated_at":"2022-12-27T10:36:10.132Z",
            "report_type":"canceled"
        },
        "id":123
    }


*/
    void parseCancelOrderResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
    }

/*

{
    "jsonrpc":"2.0",
    "result": {
        "id":19588546870,
        "client_order_id":"harry223",
        "symbol":"BTCUSDT",
        "side":"buy",
        "status":"new",
        "type":"limit",
        "time_in_force":"GTC",
        "quantity":"0.00001",
        "quantity_cumulative":"0",
        "price":"0.02",
        "post_only":false,
        "created_at":"2022-12-28T07:54:07.961Z",
        "updated_at":"2022-12-28T07:54:30.174Z",
        "report_type":"replaced",
        "original_client_order_id":"harry222"
    },
    "id":123
}

*/

    void parseReplaceResponse(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("result");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("type");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
        result.get<std::string>("report_type");
        result.get<std::string>("original_client_order_id");
    }


/*

{
    "jsonrpc":"2.0",
    "method":"spot_order",
    "params":{
        "id":19592625059,
        "client_order_id":"harry225",
        "symbol":"BTCUSDT",
        "side":"buy",
        "status":"filled",
        "type":"limit",
        "time_in_force":"GTC",
        "quantity":"0.00150",
        "quantity_cumulative":"0.00150",
        "price":"16731.00",
        "post_only":false,
        "created_at":"2022-12-29T10:59:40.65Z",
        "updated_at":"2022-12-29T10:59:40.65Z",
        "report_type":"trade",
        "trade_id":1068784,
        "trade_quantity":"0.00150",
        "trade_price":"16724.16",
        "trade_fee":"0.002704296672",
        "trade_taker":true
    }
}

*/

    void parseExecutionReport(boost::property_tree::ptree &jsonTree) {
        auto result = jsonTree.get_child("params");
        result.get<std::string>("client_order_id");
        result.get<std::string>("symbol");
        result.get<std::string>("side");
        result.get<std::string>("status");
        result.get<std::string>("time_in_force");
        result.get<std::string>("quantity");
        result.get<std::string>("quantity_cumulative");
        result.get<std::string>("price");
        result.get<bool>("trade_taker");
    }

}

namespace formattersSpotGateway {
    void formatLogin(
        std::ostringstream &oss,
        std::string &aPriKey,
        std::string &aPubKey ) {

        oss << "{"
        << R"("method": "login")"
        << R"(,"params": {)"
            << R"("type": "BASIC")"
            << R"(,"api_key": ")" << aPriKey << '"'
            << R"(,"secret_key": ")" << aPubKey << '"'
        << R"(}})";
    }

    void formatNewOrder(
        std::ostringstream &oss,
        std::string &cliOrdId, std::string &symbol,
        std::string &side, std::string &type,
        std::string &quantity, std::string &price,
        std::string &tif, int id) {

        oss << R"({)"
            << R"("method":"spot_new_order")"
            << R"(,"params":{)"
            << R"("client_order_id":")" << cliOrdId << '"'
            << R"(,"symbol":")" << symbol << '"'
            << R"(,"side":")" << side << '"'
            << R"(,"type":)" << type << '"'
            << R"(,"time_in_force":")" << tif << '"'
            << R"(,"quantity":")" << quantity << '"'
            << R"(,"price":")" << price << '"'
            << R"(},)"
            << R"("id":)" << id << '}';
    }

    void formatCancelOrder(
        std::ostringstream &oss,
        std::string &cliOrdId,
        int id) {

        oss << R"({)"
            << R"("method":"spot_cancel_order")"
            << R"(,"params":{)"
            << R"("client_order_id":")" << cliOrdId << '"'
            << R"(},)"
            << R"("id":)" << id
            << R"(})";
    }

    void formatBalanceSubscribe(std::ostringstream &oss, int id) {
        oss << R"({)"
            << R"("method": "spot_balance_subscribe","params": {"mode": "batches"},)"
            << R"("id":)" << id << '}';
    }

    void formatSubscribe(std::ostringstream &oss, int id) {
        oss << "{"
            << R"("method":"spot_subscribe")"
            << R"(,"params":{})"
            << R"(,"id":)" << id << '}';
    }
}



// wss://api.demo.hitbtc.com/api/3/ws/wallet
// {"method": "spot_subscribe","params": {},"id": 123}

// {"method": "subscribe_wallet_balances","params": {},"id": 7653}

// {"method": "login","params": {"type": "BASIC","api_key": "DjHTMhvOLe8QiKaGJ4EXGYQbzfXLqKcC","secret_key": "0e4x1WxctyiFxbvbcGVXuzawNfO2Ruy6"}}

// {"method": "spot_cancel_order","params": {"client_order_id": "harry225"},"id": 123}

// {"method": "spot_balance_subscribe","params": {"mode": "batches"},"id": 123}

// {"method":"spot_get_orders", "id":123}


// wss://api.demo.hitbtc.com/api/3/ws/trading

// {"method": "spot_new_order","params": {"client_order_id": "harry225","symbol": "BTCUSDT","side": "sell","type": "limit", "time_in_force":"GTC", "quantity": "0.0015","price": "16731"},"id": 123}

// {"method": "spot_replace_order","params": {"client_order_id": "harry222","new_client_order_id": "harry223","quantity": "0.00001","price": "0.02"},"id": 123}
