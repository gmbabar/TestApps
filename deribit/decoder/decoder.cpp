
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

size_t decode(int msgType, char *buffer, size_t offset, size_t buffLen, size_t blockLen, int version) {
   size_t varLen = 0;
   // check template-id
   switch(msgType) {
   case 1000:
       std::cout << "msg type: Instrument" << std::endl;
       break;
   case 1001:
   {
       std::cout << "msg type: Book" << std::endl;
       deribit_multicast::Book book(buffer, offset, buffLen, blockLen, version);
       std::cout << "instrumentId: " << book.instrumentId() << std::endl;
       std::cout << "timestampMs: " << book.timestampMs() << std::endl;
       std::cout << "changeId: " << book.changeId() << std::endl;
       std::cout << "groups: " << book.header().numGroups() << std::endl;

       deribit_multicast::Book::ChangesList changesList;
       offset += book.sbeBlockLength();
       changesList.wrapForDecode(buffer, &offset, version, buffLen);
       for (int idx=0; idx<book.header().numGroups(); ++idx) {
           std::cout << "changeList: " << changesList.next() << std::endl;
       }
       std::cout << std::endl;
       break;
   }
   case 1002:
   {
       std::cout << "msg type: Trades" << std::endl;
       deribit_multicast::Trades trades(buffer, offset, buffLen, blockLen, version);
       std::cout << trades << std::endl;
       offset += trades.sbeBlockLength();
       std::cout << std::endl;
       break;
   }
   case 1003:
   {
       std::cout << "msg type: Ticker " << std::endl;
       deribit_multicast::Ticker ticker(buffer, offset, buffLen, blockLen, version);
       std::cout << ticker << std::endl;
       offset += ticker.sbeBlockLength();
       std::cout << std::endl;
       break;
   }
   case 1004:
   {
       std::cout << "msg type: Snapshot" << std::endl;
       break;
   }
   default:
       std::cout << "Unknown message type." << std::endl;
       break;
   }
   return offset;
}

int main () {
    std::string explMsg = "hQDrAwEAAQAAAAAALOgBAAE34DTMgAEAAAAAAGV7ReZBSOF6FO5s3EAAAAAAkErdQAAAAADA3dxAuB6F6zHb3ECF61G47tvcQAAAAABg29xAAAAAAAAANEAAAAAAgNzcQAAAAAAALKpAAAAAAAAAAADTBSdotNHgvrgehesx29xA//////////8K16Nw7d/cQB0A6QMBAAEAAQAAACzoAQA34DTMgAEAAMW85rMCAAAA0LzmswIAAAABEgABAAAAAAAAAQAAAACA3NxAAAAAAAAsqkA=";

    std::string snapMsg = "FgDsAwEAAQABAAAASDcDAKIDzXyBAQAANrzM5wUAAAABAREAEAAAAAAAAAAAAAAAPK5AAAAAAACIw0AAAAAAAABQrkAAAAAAAIjDQAAAAAAAANauQAAAAAAAAPA/AAAAAAAApK9AAAAAAICEDkEAMzMzM7PdsEAAAAAAAIB8QAAAAAAAACOxQAAAAAAAJ7BAAAAAAAAAxrFAAAAAAIBPEkEAAAAAAADrsUAAAAAAACewQAAAAAAAAFSzQAAAAAAAVLNAAAAAAAAAiLNAAAAAAABQlEAAAAAAAABQtEAAAAAAAGroQAAAAAAAAIK0QAAAAAAAADlAAAAAAAAAGLVAAAAAAABq+EAAAAAAAACttUAAAAAAAACKQAAAAAAAAKi2QAAAAAAAF/FAAAAAAAAAXMFAAAAAAADgdUA=";

   char buffer[10 * 1024];
   auto res = boost::beast::detail::base64::decode(buffer, explMsg.c_str(), explMsg.size());
   //printBuffer(buffer, res.first);


   // decode msg
   size_t buffLen = res.first;
   size_t blockLen = sizeof(buffer);
   size_t offset = 0;
   int version = 1;
   int msgType = 0;
   deribit_multicast::MessageHeader header;


   while(offset < buffLen) {
       // decode msg header.
       header.wrap(buffer, offset, version, buffLen);
       blockLen = header.blockLength();
       msgType = header.templateId();
       // decode full msg.
       offset = decode(msgType, buffer, offset, buffLen, blockLen, version);
   }
}


