#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "bufrange.hpp" 
#include <sstream>
using namespace rapidjson;
using namespace Parse::data;



/*
trade message
[337,[["20781.60000","0.00017774","1674125899.401130","b","l",""]],"trade","XBT/USD"]
*/
inline void slowParseTrade(const std::string& trade) {
    Document document;
    document.Parse(trade.c_str());
    if(document.IsArray())
    {
        std::cout << document[0].GetInt() << std::endl;

        if(document[1].IsArray()) {

            auto trades = document[1].GetArray();
            for (rapidjson::SizeType i = 0; i < trades.Size(); i++) {

                auto trade =  trades[i].GetArray();
                const char* price = trade[0].GetString();
                const char* volume = trade[1].GetString();
                const char* time = trade[2].GetString();
                const char* side = trade[3].GetString();
                const char* tradeOrderType = trade[4].GetString();
                const char* tradeMiscellaneous = trade[5].GetString();

                std::cout << " Price:" << price << " Volume:" << volume << " Timestamp:" << time
                << " Side:" << side << " TradeOrderType:" << tradeOrderType
                << " TradeMiscellaneuous:" << tradeMiscellaneous << std::endl;

            }
        }
        std::cout<< "ChannelName: "<< document[2].GetString() << std::endl;
        std::cout<< "Pair: "<< document[3].GetString() << std::endl;
    }
}


void GetSymbol(const std::string &aMsg, int &aBeg, int &aEnd) {
    aBeg = aMsg.find_last_of(',');
    aBeg = aMsg.find('"', aBeg)+1;
    aEnd = aMsg.find("\"]", aBeg);
    // ConstBufRange Symbol(&aBuf[symbolS]+1,&aBuf[symbolE]);
    // std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;

}

/*
trade message
[337,[["20781.60000","0.00017774","1674125899.401130","b","l",""]],"trade","XBT/USD"]
*/
inline void fastParseTrade(const std::string& trade) {
    int start = 0;
    int stop = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();


        GetSymbol(trade, start, stop);
        ConstBufRange Symbol(&aBuf[start], &aBuf[stop]);
        std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
        start = 0;
        stop = 0;

        start = trade.find("[");
        stop  = trade.find(",", start);
        ConstBufRange channelID(&aBuf[start]+1,&aBuf[stop]);
        std::cout << "channal ID: " << channelID.asStr() << std::endl;

        start = trade.find("[",start+1);
        stop  = trade.find("]]", start);
        ConstBufRange Trades(&aBuf[start],&aBuf[stop]+2);
        std::string trades = Trades.asStr();
        std::cout << "Trades: " << trades << std::endl;
        while (true) {
        firstTradeIndex = trades.find("[", firstTradeIndex+1);
        lastTradeIndex = trades.find("]", lastTradeIndex+1);
        if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
            // std::cout << "_______inside if statement_____" << std::endl; 

            break;
        }
        // std::cout << "_______After if statement_____" << std::endl; 
        ConstBufRange arr(&trades[firstTradeIndex],&trades[lastTradeIndex]+1);
        std::string trade = arr.asStr();
        std::cout << trade << std::endl; 

        start = trade.find("\"");
        stop  = trade.find("\",", start);
        ConstBufRange price(&trade[start]+1,&trade[stop]);//price
        std::cout << "Price: " << price.asStr() << std::endl;

        start = trade.find("\"", stop+1);
        stop  = trade.find("\",", start);
        ConstBufRange volume(&trade[start]+1,&trade[stop]);//volume
        std::cout << "Volume: " << volume.asStr() << std::endl;

        start = trade.find("\"", stop+1);
        stop  = trade.find("\",", start);
        ConstBufRange timestamp(&trade[start]+1,&trade[stop]);//timestamp
        std::cout << "Timestamp: " << timestamp.asStr() << std::endl;

        start = trade.find("\"", stop+1);
        stop  = trade.find("\",", start);
        ConstBufRange side(&trade[start]+1,&trade[stop]);//side
        std::cout << "Side: " << side.asStr() << std::endl;

        start = trade.find("\"", stop+1);
        stop  = trade.find("\",", start);
        ConstBufRange ordertype(&trade[start]+1,&trade[stop]);//ordertype
        std::cout << "ordertype: " << ordertype.asStr() << std::endl;

        // start = trade.find("\"", stop+1);
        // stop  = trade.find("\",", start);
        // ConstBufRange misc(&trade[start],&trade[stop]);//miscellneuos
        // std::cout << "Misc: " << misc.asStr() << std::endl;
        
    }
    start = trade.find("]]");
    stop  = trade.find("\"", start+4);
    ConstBufRange channelName(&trade[start]+4,&trade[stop]);//channel name
    std::cout << "Channel name: " << channelName.asStr() << std::endl;
    start = trade.find(",\"",stop);
    stop  = trade.find("\"", start+2);
    ConstBufRange pairName(&trade[start]+2,&trade[stop]);//channel name
    std::cout << "Pair name: " << pairName.asStr() << std::endl;

}


