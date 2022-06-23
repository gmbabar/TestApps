#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include "../../deribit_multicast/Trades.h"
#include "../../deribit_multicast/Book.h"

using namespace deribit_multicast;
const char* encodeTrades(Direction::Value dValue, float price,
                        float amount, long int timestampMs,
                        float markPrice, float indexPrice,
                        int tradeSeq, int tradeId,
                        TickDirection::Value tdValue,
                        Liquidation::Value lValue,
                        int iv=0, int blockTradeId=0,
                        int comboTradeId=0) {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   /*** 
   MessageHeader hdr(buffer, sizeof(buffer));
   hdr.blockLength(Trades::sbeBlockLength()) // Deribit expects header size to be excluded.
      .templateId(Trades::sbeTemplateId())
      .schemaId(Trades::sbeSchemaId())
      .version(Trades::sbeSchemaVersion())
      .numGroups(1)
      .numVarDataFields(0);
   ***/

   /***
   <message name="trades" id="1002">
     <field name="header" id="1" type="messageHeader"/>
     <field name="instrumentId" id="2" type="uint32"/>
     <group name="tradesList" id="3" dimensionType="groupSizeEncoding">
       <field name="direction" id="1" type="direction"/>
       <field name="price" id="2" type="double"/>
       <field name="amount" id="3" type="double"/>
       <field name="timestampMs" id="4" type="uint64"/>
       <field name="markPrice" id="5" type="double"/>
       <field name="indexPrice" id="6" type="double"/>
       <field name="tradeSeq" id="7" type="uint64"/>
       <field name="tradeId" id="8" type="uint64"/>
       <field name="tickDirection" id="9" type="tickDirection"/>
       <field name="liquidation" id="10" type="liquidation"/>
       <field name="iv" id="11" type="double" presence="optional" nullValue="0"/>
       <field name="blockTradeId" id="12" type="uint64" presence="optional" nullValue="0"/>
       <field name="comboTradeId" id="13" type="uint64" presence="optional" nullValue="0"/>
     </group>
   </message>
   ***/
   
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
             .direction(dValue)
             .price(price)
             .amount(amount)
             .timestampMs(timestampMs)
             .markPrice(markPrice)
             .indexPrice(indexPrice)
             .tradeSeq(tradeSeq)
             .tradeId(tradeId)
             .tickDirection(tdValue)
             .liquidation(lValue)
             .iv(iv)
             .blockTradeId(blockTradeId)
             .comboTradeId(comboTradeId);

   // std::cout << "Header: " << trades.header() << std::endl;
   // std::cout << "Trades: \n" << trades << std::endl;
   // std::cout << "position: " << trades.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(baseBuffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, trades.sbePosition());
   // std::cout << "encoded size: " << len << std::endl;
   // std::cout << "base64 buffer: " << baseBuffer << std::endl;
   const char *encBase = baseBuffer;
   return(encBase);
}

void PublishData(int &sockFd,sockaddr_in &addr, socklen_t &size)
{  
   int count = 0;
   long int tms = 1655921357363;
   float price = 29900.50;
   float amount = 1.5;
   float markPrice = 28750.50;
   float indexPrice = 28900.50;
   int tradeSeq = 273112;
   int tradeId = 77623;
   char buffer[1024];
   memset(buffer, 0, sizeof(buffer));
   const char *data;

   while (true)
   {
      if(count%2==0)
      {
         data = encodeTrades(Direction::Value::buy, price, amount,
                     tms, markPrice, indexPrice, tradeSeq,
                     tradeId, TickDirection::zerominus,
                     Liquidation::none);
         price -= 250;
         amount +=0.5;
         markPrice -=250;
         indexPrice -= 300;
         tradeSeq++;
         tradeId++;
      }
      else
      {
         data = encodeTrades(Direction::sell, price, amount,
                     tms, markPrice, indexPrice, tradeSeq,
                     tradeId, TickDirection::zeroplus,
                     Liquidation::none);
         price += 500;
         amount -=1;
         markPrice +=250;
         indexPrice += 300;
         tradeSeq++;
         tradeId++;
      }
      memcpy(buffer, data, strlen(data));
      sleep(1);
      if(count!=0)
      {
         std::cout << "[Sent Data] " << buffer << std::endl;
         sendto(sockFd, buffer, strlen(buffer), 0, (sockaddr*)&addr, size);
      }
      count++;
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
   if(sockfd < 0)
   {
      perror("Socket");
      exit(EXIT_FAILURE);
   }
   std::cout << "[Info] Socket Created" << std::endl;

   int ec = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&lAddr, sizeof(lAddr));
   if(ec < 0)
   {
      perror("setsockopt");
      exit(EXIT_FAILURE);
   }

   PublishData(sockfd, srvAddr, size);
   return 0;
}