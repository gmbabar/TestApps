
#include <chrono>
#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/MessageHeader.h"
#include "deribit_multicast/Book.h"
#include "deribit_multicast/Instrument.h"
#include "deribit_multicast/Snapshot.h"
#include "deribit_multicast/Ticker.h"
#include "deribit_multicast/Trades.h"

void printBuffer(const char *buffer, size_t size) {
   std::cout << "Output data:" << std::endl;
   for (int idx=0; idx<size; ++idx) 
       printf("%.02x ", static_cast<unsigned char>(buffer[idx]));
   printf("\n");
}

using namespace deribit_multicast;

void FormBfcSym(Instrument &instrument) {

    std::ostringstream symbol;
    std::chrono::duration<uint64_t,std::milli> milliDtn(instrument.expirationTimestampMs());
    time_t seconds = std::chrono::duration_cast<std::chrono::seconds> (milliDtn).count();
    struct tm *tms = localtime(&seconds);

    if(instrument.kind() == InstrumentKind::option) {

        symbol << "R_T_" << instrument.baseCurrency() << "_" << instrument.counterCurrency() << "_" << std::setfill('0')
                << std::setw(2) << tms->tm_year % 100 << std::setw(2) << tms->tm_mon << std::setw(2) << tms->tm_mday << "_"
                << instrument.strikePrice()
                << (instrument.optionType() == OptionType::put ? "_P" : "_C");

    }
    else if(instrument.kind() == InstrumentKind::future && instrument.settlementPeriod() == Period::perpetual){
            symbol << "R_" << (instrument.futureType() == FutureType::reversed ? "I_" : "T_") << instrument.baseCurrency() << "Perpetual";
    }
    else if(instrument.kind() == InstrumentKind::future) {

        symbol << "R_" << (instrument.futureType() == FutureType::reversed ? "I_" : "T_") << instrument.baseCurrency() << "_" << instrument.counterCurrency() << "_" 
                << std::setfill('0') << std::setw(2) << tms->tm_year % 100 << std::setw(2) << tms->tm_mon << std::setw(2) << tms->tm_mday << "_"
                << instrument.strikePrice();
    } 
    else {
        std::cerr << "Invalid Instrument Kind" << std::endl;
        return;
    }

    std::cout << std::endl << symbol.str() << std::endl;
}

// void parseSnapshot(Snapshot &snapshot) {

//     std::cout << "Instrument : " << snapshot.instrumentId() << std::endl;
//     std::cout << "Side : " << (snapshot.levelsList().side() == BookSide::ask ? "Ask":"Bid") << std::endl;
//     std::cout << "Price : " << snapshot.levelsList().price() << std::endl;
//     std::cout << "Amount : " << snapshot.levelsList().amount() << std::endl;
// }