/*
SnapShot msg
[
    352,{"as":[["19203.00000","0.04081562","1674137732.716713"],["19204.90000","0.18660258","1674137731.782711"]],
    "bs":[["19199.60000","0.06471580","1674137732.614654"],["19199.50000","0.04081562","1674137732.864419"]]},
    "book-10","XBT/EUR"
]

*/

inline void fastParseSnapShot(const std::string& trade) {
    int start = 0;
    int stop = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();

    GetSymbol(trade, start, stop);
    ConstBufRange Symbol(&aBuf[start], &aBuf[stop]);
    std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
    start = 0;
    stop = 0;

    if(trade.find("\"as\":") != std::string::npos || trade.find("\"bs\":") != std::string::npos) {
        start = trade.find("[");
        stop  = trade.find(",", start);
        ConstBufRange channelID(&aBuf[start]+1,&aBuf[stop]);
        std::cout << "channal ID: " << channelID.asStr() << std::endl;
        start = trade.find("{");
        stop = trade.find(":",start+1);
        ConstBufRange Side(&aBuf[start]+2,&aBuf[stop]-1);
        std::cout << "Side "<< Side.asStr() << std::endl;
        start = trade.find("[",start+1);
        stop  = trade.find("]]", start);
        ConstBufRange Trades(&aBuf[start],&aBuf[stop]+2);
        std::string trades = Trades.asStr();
        std::cout << "Trades: " << trades << std::endl;
        while (true) {
            firstTradeIndex = trades.find("[", firstTradeIndex+1);
            lastTradeIndex = trades.find("]", lastTradeIndex+1); 
            if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
                // std::cout << "_______inside if statement_____" << std::endl; 
                break;
            }
            // std::cout << "_______After if statement_____" << std::endl; 
            ConstBufRange arr(&trades[firstTradeIndex],&trades[lastTradeIndex]+1);
            std::string trade = arr.asStr();
            std::cout << trade << std::endl; 

            start = trade.find("\"");
            stop  = trade.find("\",", start);
            ConstBufRange price(&trade[start]+1,&trade[stop]);//price
            std::cout << "Price: " << price.asStr() << std::endl;

            start = trade.find("\"", stop+1);
            stop  = trade.find("\",", start);
            ConstBufRange volume(&trade[start]+1,&trade[stop]);//volume
            std::cout << "Volume: " << volume.asStr() << std::endl;

            start = trade.find("\"", stop+1);
            stop  = trade.find("\"", start+1);
            ConstBufRange timestamp(&trade[start]+1,&trade[stop]);//timestamp
            std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
        }
        start = trade.find("]],");
        stop = trade.find(":",start+1);
        ConstBufRange Side_(&aBuf[start]+4,&aBuf[stop]-1);
        std::cout << "Side "<< Side_.asStr() << std::endl;
        start = trade.find("[",start+1);
        stop  = trade.find("]]", start);
        ConstBufRange Trades_(&aBuf[start],&aBuf[stop]+2);
        std::string trades_ = Trades_.asStr();
        std::cout << "Trades: " << trades_ << std::endl;
        firstTradeIndex = 0;
        lastTradeIndex = 0;
        while (true) {
            firstTradeIndex = trades_.find("[", firstTradeIndex+1);
            lastTradeIndex = trades_.find("]", lastTradeIndex+1); 
            if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
                // std::cout << "_______inside if statement_____" << std::endl; 
                break;
            }
            // std::cout << "_______After if statement_____" << std::endl; 
            ConstBufRange arr(&trades_[firstTradeIndex],&trades_[lastTradeIndex]+1);
            std::string trade_ = arr.asStr();
            std::cout << trade_ << std::endl; 

            start = trade_.find("\"");
            stop  = trade_.find("\",", start);
            ConstBufRange price(&trade_[start]+1,&trade_[stop]);//price
            std::cout << "Price: " << price.asStr() << std::endl;

            start = trade_.find("\"", stop+1);
            stop  = trade_.find("\",", start);
            ConstBufRange volume(&trade_[start]+1,&trade_[stop]);//volume
            std::cout << "Volume: " << volume.asStr() << std::endl;

            start = trade_.find("\"", stop+1);
            stop  = trade_.find("\"", start+1);
            ConstBufRange timestamp(&trade_[start]+1,&trade_[stop]);//timestamp
            std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
        }
    start = trade.find("},");
    stop  = trade.find("\"", start+4);
    ConstBufRange channelName(&trade[start]+3,&trade[stop]);//channel name
    std::cout << "Channel name: " << channelName.asStr() << std::endl;
    start = trade.find(",\"",stop);
    stop  = trade.find("\"", start+2);
    ConstBufRange pairName(&trade[start]+2,&trade[stop]);//channel name
    std::cout << "Pair name: " << pairName.asStr() << std::endl;
    }

}


