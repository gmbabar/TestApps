#include <iostream>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "bufrange.hpp" 
#include <sstream>
using namespace rapidjson;
using namespace parser::data;


inline
bool getNextJsonArrayValue(
        const char *&cptr,
        const char *cend,
        std::string &token,
        bool &hasMoreValues) {
    // Check if more values at same level.
    auto HasMoreValues = [](const char *cptr, const char *cend) {
        if(*cptr == ',')
            ++cptr;
        bool more = false, done = false;
        while (cptr <= cend && !done) {
            switch(*cptr) {
            case ',':
                more = true;
                done = true;
                break;
            case '[':
            case ']':
                done = true;
                break;
            default:
                if(!isspace(*cptr)) {
                    more = true;
                    done = true;
                }
                break;
            }
            ++cptr;
        }
        return more;
    };

    hasMoreValues = false;
    //skip delims
    while (*cptr !=0 && (*cptr==',' || *cptr==' ' || *cptr=='[' || *cptr==']'))
        ++cptr;
    if (*cptr == 0)
        return false;

    const char *begin = cptr++;
    while(cptr <= cend && *cptr !=',' && *cptr !='[' && *cptr !=']')
        ++cptr;

    if (cptr <= cend) {
        hasMoreValues = HasMoreValues(cptr, cend);
        token = std::string(begin, cptr-begin);
        ++cptr;
        return true;
    }

    return false;
}

//inline
bool isJsonArray(const std::string &aMsg) {
    size_t len = aMsg.length();
    char ch;
    size_t idx = 0;
    while (idx < len) {
        ch = aMsg[idx++];
        if (::isspace(ch)) continue;
        if (ch == '[') return true;      // should be first char
        break;
    }
    return false;
}

// //inline
// bool isJsonArray(const std::string &aBuf) {
//     size_t len = aBuf.size();
//     char ch;
//     size_t idx = 0;
//     while (idx < len) {
//         ch = aBuf[idx++];
//         if (::isspace(ch)) continue;
//         if (ch == '[') return true;      // should be first char
//         break;
//     }
//     return false;
// }

// inline

bool getJsonKey(
        const std::string &aBuf,
        const std::string& aKey,
        size_t &aBegin,
        size_t &aEnd) {
    auto pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
    if (pos == std::string::npos)
        return false;
    aBegin = pos;
    aEnd = pos + aKey.size();
    return true;
}

// inline
bool getJsonStrVal(
        const std::string &aBuf,
        const std::string& aKey,
        size_t &aBegin,
        size_t &aEnd) {
    size_t pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
    if (pos == std::string::npos) 
        return false;
    pos += aKey.size();
    pos = aBuf.find(':', pos);
    if (pos == std::string::npos) 
        return false;
    aBegin = aBuf.find('"', pos)+1;
    if (aBegin == std::string::npos) 
        return false;
    aEnd = aBuf.find('"', aBegin);
    if (aEnd == std::string::npos) 
        return false;
    return true;
}

// inline
bool getJsonNumVal(
        const std::string &aBuf,
        const std::string& aKey,
        size_t &aBegin,
        size_t &aEnd) {
    size_t pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
    if (pos == std::string::npos) 
        return false;
    pos += aKey.size();
    pos = aBuf.find(':', pos);
    if (pos == std::string::npos) 
        return false;
    aBegin = pos+1;
    aEnd = pos;
    size_t len = aBuf.size();
    while(++aEnd < len && (isdigit(aBuf[aEnd]) || aBuf[aEnd] == '.'));
    return aEnd > aBegin;
}



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



