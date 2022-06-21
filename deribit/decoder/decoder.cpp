
#include <chrono>
#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/MessageHeader.h"
#include "deribit_multicast/Book.h"
#include "deribit_multicast/Instrument.h"
#include "deribit_multicast/Snapshot.h"
#include "deribit_multicast/Ticker.h"
#include "deribit_multicast/Trades.h"

void decode(int msgType, char *buffer, size_t offset, size_t buffLen, size_t blockLen, int version) {
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
       for (int idx=0; idx<book.header().numGroups(); ++idx)
           std::cout << "changeList: " << book.changesList().next() << std::endl;
       std::cout << std::endl;
       break;
   }
   case 1002:
       std::cout << "msg type: Trades" << std::endl;
       std::cout << deribit_multicast::Trades(buffer, offset, buffLen, blockLen, version) << std::endl;
       std::cout << std::endl;
       break;
   case 1003:
       std::cout << "msg type: Ticker " << std::endl;
       std::cout << deribit_multicast::Ticker(buffer, offset, buffLen, blockLen, version) << std::endl;
       std::cout << std::endl;
       break;
   case 1004:
       std::cout << "msg type: Snapshot" << std::endl;
       break;
   default:
       std::cout << "Unknown message type." << std::endl;
       break;
   }
}

int main () {
    std::string explMsg = "hQDrAwEAAQAAAAAALOgBAAE34DTMgAEAAAAAAGV7ReZBSOF6FO5s3EAAAAAAkErdQAAAAADA3dxAuB6F6zHb3ECF61G47tvcQAAAAABg29xAAAAAAAAANEAAAAAAgNzcQAAAAAAALKpAAAAAAAAAAADTBSdotNHgvrgehesx29xA//////////8K16Nw7d/cQB0A6QMBAAEAAQAAACzoAQA34DTMgAEAAMW85rMCAAAA0LzmswIAAAABEgABAAAAAAAAAQAAAACA3NxAAAAAAAAsqkA=";

    std::string snapMsg = "FgDsAwEAAQABAAAASDcDAKIDzXyBAQAANrzM5wUAAAABAREAEAAAAAAAAAAAAAAAPK5AAAAAAACIw0AAAAAAAABQrkAAAAAAAIjDQAAAAAAAANauQAAAAAAAAPA/AAAAAAAApK9AAAAAAICEDkEAMzMzM7PdsEAAAAAAAIB8QAAAAAAAACOxQAAAAAAAJ7BAAAAAAAAAxrFAAAAAAIBPEkEAAAAAAADrsUAAAAAAACewQAAAAAAAAFSzQAAAAAAAVLNAAAAAAAAAiLNAAAAAAABQlEAAAAAAAABQtEAAAAAAAGroQAAAAAAAAIK0QAAAAAAAADlAAAAAAAAAGLVAAAAAAABq+EAAAAAAAACttUAAAAAAAACKQAAAAAAAAKi2QAAAAAAAF/FAAAAAAAAAXMFAAAAAAADgdUA=";

   char buffer[10 * 1024];
   auto res = boost::beast::detail::base64::decode(buffer, explMsg.c_str(), explMsg.size());
   std::cout << "Input size: " << explMsg.size() << std::endl;
   std::cout << "Input read: " << res.second << std::endl;
   std::cout << "Output size: " << res.first << std::endl;

   std::cout << "Output data:" << std::endl;
   for (int idx=0; idx<res.first; ++idx) 
       printf("%.02x ", static_cast<unsigned char>(buffer[idx]));
   printf("\n");

   // decode msg header.
   deribit_multicast::MessageHeader header(buffer, 0, res.first, 1);
   std::cout << "Header: " << std::endl << header << std::endl << std::endl;

   // decode msg
   size_t buffLen = res.first;
   size_t blockLen = header.blockLength();
   size_t offset = 0;
   int msgType = header.templateId();
   int version = 1;
   decode(msgType, buffer, offset, buffLen, blockLen, version);

   // decode second msg.
   offset = blockLen + header.encodedLength();
   //buffLen -= offset;
   header.wrap(buffer, offset, version, buffLen);
   std::cout << "Header: " << std::endl << header << std::endl << std::endl;
   blockLen = header.blockLength();
   msgType = header.templateId();
   decode(msgType, buffer, offset, buffLen, blockLen, version);
}