inline void slowParseSnapShot(const std::string& trade) {
    Document document;
    document.Parse(trade.c_str());
    if(document.IsArray())
    {
        std::cout << "Channel ID :" << document[0].GetInt() << std::endl;
        auto data = document[1].GetObject();

        if(data.HasMember("as")) {

            auto trades = data["as"].GetArray();
            for (rapidjson::SizeType i = 0; i < trades.Size(); i++) {

                auto trade =  trades[i].GetArray();
                const char* price = trade[0].GetString();
                const char* volume = trade[1].GetString();
                const char* time = trade[2].GetString();
                

                std::cout << " Price:" << price << " Volume:" << volume << " Timestamp:" << time << std::endl;
            }
        }
        if(data.HasMember("bs")) {

            auto trades = data["bs"].GetArray();
            for (rapidjson::SizeType i = 0; i < trades.Size(); i++) {

                auto trade =  trades[i].GetArray();
                const char* price = trade[0].GetString();
                const char* volume = trade[1].GetString();
                const char* time = trade[2].GetString();
                

                std::cout << " Price:" << price << " Volume:" << volume << " Timestamp:" << time << std::endl;
            }
        }
        std::cout<< "ChannelName: "<< document[2].GetString() << std::endl;
        std::cout<< "Pair: "<< document[3].GetString() << std::endl;
    }
}


/*
level update msg

[352,{"b":[["19196.10000","0.00000000","1674137733.183032"],["19194.00000","0.18676326","1674137727.207292","r"]],
"c":"2186911512"},
"book-10","XBT/EUR"]

[352,{"a":[["19203.10000","0.00000000","1674137733.209939"],["19204.90000","0.18660258","1674137731.782711","r"]],
"c":"1680327067"},
"book-10","XBT/EUR"]

*/

inline void fastParseLevelUpdate(const std::string& trade) {
    int start = 0;
    int stop = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();

    GetSymbol(trade, start, stop);
    ConstBufRange Symbol(&aBuf[start], &aBuf[stop]);
    std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
    start = 0;
    stop = 0;

    if(trade.find("\"a\":") != std::string::npos || trade.find("\"b\":") != std::string::npos) {
        start = trade.find("[");
        stop  = trade.find(",", start);
        ConstBufRange channelID(&aBuf[start]+1,&aBuf[stop]);
        std::cout << "channal ID: " << channelID.asStr() << std::endl;
        start = trade.find("[",start+1);
        stop  = trade.find("]]", start);
        ConstBufRange Asks(&aBuf[start],&aBuf[stop]+2);
        std::string trades = Asks.asStr();
        start = trade.find("{");
        stop = trade.find(":",start+1);
        ConstBufRange Side(&aBuf[start]+2,&aBuf[stop]-1);
        std::cout << "Side "<< Side.asStr() <<" : "<< trades << std::endl;
            while (true) {
                firstTradeIndex = trades.find("[", firstTradeIndex+1);
                lastTradeIndex = trades.find("]", lastTradeIndex+1);
 
                if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
                    // std::cout << "_______inside if statement_____" << std::endl; 
                    break;
                }
                // std::cout << "_______After if statement_____" << std::endl; 
                ConstBufRange arr(&trades[firstTradeIndex],&trades[lastTradeIndex]+1);
                std::string trade = arr.asStr();
                std::cout << trade << std::endl; 
                start = trade.find("\"");
                stop  = trade.find("\",", start);
                ConstBufRange price(&trade[start]+1,&trade[stop]);//price
                std::cout << "Price: " << price.asStr() << std::endl;
                start = trade.find("\"", stop+1);
                stop  = trade.find("\",", start);
                ConstBufRange volume(&trade[start]+1,&trade[stop]);//volume
                std::cout << "Volume: " << volume.asStr() << std::endl;
                
                if(trade.find("r") != std::string::npos) {
                    // std::cout << "____inside if____" << std::endl;
                    start = trade.find("\"", stop+1);
                    stop  = trade.find("\",", start);
                    ConstBufRange timestamp(&trade[start]+1,&trade[stop]);//timestamp
                    std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
                    start = trade.find("\"", stop+1);
                    stop  = trade.find("\"]", start);
                    ConstBufRange updateType(&trade[start]+1,&trade[stop]);//updateType
                    std::cout << "update type: " << updateType.asStr() << std::endl;
                }
                else {
                    // std::cout << "____inside Else____" << std::endl;
                    start = trade.find("\"", stop+1);
                    stop  = trade.find("\"]", start);
                    ConstBufRange timestamp(&trade[start]+1,&trade[stop]);//timestamp
                    std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
                }
            }
        start = trade.find("},");
        stop  = trade.find("\"", start+4);
        ConstBufRange channelName(&trade[start]+3,&trade[stop]);//channel name
        std::cout << "Channel name: " << channelName.asStr() << std::endl;
        start = trade.find(",\"",stop);
        stop  = trade.find("\"", start+2);
        ConstBufRange pairName(&trade[start]+2,&trade[stop]);//channel name
        std::cout << "Pair name: " << pairName.asStr() << std::endl;
    }
}    