/*
trade message
[337,[["20781.60000","0.00017774","1674125899.401130","b","l",""]],"trade","XBT/USD"]
*/
inline void fastParseTrade(const std::string& trade) {
    int beg = 0;
    int end = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();


        // GetSymbol(trade, beg, end);
        // ConstBufRange Symbol(&aBuf[beg], &aBuf[end]);
        // std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
        beg = 0;
        end = 0;

        beg = trade.find("[");
        end  = trade.find(",", beg);
        ConstBufRange channelID(&aBuf[beg]+1,&aBuf[end]);
        std::cout << "channal ID: " << channelID.asStr() << std::endl;

        beg = trade.find("[",beg+1);
        end  = trade.find("]]", beg);
        ConstBufRange Trades(&aBuf[beg],&aBuf[end]+2);
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

        beg = trade.find("\"");
        end  = trade.find("\",", beg);
        ConstBufRange price(&trade[beg]+1,&trade[end]);//price
        std::cout << "Price: " << price.asStr() << std::endl;

        beg = trade.find("\"", end+1);
        end  = trade.find("\",", beg);
        ConstBufRange volume(&trade[beg]+1,&trade[end]);//volume
        std::cout << "Volume: " << volume.asStr() << std::endl;

        beg = trade.find("\"", end+1);
        end  = trade.find("\",", beg);
        ConstBufRange timestamp(&trade[beg]+1,&trade[end]);//timestamp
        std::cout << "Timestamp: " << timestamp.asStr() << std::endl;

        beg = trade.find("\"", end+1);
        end  = trade.find("\",", beg);
        ConstBufRange side(&trade[beg]+1,&trade[end]);//side
        std::cout << "Side: " << side.asStr() << std::endl;

        beg = trade.find("\"", end+1);
        end  = trade.find("\",", beg);
        ConstBufRange ordertype(&trade[beg]+1,&trade[end]);//ordertype
        std::cout << "ordertype: " << ordertype.asStr() << std::endl;

        // beg = trade.find("\"", end+1);
        // end  = trade.find("\",", beg);
        // ConstBufRange misc(&trade[beg],&trade[end]);//miscellneuos
        // std::cout << "Misc: " << misc.asStr() << std::endl;
        
    }
    beg = trade.find("]]");
    end  = trade.find("\"", beg+4);
    ConstBufRange channelName(&trade[beg]+4,&trade[end]);//channel name
    std::cout << "Channel name: " << channelName.asStr() << std::endl;
    beg = trade.find(",\"",end);
    end  = trade.find("\"", beg+2);
    ConstBufRange pairName(&trade[beg]+2,&trade[end]);//channel name
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
    int beg = 0;
    int end = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();

    if(trade.find("\"as\":") != std::string::npos || trade.find("\"bs\":") != std::string::npos) {
        beg = trade.find("[");
        end  = trade.find(",", beg);
        // ConstBufRange channelID(&aBuf[beg]+1,&aBuf[end]);
        // std::cout << "channal ID: " << channelID.asStr() << std::endl;
        beg = trade.find("{");
        end = trade.find(":",beg+1);
        ConstBufRange Side(&aBuf[beg]+2,&aBuf[end]-1);
        std::cout << "Side "<< Side.asStr() << std::endl;
        beg = trade.find("[",beg+1);
        end  = trade.find("]]", beg);
        ConstBufRange Trades(&aBuf[beg],&aBuf[end]+2);
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

            beg = trade.find("\"");
            end  = trade.find("\",", beg);
            ConstBufRange price(&trade[beg]+1,&trade[end]);//price
            std::cout << "Price: " << price.asStr() << std::endl;

            beg = trade.find("\"", end+1);
            end  = trade.find("\",", beg);
            ConstBufRange volume(&trade[beg]+1,&trade[end]);//volume
            std::cout << "Volume: " << volume.asStr() << std::endl;

            // beg = trade.find("\"", end+1);
            // end  = trade.find("\"", beg+1);
            // ConstBufRange timestamp(&trade[beg]+1,&trade[end]);//timestamp
            // std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
        }
        beg = trade.find("]],");
        end = trade.find(":",beg+1);
        ConstBufRange Side_(&aBuf[beg]+4,&aBuf[end]-1);
        std::cout << "Side "<< Side_.asStr() << std::endl;
        beg = trade.find("[",beg+1);
        end  = trade.find("]]", beg);
        ConstBufRange Trades_(&aBuf[beg],&aBuf[end]+2);
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

            beg = trade_.find("\"");
            end  = trade_.find("\",", beg);
            ConstBufRange price(&trade_[beg]+1,&trade_[end]);//price
            std::cout << "Price: " << price.asStr() << std::endl;

            beg = trade_.find("\"", end+1);
            end  = trade_.find("\",", beg);
            ConstBufRange volume(&trade_[beg]+1,&trade_[end]);//volume
            std::cout << "Volume: " << volume.asStr() << std::endl;

            // beg = trade_.find("\"", end+1);
            // end  = trade_.find("\"", beg+1);
            // ConstBufRange timestamp(&trade_[beg]+1,&trade_[end]);//timestamp
            // std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
        }
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

// inline void fastParseLevelUpdate(const std::string& trade) {
//     int beg = 0;
//     int end = 0;
//     int firstTradeIndex = 0;
//     int lastTradeIndex = 0;
//     const char *aBuf = trade.c_str();

//     // GetSymbol(trade, beg, end);
//     // ConstBufRange Symbol(&aBuf[beg], &aBuf[end]);
//     // std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
//     beg = 0;
//     end = 0;

