#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/Trades.h"
#include "deribit_multicast/Instrument.h"
#include "deribit_multicast/Book.h"
#include "deribit_multicast/Snapshot.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <ctime>

using namespace deribit_multicast;

// TODO:
// 1. Make formatting as Nebula standard [Done]
// 2. No need for FormatDate function, construct 'struct tm' and use ostringstream to build symbol. [Done]
// 3. ParseSnapshot [Not Done] -> Stuck At Problem "Decoder Is Not Decoding The Snapshot Correctly";

void parseSnapshot(Snapshot &snapshot) {

   auto levelList = snapshot.levelsList();
   std::cout << levelList.amount() << std::endl;
   std::cout << levelList.price() << std::endl;
   std::cout << levelList.side() << std::endl;
}

void encodeSnapshot() {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   /*
   <message name="snapshot" id="1004">
		<field name="header" id="1" type="messageHeader" />
		<field name="instrumentId" id="2" type="uint32" />
		<field name="timestampMs" id="3" type="uint64" />
		<field name="changeId" id="4" type="uint64" />
		<field name="isBookComplete" id="5" type="yesNo" />
		<field name="isLastInBook" id="6" type="yesNo" />
		<group name="levelsList" id="8" dimensionType="groupSizeEncoding">
			<field name="side" id="1" type="bookSide" />
			<field name="price" id="2" type="double" />
			<field name="amount" id="3" type="double" />
		</group>
	</message>
   */
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

  std::cout << "Header : " << snapshot.header() << std::endl;
   std::cout << "Snapshot : " << snapshot << std::endl;
   std::cout << "Position : " << snapshot.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(buffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, snapshot.sbePosition());
   std::cout << "Encoded Size : " << len << std::endl;
   std::cout << "Base64 Buffer : " << baseBuffer << std::endl;

   parseSnapshot(snapshot);

}

void encodeInstrument() {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   /*
   <message name="instrument" id="1000">
		<field name="header" id="1" type="messageHeader" />
		<field name="instrumentId" id ="2" type="uint32" />
		<field name="instrumentState" id="3" type="instrumentState" />
		<field name="kind" id="4" type="instrumentKind" />
		<field name="futureType" id="5" type="futureType" />
		<field name="optionType" id="6" type="optionType" />
		<field name="rfq" id="7" type="yesNo" />
		<field name="settlementPeriod" id="8" type="period" />
		<field name="settlementPeriodCount" id="9" type="uint16" />
		<field name="baseCurrency" id="10" type="string8" />
		<field name="quoteCurrency" id="11" type="string8" />
		<field name="counterCurrency" id="12" type="string8" />
		<field name="settlementCurrency" id="13" type="string8" />
		<field name="sizeCurrency" id="14" type="string8" />
		<field name="creationTimestampMs" id="15" type="uint64" />
		<field name="expirationTimestampMs" id="16" type="uint64" />
		<field name="strikePrice" id="17" type="double" />
		<field name="contractSize" id="18" type="double" />
		<field name="minTradeAmount" id="19" type="double" />
		<field name="tickSize" id="20" type="double" />
		<field name="makerCommission" id="21" type="double" />
		<field name="takerCommission" id="22" type="double" />
		<field name="blockTradeCommission" id="23" type="double" />
		<field name="maxLiquidationCommission" id="24" type="double" />
		<field name="maxLeverage" id="25" type="double" />
		<data name="instrumentName" id="26" type="varString" />
	</message>
   */
  Instrument instrument;
  instrument.wrapForEncode(buffer, 0, sizeof(buffer))
            .instrumentId(618)
            .instrumentState(InstrumentState::created)
            .kind(InstrumentKind::future)
            .futureType(FutureType::reversed)
            .optionType(OptionType::not_applicable)
            .rfq(YesNo::no)
            .settlementPeriod(Period::minute)
            .settlementPeriodCount(15)
            .settlementPeriod(Period::perpetual)
            .settlementPeriodCount(15)
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

   std::cout << "Header : " << instrument.header() << std::endl;
   std::cout << "Instrument : " << instrument << std::endl;
   std::cout << "Position : " << instrument.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(buffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, instrument.sbePosition());
   std::cout << "Encoded Size : " << len << std::endl;
   std::cout << "Base64 Buffer : " << baseBuffer << std::endl;


}

void encodeBook() {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

   /*   
   <message name="book" id="1001">
		<field name="header" id="1" type="messageHeader" />
		<field name="instrumentId" id="2" type="uint32" />
		<field name="timestampMs" id="3" type="uint64" />
		<field name="prevChangeId" id="4" type="uint64" />
		<field name="changeId" id="5" type="uint64" />
		<field name="isLast" id="6" type="yesNo" />
		<group name="changesList" id="7" dimensionType="groupSizeEncoding">
			<field name="side" id="1" type="bookSide" />
			<field name="change" id="2" type="bookChange" />
			<!-- Use double (64 bit float) encoding, SBE FIX price/amount decimal encoding makes sense when the 
			decimal point is on a somewhat fixed position, for crypto, it can vary more by instrument -->
			<field name="price" id="3" type="double" />
			<field name="amount" id="4" type="double" />
		</group>
      </message>
   */
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

   auto& changeList = book.changesListCount(2);
   changeList.next()
             .side(BookSide::bid)
             .change(BookChange::created)
             .price(1091.9)
             .amount(2970);
   changeList.next()
             .side(BookSide::ask)
             .change(BookChange::created)
             .price(1092.9)
             .amount(100);

   std::cout << "Header : " << book.header() << std::endl;
   std::cout << "Book : " << book << std::endl;
   std::cout << "Position : " << book.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(buffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, book.sbePosition());
   std::cout << "Encoded Size : " << len << std::endl;
   std::cout << "Base64 Buffer : " << baseBuffer << std::endl;
}

// Trades encoding.
void encodeTrades() {
   char buffer[1024];
   size_t version = 1;
   size_t offset = 0;
   memset(buffer, 0, sizeof(buffer));

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
             .direction(Direction::Value::sell)
             .price(39344.2401)
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

   std::cout << "Header: " << trades.header() << std::endl;
   std::cout << "Trades: \n" << trades << std::endl;
   {
      std::cout << std::setprecision(10);
      std::cout << "tradesList: " << tradesList << std::endl;
      std::cout << "trades price: " << tradesList.price() << std::endl;
   }
   std::cout << "position: " << trades.sbePosition() << std::endl;

   char baseBuffer[1024];
   memset(baseBuffer, 0, sizeof(baseBuffer));
   auto len = boost::beast::detail::base64::encode(baseBuffer, buffer, trades.sbePosition());
   std::cout << "encoded size: " << len << std::endl;
   std::cout << "base64 buffer: " << baseBuffer << std::endl;
}


int main () {

   // encodeTrades();
   encodeBook();
   // encodeInstrument();
   // encodeSnapshot();
   return 0;
}
