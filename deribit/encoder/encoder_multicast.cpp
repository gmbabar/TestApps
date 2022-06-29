#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include "deribit_multicast/Trades.h"
#include "deribit_multicast/Book.h"

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

void publishData(int &sockFd,sockaddr_in &addr, socklen_t &size) {  
   char buffer[1024];
   memset(buffer, 0, sizeof(buffer));

   while (true) {
      encodeTrades(buffer, sizeof(buffer));
      sleep(1);
      std::cout << "[Sent Data] " << buffer << std::endl;
      sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
   }   
}


int main () {
   sockaddr_in srvAddr;
   in_addr lAddr;
   srvAddr.sin_family = AF_INET;
   srvAddr.sin_port = htons(1234);
   srvAddr.sin_addr.s_addr = inet_addr("226.0.0.1");
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