size_t decode(int msgType, char *buffer, size_t offset, size_t buffLen, size_t blockLen, int version) {
   size_t varLen = 0;
   // check template-id
   switch(msgType) {
   case 1000:
   {
       std::cout << "msg type: Instrument" << std::endl;
       deribit_multicast::Instrument instrument(buffer, offset, buffLen, blockLen, version);
       std::cout << instrument << std::endl;
       offset += instrument.sbeBlockLength();
       // sbe codec has issues to handle vars automatically
       auto instrNameLen = (int)buffer[offset++];
       std::string instrName(&buffer[offset], instrNameLen);
       std::cout << "instrumentNameLength: " << instrNameLen << std::endl;
       std::cout << "instrumentName: " << instrName << std::endl;
       offset += instrNameLen;
       FormBfcSym(instrument);
       break;
   }
   case 1001:
   {
       std::cout << "msg type: Book" << std::endl;
       deribit_multicast::Book book(buffer, offset, buffLen, blockLen, version);
       std::cout << book.header() << std::endl;
       std::cout << "instrumentId: " << book.instrumentId() << std::endl;
       std::cout << "timestampMs: " << book.timestampMs() << std::endl;
       std::cout << "changeId: " << book.changeId() << std::endl;
       std::cout << "numGroups: " << book.header().numGroups() << std::endl;
       std::cout << "numVars: " << book.header().numVarDataFields() << std::endl;

       deribit_multicast::Book::ChangesList changesList;
       offset += book.sbeBlockLength();
       changesList.wrapForDecode(buffer, (uint64_t*)&offset, version, buffLen);
       while (changesList.hasNext()) {
           std::cout << "changesList: " << changesList.next() << std::endl;
       }
       std::cout << std::endl;
       break;
   }
   case 1002:
   {
       std::cout << "msg type: Trades" << std::endl;
       deribit_multicast::Trades trades(buffer, offset, buffLen, blockLen, version);
       std::cout << trades.header() << std::endl;
       std::cout << "instrumentId: " << trades.instrumentId() << std::endl;
       std::cout << "numGroups: " << trades.header().numGroups() << std::endl;
       std::cout << "numVars: " << trades.header().numVarDataFields() << std::endl;

       deribit_multicast::Trades::TradesList tradesList;
       offset += trades.sbeBlockLength();
       tradesList.wrapForDecode(buffer, (uint64_t*)&offset, version, buffLen);
       while (tradesList.hasNext()) {
           std::cout << "tradesList: " << tradesList.next() << std::endl;
       }
       break;
   }
   case 1003:
   {
       std::cout << "msg type: Ticker " << std::endl;
       deribit_multicast::Ticker ticker(buffer, offset, buffLen, blockLen, version);
       std::cout << ticker << std::endl << std::endl;
       offset += ticker.sbeBlockLength();
       break;
   }
   case 1004:
   {
       std::cout << "msg type: Snapshot" << std::endl;
       deribit_multicast::Snapshot snapshot(buffer, offset, buffLen, blockLen, version);
       std::cout << snapshot.header() << std::endl;
       std::cout << "instrumentId: " << snapshot.instrumentId() << std::endl;
       std::cout << "timestampMs: " << snapshot.timestampMs() << std::endl;
       std::cout << "changeId: " << snapshot.changeId() << std::endl;
       std::cout << "numGroups: " << snapshot.header().numGroups() << std::endl;
       std::cout << "numVars: " << snapshot.header().numVarDataFields() << std::endl;

       deribit_multicast::Snapshot::LevelsList levelsList;
       offset += snapshot.sbeBlockLength();
       levelsList.wrapForDecode(buffer, (uint64_t*)&offset, version, buffLen);
       while (levelsList.hasNext()) {
           std::cout << "levelsList: " << levelsList.next() << std::endl;
       }
       std::cout << std::endl;
       /*parseSnapshot(snapshot);  When This Function Is Not Called It Does Not Prints The LevelsList But When 
                                   It Is Called It Prints The LevelsList But Gives An Error Unknow Value For Book Side */
       break;
   }
   default:
       std::cout << "Unknown message type: " << msgType << std::endl;
       offset = buffLen;
       break;
   }
   return offset;
}

