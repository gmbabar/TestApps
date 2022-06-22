#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/Trades.h"

using namespace deribit_multicast;

void encodeTrades() {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   MessageHeader hdr(buffer, sizeof(buffer));
   hdr.blockLength(Trades::sbeBlockLength())
      .templateId(Trades::sbeTemplateId())
      .schemaId(Trades::sbeSchemaId())
      .version(Trades::sbeSchemaVersion())
      .numGroups(1)
      .numVarDataFields(0);

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
   trades.wrapForEncode(buffer, hdr.encodedLength(), sizeof(buffer)) 
         .instrumentId(1234);
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

   std::cout << "Header: " << hdr << std::endl;
   std::cout << "Trades: \n" << trades << std::endl;
   std::cout << "position: " << trades.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(baseBuffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, trades.sbePosition());
   std::cout << "encoded size: " << len << std::endl;
   std::cout << "base64 buffer: " << baseBuffer << std::endl;
}


int main () {

   encodeTrades();

   return 0;
}
