#include <iostream>
#include <boost/beast/core/detail/base64.hpp>
#include "deribit_multicast/Book.h"

using namespace deribit_multicast;

int main()
{
    char buffer[256];
    char bufferHdr[256];
    char bufferEnc[512];
    MessageHeader hdr(bufferHdr, sizeof(bufferHdr));
    hdr.wrap(bufferHdr, 0, 1, sizeof(bufferHdr))
    .blockLength(Book::sbeBlockLength())
    .templateId(Book::sbeTemplateId())
    .schemaId(Book::sbeSchemaId())
    .version(Book::sbeSchemaVersion())
    .numGroups(1)
    .numVarDataFields(0);
    Book b1;
    b1.wrapForEncode(buffer, 0+hdr.encodedLength(), sizeof(buffer))
    .instrumentId(124972)
    .timestampMs(1652693458999)
    .prevChangeId(11608177861)
    .changeId(11608177872)
    .isLast(deribit_multicast::YesNo::yes);

    std::ostringstream oss;
    oss << b1;

    std::cout << oss.str() << std::endl;


}