int main () {
    std::string explMsg = "hQDrAwEAAQAAAAAALOgBAAE34DTMgAEAAAAAAGV7ReZBSOF6FO5s3EAAAAAAkErdQAAAAADA3dxAuB6F6zHb3ECF61G47tvcQAAAAABg29xAAAAAAAAANEAAAAAAgNzcQAAAAAAALKpAAAAAAAAAAADTBSdotNHgvrgehesx29xA//////////8K16Nw7d/cQB0A6QMBAAEAAQAAACzoAQA34DTMgAEAAMW85rMCAAAA0LzmswIAAAABEgABAAAAAAAAAQAAAACA3NxAAAAAAAAsqkA=";

    //std::string snapMsg = "FgDsAwEAAQABAAAASDcDAPOgqi2CAQAAhwkZPQYAAAABAREAFwAAAAAAAQAAAAAA4H9AAAAAAODIIEEBMzMzMzOzf0AAAAAAAIjDQAGamZmZmUV/QAAAAAAAAE5AAQAAAAAAAH5AAAAAAAAAWEABAAAAAACQekAAAAAAAGigQAEAAAAAAPB1QAAAAAAATO1AAQAAAAAA4HVAAAAAAABAj0ABAAAAAABQdEAAAAAAAECPQAFmZmZmZkp0QAAAAAAAiMNAAQAAAAAAEHNAAAAAAABooEABAAAAAADAckAAAAAAAECPQAEAAAAAADBxQAAAAAAAQI9AAQAAAAAAQHBAAAAAAABuoEABAAAAAAAAcEAAAAAAAAAkQAEAAAAAAEBvQAAAAAAAQI9AAc3MzMzMZG1AAAAAAACIw0ABAAAAAACAZkAAAAAAAADwPwEAAAAAAKBkQAAAAAAA4IVAAZqZmZmZaWNAAAAAAAAAAEABZmZmZmZ+YEAAAAAAAIjDQAEAAAAAAABZQAAAAAAAAFlAAc3MzMzMPFNAAAAAAACwckABAAAAAAAAPkAAAAAAAIjDQA==";
    std::string snapMsg = "FgDsAwEAAQABAAAASDcDAKJpyJGBAQAAORM19AUAAAABAREAAQAAAAAAAAAAAAAAIKxAAAAAAACIw0A=";

    std::string instMsg = "jADoAwEAAQAAAAEARTcDAAEBAAIABQMARVRIAAAAAABFVEgAAAAAAFVTRAAAAAAARVRIAAAAAABFVEgAAAAAAOha1It+AQAAAJi5lIEBAAAAAAAAAFirQAAAAAAAAPA/AAAAAAAA8D/8qfHSTWJAP2EyVTAqqTM/YTJVMCqpMz9hMlUwKqkzPwAAAAAAAAAAAAAAAAAAAAASRVRILTI0SlVOMjItMzUwMC1QhQDrAwEAAQAAAAAARTcDAAFlrJWIgQEAAAAAAAAAoH5AkxgEVg4tAEB56SYxCKwBQJqZmZmZmfE/9ihcj8KUkUBmZmZmZuYAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//////////////////////YoXI/ClJFA//////////9mZmZmZmYAQBYA7AMBAAEAAQAAAEU3AwBlrJWIgQEAAI5Tq98FAAAAAQERAAAAAAAAAA==";

    std::string trdMsg = "BADqAwEAAQABAAAA0gQAAFMAAQAAAAAAAQAAAAAINuNAAAAAAAAAJEAzspqMgQEAAIXrUbj+8+JArkfhenTZ4kDn+wUAAAAAANLLEwAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

   char buffer[10 * 1024];


   // decode msg
   size_t buffLen = sizeof(buffer);
   size_t blockLen = sizeof(buffer);
   size_t offset = 0;
   int version = 1;
   int msgType = 0;
   deribit_multicast::MessageHeader header;

   // decoding ticker message
   auto res = boost::beast::detail::base64::decode(buffer, explMsg.c_str(), explMsg.size());
   //printBuffer(buffer, res.first);
   buffLen = res.first;
   while(offset < buffLen) {
       // decode msg header.
       header.wrap(buffer, offset, version, buffLen);
       // decode full msg.
       msgType = header.templateId();
       blockLen = header.blockLength();
       offset = decode(msgType, buffer, offset, buffLen, blockLen, version);
   }
   offset = 0;

   // decoding snapshot message
   res = boost::beast::detail::base64::decode(buffer, snapMsg.c_str(), snapMsg.size());
   buffLen = res.first;
   while(offset < buffLen) {
       // decode msg header.
       header.wrap(buffer, offset, version, buffLen);
       // decode full msg.
       msgType = header.templateId();
       blockLen = header.blockLength();
       offset = decode(msgType, buffer, offset, buffLen, blockLen, version);
   }
   offset = 0;

   // decoding instrument message
   res = boost::beast::detail::base64::decode(buffer, instMsg.c_str(), instMsg.size());
   buffLen = res.first;
   while(offset < buffLen) {
       // decode msg header.
       header.wrap(buffer, offset, version, buffLen);
       // decode full msg.
       msgType = header.templateId();
       blockLen = header.blockLength();
       offset = decode(msgType, buffer, offset, buffLen, blockLen, version);
   }
   offset = 0;

   // decoding trades message
   res = boost::beast::detail::base64::decode(buffer, trdMsg.c_str(), trdMsg.size());
   buffLen = res.first;
   while(offset < buffLen) {
       // decode msg header.
       header.wrap(buffer, offset, version, buffLen);
       // decode full msg.
       msgType = header.templateId();
       blockLen = header.blockLength();
       offset = decode(msgType, buffer, offset, buffLen, blockLen, version);
   }
   offset = 0;

}


