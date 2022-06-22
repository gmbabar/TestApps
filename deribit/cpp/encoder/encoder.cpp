#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "./deribit_multicast/Trades.h"

using namespace deribit_multicast;

int main()
{
   char buffer[256];
   MessageHeader hdr(buffer, sizeof(buffer));
   hdr.wrap(buffer, 0, 1, sizeof(buffer))
      .blockLength(Trades::sbeBlockLength())
      .templateId(Trades::sbeTemplateId())
      .schemaId(Trades::sbeSchemaId())
      .version(Trades::sbeSchemaVersion())
      .numGroups(1)
      .numVarDataFields(0);
   
   Trades trade;
   //Everything Ok Just Need To Fix The Values Of Offset and Bufflength
   trade.wrapForEncode(buffer, 0, sizeof(buffer)) // <--- Here 
   .instrumentId(1)
   .tradesList() // <----- And Here 
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

   //Giving Error std::runtime_error buffer too short for next group index [E108]
   std::cout << trade << std::endl;
}
