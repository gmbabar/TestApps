#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include "deribit_multicast/Trades.h"
#include "deribit_multicast/Book.h"
#include "deribit_multicast/Instrument.h"
#include "deribit_multicast/Snapshot.h"

using namespace deribit_multicast;
void encodeTrades(char *baseBuffer, size_t buffSize) {
   char buffer[1024];
   memset(buffer, 0, sizeof(buffer));

   Trades trades;
   trades.wrapForEncode(buffer, 0, sizeof(buffer)) 
         .instrumentId(1234);
   // Deribit expects header size excluded
   trades.header().blockLength(Trades::sbeBlockLength() - MessageHeader::encodedLength())
      .templateId(Trades::sbeTemplateId())
      .schemaId(Trades::sbeSchemaId())
      .version(Trades::sbeSchemaVersion())
      .numGroups(1)
      .numVarDataFields(0);
   auto& tradesList = trades.tradesListCount(1);
   tradesList.next()
             .direction(Direction::Value::sell)
             .price(39344.25)
             .amount(10)
             .timestampMs(1655921357363 )
             .markPrice(38815.96)
             .indexPrice(38603.64)
             .tradeSeq(392167)
             .tradeId(1297362)
             .tickDirection(TickDirection::Value::zerominus)
             .liquidation(Liquidation::Value::none)
             .iv(0)
             .blockTradeId(0)
             .comboTradeId(0);

   memset(baseBuffer, 0, buffSize);
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, trades.sbePosition());
}

void encodeSnapshot(char *baseBuffer, size_t buffSize) {
  char buffer[1024];
  size_t version = 1;
  size_t offset = 0;
  memset(buffer, 0, sizeof(buffer));

  Snapshot snapshot;
  snapshot.wrapForEncode(buffer, offset, sizeof(buffer))
          .instrumentId(210760)
          .timestampMs(1656008239522)
          .changeId(25571955513)
          .isBookComplete(YesNo::yes)
          .isLastInBook(YesNo::yes);

  snapshot.header().blockLength(Snapshot::sbeBlockLength() - MessageHeader::encodedLength())
          .templateId(Snapshot::sbeTemplateId())
          .schemaId(Snapshot::sbeSchemaId())
          .version(Snapshot::sbeSchemaVersion())
          .numGroups(1)
          .numVarDataFields(0);

  auto &levelsList = snapshot.levelsListCount(1);
  levelsList.next()
            .side(BookSide::ask)
            .price(3600)
            .amount(10000);

  memset(baseBuffer, 0, sizeof(buffer));
  auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, snapshot.sbePosition());
}

void encodeInstrument(char *baseBuffer, size_t buffSize) {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

  Instrument instrument;
  instrument.wrapForEncode(buffer, 0, sizeof(buffer))
            .instrumentId(618)
            .instrumentState(InstrumentState::created)
            .kind(InstrumentKind::option)
            .futureType(FutureType::not_applicable)
            .optionType(OptionType::put)
            .rfq(YesNo::no)
            .settlementPeriod(Period::minute)
            .settlementPeriodCount(15)
            // .baseCurrency(1, "BTC")
            // .quoteCurrency(1, "BTC")
            // .counterCurrency(1, "USD")
            // .settlementCurrency(1, "BTC")
            // .sizeCurrency(1, "BTC")
            .creationTimestampMs(1655921357363)
            .expirationTimestampMs(1651021357363)
            .strikePrice(29200)
            .contractSize(1)
            .minTradeAmount(0.01)
            .tickSize(0.0001)
            .makerCommission(0.0001)
            .takerCommission(0.0005)
            .blockTradeCommission(0.00015)
            .maxLiquidationCommission(0)
            .maxLeverage(0)
            .instrumentName();
   strcpy(instrument.baseCurrency(), "BTC");
   strcpy(instrument.quoteCurrency(), "BTC");
   strcpy(instrument.counterCurrency(), "USD");
   strcpy(instrument.settlementCurrency(), "BTC");
   strcpy(instrument.sizeCurrency(), "BTC");


  instrument.header().blockLength(Instrument::sbeBlockLength() - MessageHeader::encodedLength())
         .templateId(Instrument::sbeTemplateId())
         .schemaId(Instrument::sbeSchemaId())
         .version(Instrument::sbeSchemaVersion())
         .numGroups(0)
         .numVarDataFields(1);

  memset(baseBuffer, 0, sizeof(buffer));
  auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, instrument.sbePosition());
}

void encodeBook(char *baseBuffer, size_t buffSize) {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   Book book;
   book.wrapForEncode(buffer, 0, sizeof(buffer))
       .instrumentId(212871)
       .timestampMs(1656008799550)
       .prevChangeId(25572133998)
       .changeId(25572134006)
       .isLast(YesNo::yes);

   book.header().blockLength(Book::sbeBlockLength() - MessageHeader::encodedLength())
       .templateId(Book::sbeTemplateId())
       .schemaId(Book::sbeSchemaId())
       .version(Book::sbeSchemaVersion())
       .numGroups(1)
       .numVarDataFields(0);

   auto& changeList = book.changesListCount(1);
   changeList.next()
             .side(BookSide::bid)
             .change(BookChange::created)
             .price(1091.9)
             .amount(2970);

   memset(baseBuffer, 0, sizeof(buffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, book.sbePosition());
}

void publishData(int &sockFd,sockaddr_in &addr, socklen_t &size) {  
   char buffer[1024];
   memset(buffer, 0, sizeof(buffer));
   const int delay = 5;

   while (true) {
      encodeTrades(buffer, sizeof(buffer));
      sleep(delay);
      std::cout << "[Sent Trade Data] " << buffer << std::endl;
      sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
      memset(buffer, 0, sizeof(buffer));

      encodeSnapshot(buffer, sizeof(buffer));
      sleep(delay);
      std::cout << "[Sent Snapshot Data] " << buffer << std::endl;
      sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
      memset(buffer, 0, sizeof(buffer));

      encodeInstrument(buffer, sizeof(buffer));
      sleep(delay);
      std::cout << "[Sent Instrument Data] " << buffer << std::endl;
      sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
      memset(buffer, 0, sizeof(buffer));

      encodeBook(buffer, sizeof(buffer));
      sleep(delay);
      std::cout << "[Sent Book Data] " << buffer << std::endl;
      sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
      memset(buffer, 0, sizeof(buffer));
   }   
}


int main (int argc, char **argv) {
   if(argc < 3)
   {
      std::cout << "Too Few Arguements:\n\t";
      std::cout << "Example:\n\t" << argv[0] << "<host> " << "<Port>" << "\n\t";
      std::cout << argv[0] << " 0.0.0.0 " << "1234" << std::endl;
      return -1;
   }

   sockaddr_in srvAddr;
   in_addr lAddr;
   srvAddr.sin_family = AF_INET;
   srvAddr.sin_port = htons(atoi(argv[2]));
   srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
   lAddr.s_addr = inet_addr("127.0.0.1");
   socklen_t size = sizeof(srvAddr);
   
   int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(sockfd < 0) {
      perror("Socket");
      exit(EXIT_FAILURE);
   }
   std::cout << "[Info] Socket Created" << std::endl;

   int ec = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&lAddr, sizeof(lAddr));
   if(ec < 0) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
   }

   publishData(sockfd, srvAddr, size);
   return 0;
}