//     if(trade.find("\"a\":") != std::string::npos || trade.find("\"b\":") != std::string::npos) {
//         beg = trade.find("[");
//         end  = trade.find(",", beg);
//         ConstBufRange channelID(&aBuf[beg]+1,&aBuf[end]);
//         std::cout << "channal ID: " << channelID.asStr() << std::endl;
//         beg = trade.find("[",beg+1);
//         end  = trade.find("]]", beg);
//         ConstBufRange Asks(&aBuf[beg],&aBuf[end]+2);
//         std::string trades = Asks.asStr();
//         beg = trade.find("{");
//         end = trade.find(":",beg+1);
//         ConstBufRange Side(&aBuf[beg]+2,&aBuf[end]-1);
//         std::cout << "Side "<< Side.asStr() <<" : "<< trades << std::endl;
//             while (true) {
//                 firstTradeIndex = trades.find("[", firstTradeIndex+1);
//                 lastTradeIndex = trades.find("]", lastTradeIndex+1);
 
//                 if(firstTradeIndex == std::string::npos || lastTradeIndex == std::string::npos ) {
//                     // std::cout << "_______inside if statement_____" << std::endl; 
//                     break;
//                 }
//                 // std::cout << "_______After if statement_____" << std::endl; 
//                 ConstBufRange arr(&trades[firstTradeIndex],&trades[lastTradeIndex]+1);
//                 std::string trade = arr.asStr();
//                 std::cout << trade << std::endl; 
//                 beg = trade.find("\"");
//                 end  = trade.find("\",", beg);
//                 ConstBufRange price(&trade[beg]+1,&trade[end]);//price
//                 std::cout << "Price: " << price.asStr() << std::endl;
//                 beg = trade.find("\"", end+1);
//                 end  = trade.find("\",", beg);
//                 ConstBufRange volume(&trade[beg]+1,&trade[end]);//volume
//                 std::cout << "Volume: " << volume.asStr() << std::endl;
                
//                 if(trade.find("r") != std::string::npos) {
//                     // std::cout << "____inside if____" << std::endl;
//                     beg = trade.find("\"", end+1);
//                     end  = trade.find("\",", beg);
//                     ConstBufRange timestamp(&trade[beg]+1,&trade[end]);//timestamp
//                     std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
//                     beg = trade.find("\"", end+1);
//                     end  = trade.find("\"]", beg);
//                     ConstBufRange updateType(&trade[beg]+1,&trade[end]);//updateType
//                     std::cout << "update type: " << updateType.asStr() << std::endl;
//                 }
//                 else {
//                     // std::cout << "____inside Else____" << std::endl;
//                     beg = trade.find("\"", end+1);
//                     end  = trade.find("\"]", beg);
//                     ConstBufRange timestamp(&trade[beg]+1,&trade[end]);//timestamp
//                     std::cout << "Timestamp: " << timestamp.asStr() << std::endl;
//                 }
//             }
//         beg = trade.find("},");
//         end  = trade.find("\"", beg+4);
//         ConstBufRange channelName(&trade[beg]+3,&trade[end]);//channel name
//         std::cout << "Channel name: " << channelName.asStr() << std::endl;
//         beg = trade.find(",\"",end);
//         end  = trade.find("\"", beg+2);
//         ConstBufRange pairName(&trade[beg]+2,&trade[end]);//channel name
//         std::cout << "Pair name: " << pairName.asStr() << std::endl;
//     }
// }


