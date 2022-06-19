
#include <chrono>
#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/MessageHeader.h"
#include "deribit_multicast/Ticker.h"

int main () {
    std::string baseMsg = "hQDrAwEAAQAAAAAALOgBAAE34DTMgAEAAAAAAGV7ReZBSOF6FO5s3EAAAAAAkErdQAAAAADA3dxAuB6F6zHb3ECF61G47tvcQAAAAABg29xAAAAAAAAANEAAAAAAgNzcQAAAAAAALKpAAAAAAAAAAADTBSdotNHgvrgehesx29xA//////////8K16Nw7d/cQB0A6QMBAAEAAQAAACzoAQA34DTMgAEAAMW85rMCAAAA0LzmswIAAAABEgABAAAAAAAAAQAAAACA3NxAAAAAAAAsqkA=";

   char buffer[1024];
   auto res = boost::beast::detail::base64::decode(buffer, baseMsg.c_str(), baseMsg.size());
   std::cout << "Input size: " << baseMsg.size() << std::endl;
   std::cout << "Input read: " << res.second << std::endl;
   std::cout << "Output size: " << res.first << std::endl;

   std::cout << "Output data:" << std::endl;
   for (int idx=0; idx<res.first; ++idx) 
       printf("%.02x ", static_cast<unsigned char>(buffer[idx]));
   printf("\n");

   // decode msg header.
   deribit_multicast::MessageHeader header(buffer, 0, res.first, 1);
   std::cout << "Header: " << std::endl;
   std::cout << header << std::endl;

   // check template-id
   switch(header.templateId()) {
   case 1000:
       std::cout << "msg type: instrument" << std::endl;
       break;
   case 1001:
       std::cout << "msg type: book" << std::endl;
       break;
   case 1002:
       std::cout << "msg type: trades" << std::endl;
       break;
   case 1003:
       std::cout << "msg type: ticker, size: " << header.blockLength() << std::endl;
       std::cout << deribit_multicast::Ticker(buffer, 0, res.first, header.blockLength(), 1) << std::endl;
       break;
   case 1004:
       std::cout << "msg type: snapshot" << std::endl;
       break;
   default:
       std::cout << "Unknown message type." << std::endl;
       break;
   }
}