inline void slowParseLevelUpdate(const std::string& trade) {
    Document document;
    document.Parse(trade.c_str());
    if(document.IsArray())
    {
        std::cout << document[0].GetInt() << std::endl;
        auto data = document[1].GetObject();

        if(data.HasMember("b")) {

            auto trades = data["b"].GetArray();
            for (rapidjson::SizeType i = 0; i < trades.Size(); i++) {

                auto trade =  trades[i].GetArray();
                const char* price = trade[0].GetString();
                const char* volume = trade[1].GetString();
                const char* time = trade[2].GetString();
                
                std::cout << " Price:" << price << " Volume:" << volume << " Timestamp:" << time
                << std::endl;
            }
        }
        if(data.HasMember("a")) {

            auto trades = data["a"].GetArray();
            for (rapidjson::SizeType i = 0; i < trades.Size(); i++) {

                auto trade =  trades[i].GetArray();
                const char* price = trade[0].GetString();
                const char* volume = trade[1].GetString();
                const char* time = trade[2].GetString();
                
                std::cout << " Price:" << price << " Volume:" << volume << " Timestamp:" << time
                << std::endl;
            }
        }
        std::cout<< "ChannelName: "<< document[2].GetString() << std::endl;
        std::cout<< "Pair: "<< document[3].GetString() << std::endl;
    }
}


/*
ticker
[340,{"a":["23249.40000",0,"0.97783032"],"b":["23248.80000",0,"0.40225246"],"c":["23249.40000","0.02216968"],"v":["4388.68858896",
"7030.45503793"],"p":["22905.52706","22644.05398"],"t":[29214,45425],"l":["22440.30000","21370.10000"],"h":["23428.60000","23428.60000"],
"o":["22682.10000","21385.40000"]},"ticker","XBT/USD"]
*/

