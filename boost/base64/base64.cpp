
#include <chrono>
#include <iostream>
#include <boost/beast/core/detail/base64.hpp>


int main () {
    std::string baseMsg = "hQDrAwEAAQAAAAAALOgBAAE34DTMgAEAAAAAAGV7ReZBSOF6FO5s3EAAAAAAkErdQAAAAADA3dxAuB6F6zHb3ECF61G47tvcQAAAAABg29xAAAAAAAAANEAAAAAAgNzcQAAAAAAALKpAAAAAAAAAAADTBSdotNHgvrgehesx29xA//////////8K16Nw7d/cQB0A6QMBAAEAAQAAACzoAQA34DTMgAEAAMW85rMCAAAA0LzmswIAAAABEgABAAAAAAAAAQAAAACA3NxAAAAAAAAsqkA=";

   /** only available in 1.6x 
   auto binMsg = boost::beast::detail::base64_decode(baseMsg);
   printf("Input: %s\n", baseMsg.c_str());
   printf("Output:\n");
   for (auto ch : binMsg) {
       printf("%x", ch);
   }
   printf("\n");
   **/

   unsigned char buffer[1024];
   auto res = boost::beast::detail::base64::decode(buffer, baseMsg.c_str(), baseMsg.size());
   std::cout << "Input size: " << baseMsg.size() << std::endl;
   std::cout << "Input read: " << res.second << std::endl;
   std::cout << "Output size: " << res.first << std::endl;

   std::cout << "Output data:" << std::endl;
   for (int idx=0; idx<res.first; ++idx) 
       printf("%.02x ", buffer[idx]);
   printf("\n");
}