inline void fastParseBookUpdate(const std::string& trade) {
    int beg = 0;
    int end = 0;
    int fieldStart = 0;
    int fieldEnd = 0;


    if (trade.find("\"a\"") != std::string::npos) {
        beg = trade.find("\"a\"")+4;
        end  = trade.find("]]", beg)+1;
        while (true) {
            std::cout << "----------------------------------------------------------------------" << std::endl;
            fieldStart = trade.find("[", beg);
            if(fieldStart >= end || fieldStart == std::string::npos)
                break;
            // fieldEnd = trade.find(',', fieldStart);
            // fieldEnd = trade.find(',', fieldEnd+1);

            // const auto pxBeg = trade.find('"', fieldStart);
            // const auto pxEnd = trade.find('"', pxBeg+1);
            // std::cout << trade.substr(pxBeg+1, pxEnd-pxBeg-1) << std::endl; //price
            // const auto volBeg = trade.find('"', pxEnd+1);
            // const auto volEnd = trade.find('"', volBeg+1);
            // std::cout << trade.substr(volBeg+1, volEnd-volBeg-1) << std::endl; //volume
            // beg = fieldEnd+1;

            fieldStart = trade.find('"', fieldStart)+1;
            fieldEnd = trade.find('"', fieldStart);
            std::cout << trade.substr(fieldStart, fieldEnd-fieldStart) << std::endl; //price
            fieldStart = trade.find('"', fieldEnd+1)+1;
            fieldEnd = trade.find('"', fieldStart);
            std::cout << trade.substr(fieldStart, fieldEnd-fieldStart) << std::endl; //volume
            beg = fieldEnd+1;
        }
    }
    if (trade.find("\"b\"", end) != std::string::npos) {
        beg = trade.find("\"b\"", end)+4;
        end  = trade.find("]]", beg)+1;

        while (true) {
            std::cout << "----------------------------------------------------------------------" << std::endl;
            fieldStart = trade.find("[", beg);
            if(fieldStart >= end || fieldStart == std::string::npos)
                break;

            // fieldEnd = trade.find(',', fieldStart);
            // fieldEnd = trade.find(',', fieldEnd+1);
            // const auto pxBeg = trade.find('"', fieldStart);
            // const auto pxEnd = trade.find('"', pxBeg+1);
            // std::cout << trade.substr(pxBeg+1, pxEnd-pxBeg-1) << std::endl; //price
            // const auto volBeg = trade.find('"', pxEnd+1);
            // const auto volEnd = trade.find('"', volBeg+1);
            // std::cout << trade.substr(volBeg+1, volEnd-volBeg-1) << std::endl; //volume
            // beg = fieldEnd+1;

            fieldStart = trade.find('"', fieldStart)+1;
            fieldEnd = trade.find('"', fieldStart);
            std::cout << trade.substr(fieldStart, fieldEnd-fieldStart) << std::endl; //price
            fieldStart = trade.find('"', fieldEnd+1)+1;
            fieldEnd = trade.find('"', fieldStart);
            std::cout << trade.substr(fieldStart, fieldEnd-fieldStart) << std::endl; //volume
            beg = fieldEnd+1;

        }
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
    int beg = 0;
    int end = 0;
    int firstTradeIndex = 0;
    int lastTradeIndex = 0;
    const char *aBuf = trade.c_str();

    // GetSymbol(trade, beg, end);
    // ConstBufRange Symbol(&aBuf[beg], &aBuf[end]);
    // std::cout << __FILE__ << ':' << __LINE__ << ':' << __func__ << ':' << Symbol.asStr() << std::endl;
    beg = 0;
    end = 0;


    beg = trade.find("[");
    end  = trade.find(",", beg);
    ConstBufRange channelID(&aBuf[beg]+1,&aBuf[end]);
    std::cout << "channal ID: " << channelID.asStr() << std::endl;
    
    beg = trade.find("{",beg);
    end  = trade.find("}", beg);
    ConstBufRange ticker(&aBuf[beg],&aBuf[end]+2);
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
            beg = trade.find("[\"");
            end  = trade.find("\",", beg);
            ConstBufRange price(&trade[beg]+2,&trade[end]);//price
            std::cout << "Price: " << price.asStr() << std::endl;
            if(trade.find("a") != std::string::npos || trade.find("b") != std::string::npos) {
                beg = trade.find(",", end+1);
                end  = trade.find(",", beg+1);
                ConstBufRange wholeLotVolume(&trade[beg]+1,&trade[end]);//wholeLotVolume
                std::cout << "wholeLotVolume: " << atoi(wholeLotVolume.asStr().c_str()) << std::endl;
            }
            beg = trade.find("\"", end+1);
            end  = trade.find("\"]", beg);
            ConstBufRange LotVolume(&trade[beg]+1,&trade[end]);//LotVolume
            std::cout << "LotVolume: " << LotVolume.asStr() << std::endl;
        }
        if(trade.find("t") != std::string::npos) {
            beg = trade.find("[");
            end  = trade.find(",", beg);
            ConstBufRange today(&trade[beg]+1,&trade[end]+1);//price
            std::cout << "today : " << atoi(today.asStr().c_str()) << std::endl;
            beg = trade.find(",");
            end  = trade.find("]", beg);
            ConstBufRange last24hour(&trade[beg]+1,&trade[end]+1);//price
            std::cout << "last 24 hours: " << atoi(last24hour.asStr().c_str()) << std::endl;

        }
       
    }
    beg = trade.find("},");
    end  = trade.find("\"", beg+4);
    ConstBufRange channelName(&trade[beg]+3,&trade[end]);//channel name
    std::cout << "Channel name: " << channelName.asStr() << std::endl;
    beg = trade.find(",\"",end);
    end  = trade.find("\"", beg+2);
    ConstBufRange pairName(&trade[beg]+2,&trade[end]);//channel name
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