inline void fastParseTicker(const std::string& trade) {
    int start = 0;
    int stop = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();

    GetSymbol(trade, start, stop);
    ConstBufRange Symbol(&aBuf[start], &aBuf[stop]);
    std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
    start = 0;
    stop = 0;


    start = trade.find("[");
    stop  = trade.find(",", start);
    ConstBufRange channelID(&aBuf[start]+1,&aBuf[stop]);
    std::cout << "channal ID: " << channelID.asStr() << std::endl;
    
    start = trade.find("{",start);
    stop  = trade.find("}", start);
    ConstBufRange ticker(&aBuf[start],&aBuf[stop]+2);
    std::string trades = ticker.asStr();
    std::cout << "ticker data: " << trades << std::endl;
    while (true) {
        firstTradeIndex = trades.find("\":[", firstTradeIndex+1);
        lastTradeIndex = trades.find("]", lastTradeIndex+1);
        if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
            // std::cout << "_______inside if statement_____" << std::endl; 
            break;
        }
        // std::cout << "_______After if statement_____" << std::endl; 
        ConstBufRange arr(&trades[firstTradeIndex]-2,&trades[lastTradeIndex]+1);
        std::string trade = arr.asStr();
        std::cout << trade << std::endl; 
        if(trade.find("t") == std::string::npos) {
            start = trade.find("[\"");
            stop  = trade.find("\",", start);
            ConstBufRange price(&trade[start]+2,&trade[stop]);//price
            std::cout << "Price: " << price.asStr() << std::endl;
            if(trade.find("a") != std::string::npos || trade.find("b") != std::string::npos) {
                start = trade.find(",", stop+1);
                stop  = trade.find(",", start+1);
                ConstBufRange wholeLotVolume(&trade[start]+1,&trade[stop]);//wholeLotVolume
                std::cout << "wholeLotVolume: " << atoi(wholeLotVolume.asStr().c_str()) << std::endl;
            }
            start = trade.find("\"", stop+1);
            stop  = trade.find("\"]", start);
            ConstBufRange LotVolume(&trade[start]+1,&trade[stop]);//LotVolume
            std::cout << "LotVolume: " << LotVolume.asStr() << std::endl;
        }
        if(trade.find("t") != std::string::npos) {
            start = trade.find("[");
            stop  = trade.find(",", start);
            ConstBufRange today(&trade[start]+1,&trade[stop]+1);//price
            std::cout << "today : " << atoi(today.asStr().c_str()) << std::endl;
            start = trade.find(",");
            stop  = trade.find("]", start);
            ConstBufRange last24hour(&trade[start]+1,&trade[stop]+1);//price
            std::cout << "last 24 hours: " << atoi(last24hour.asStr().c_str()) << std::endl;

        }
       
    }
    start = trade.find("},");
    stop  = trade.find("\"", start+4);
    ConstBufRange channelName(&trade[start]+3,&trade[stop]);//channel name
    std::cout << "Channel name: " << channelName.asStr() << std::endl;
    start = trade.find(",\"",stop);
    stop  = trade.find("\"", start+2);
    ConstBufRange pairName(&trade[start]+2,&trade[stop]);//channel name
    std::cout << "Pair name: " << pairName.asStr() << std::endl;
}
   

inline void slowParseTicker(const std::string& trade) {
    Document document;
    document.Parse(trade.c_str());
    if(document.IsArray())
    {
        std::cout << document[0].GetInt() << std::endl;
        auto data = document[1].GetObject();

        if(data.HasMember("a")) {

            auto trade = data["a"].GetArray();
            const char* price = trade[0].GetString();
            const int wholeLotVolume = trade[1].GetInt();
            const char* lotVolume = trade[2].GetString();
            
            std::cout << " Price:" << price << " wholeLotVolume:" << wholeLotVolume << " lotVolume:" << lotVolume << std::endl;
        }
        if(data.HasMember("b")) {
        
            auto trade = data["b"].GetArray();
            const char* price = trade[0].GetString();
            const int wholeLotVolume = trade[1].GetInt();
            const char* lotVolume = trade[2].GetString();
            
            std::cout << " Price:" << price << " wholeLotVolume:" << wholeLotVolume << " lotVolume:" << lotVolume << std::endl;
        }
        if(data.HasMember("c")) {
        
            auto trade = data["c"].GetArray();
            const char* price = trade[0].GetString();
            const char* lotVolume = trade[1].GetString();
            
            std::cout << " Price:" << price << " lotVolume:" << lotVolume << std::endl;
        }
        if(data.HasMember("v")) {
        
            auto trade = data["v"].GetArray();
            const char* today = trade[0].GetString();
            const char* last24Hours = trade[1].GetString();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;
        }
        if(data.HasMember("p")) {
        
            auto trade = data["p"].GetArray();
            const char* today = trade[0].GetString();
            const char* last24Hours = trade[1].GetString();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;;
        }
        if(data.HasMember("t")) {
        
            auto trade = data["t"].GetArray();
            const double today = trade[0].GetDouble();
            const double last24Hours = trade[1].GetDouble();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;
        }
        if(data.HasMember("l")) {
        
            auto trade = data["l"].GetArray();
            const char* today = trade[0].GetString();
            const char* last24Hours = trade[1].GetString();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;
        }
        if(data.HasMember("h")) {
        
            auto trade = data["h"].GetArray();
            const char* today = trade[0].GetString();
            const char* last24Hours = trade[1].GetString();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;
        }
        if(data.HasMember("o")) {
        
            auto trade = data["o"].GetArray();
            const char* today = trade[0].GetString();
            const char* last24Hours = trade[1].GetString();
            
            std::cout << " Today:" << today << " Last24Hours:" << last24Hours << std::endl;
        }        
        std::cout<< "ChannelName: "<< document[2].GetString() << std::endl;
        std::cout<< "Pair: "<< document[3].GetString() << std::endl;
    }
}