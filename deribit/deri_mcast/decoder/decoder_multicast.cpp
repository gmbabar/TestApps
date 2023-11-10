
#include <chrono>
#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/MessageHeader.h"
#include "deribit_multicast/Book.h"
#include "deribit_multicast/Instrument.h"
#include "deribit_multicast/Snapshot.h"
#include "deribit_multicast/Ticker.h"
#include "deribit_multicast/Trades.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void printBuffer(const char *buffer, size_t size) {
   std::cout << "Output data:" << std::endl;
   for (int idx=0; idx<size; ++idx) 
       printf("%.02x ", static_cast<unsigned char>(buffer[idx]));
   printf("\n");
}

void FormBfcSym(deribit_multicast::Instrument &instrument) {

    std::ostringstream symbol;
    std::chrono::duration<uint64_t,std::milli> milliDtn(instrument.expirationTimestampMs());
    time_t seconds = std::chrono::duration_cast<std::chrono::seconds> (milliDtn).count();
    struct tm *tms = localtime(&seconds);

    if(instrument.kind() == deribit_multicast::InstrumentKind::option) {

        symbol << "R_T_" << instrument.baseCurrency() << "_" << instrument.counterCurrency() << "_" << std::setfill('0')
                << std::setw(2) << tms->tm_year % 100 << std::setw(2) << tms->tm_mon << std::setw(2) << tms->tm_mday << "_"
                << instrument.strikePrice()
                << (instrument.optionType() == deribit_multicast::OptionType::put ? "_P" : "_C");

    }
    else if(instrument.kind() == deribit_multicast::InstrumentKind::future && instrument.settlementPeriod() == deribit_multicast::Period::perpetual){
            symbol << "R_" << (instrument.futureType() == deribit_multicast::FutureType::reversed ? "I_" : "T_") << instrument.baseCurrency() << "Perpetual";
    }
    else if(instrument.kind() == deribit_multicast::InstrumentKind::future) {

        symbol << "R_" << (instrument.futureType() == deribit_multicast::FutureType::reversed ? "I_" : "T_") << instrument.baseCurrency() << "_" << instrument.counterCurrency() << "_" 
                << std::setfill('0') << std::setw(2) << tms->tm_year % 100 << std::setw(2) << tms->tm_mon << std::setw(2) << tms->tm_mday << "_"
                << instrument.strikePrice();
    } 
    else {
        std::cerr << "Invalid Instrument Kind" << std::endl;
        return;
    }

    std::cout << std::endl << "BFC-Sym : "<< symbol.str() << std::endl;
}

size_t decode(int msgType, char *buffer, size_t offset, size_t buffLen, size_t blockLen, int version) {
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
       // TODO:
       // - Construct symbol as advised in encoder.cpp
       // - Remove implementation from encoder.cpp
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
            // TODO: Parse snapshot to extract:
            // - side
            // - price
            // - size
            // - symbol
            std::cout << "Snapshot Parser : " << std::endl;
            std::cout << "\tSide : " << (levelsList.side() == deribit_multicast::BookSide::ask ? "Ask":"Bid") << std::endl;
            std::cout << "\tPrice : " << levelsList.price() << std::endl;
            std::cout << "\tAmount : " << levelsList.amount() << std::endl;
            std::cout << "\tInstrument : " << snapshot.instrumentId() << std::endl;
       }
       std::cout << std::endl;
       break;
   }
   default:
       std::cout << "Unknown message type: " << msgType << std::endl;
       offset = buffLen;
       break;
   }
   return offset;
}


void parseMsg(const char *buffer) {
    size_t buffLen = 0;
    size_t blockLen = 0;
    size_t offset = 0;
    int version = 1;
    int msgType = 0;
    deribit_multicast::MessageHeader header;
    char binBuff[10*1024];

    auto res = boost::beast::detail::base64::decode(binBuff, buffer, strlen(buffer));
    buffLen = res.first;
    while(offset < buffLen) {
        // decode msg header.
        header.wrap(binBuff, offset, version, buffLen);
        // decode full msg.
        msgType = header.templateId();
        blockLen = header.blockLength();
        offset = decode(msgType, binBuff, offset, buffLen, blockLen, version);
   }
}

void reciveAndDecode(int &sockFd, sockaddr_in &addr, socklen_t &size) {
    char buffer[1024];
    while (true) {
        recvfrom(sockFd, buffer, sizeof(buffer), 0, (sockaddr*)&addr, &size);
        std::cout << "[Data Recieved] " << buffer << std::endl;
        parseMsg(buffer);
    }   
}

int main (int argc, char *argv[]) {
    if (argc != 4) {
       std::cout << "Usage:\n";
       std::cout << "\t" << argv[0] << "<mcast_ip> <local_ip> <Port>" << "\n\t";
       std::cout << argv[0] << " 226.0.0.1  127.0.0.1  1234\n" << std::endl;
       return -1;
    }

    sockaddr_in srvAddr;
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(atoi(argv[3]));
    srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t size = sizeof(srvAddr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket Created" << std::endl;

    ip_mreq mrq;
    mrq.imr_interface.s_addr = inet_addr(argv[2]);
    mrq.imr_multiaddr.s_addr = inet_addr(argv[1]);
    
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int ec = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mrq, sizeof(mrq));
    if (ec < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    ec = bind(sockfd, (sockaddr*)&srvAddr, size);
    if (ec < 0) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }

    std::cout << "Starting To Recieve Data" << std::endl;
    reciveAndDecode(sockfd, srvAddr, size);
}


