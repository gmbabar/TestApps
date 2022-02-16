// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/flat_buffer.hpp> // flat_buffer

int main () {

	// construct
	boost::beast::flat_buffer flatbuff(1024);
	printf("created static flat buffer. flatbuff size: %ld\n", flatbuff.size());

	// consume 6 bytes.
	auto fbuf = flatbuff.prepare(10);
	printf("prepared 10 bytes. fbuf size: %ld, flatbuff size: %ld\n", fbuf.size(), flatbuff.size());
	char *p = static_cast<char*>(fbuf.data());
	memcpy(p, "123456", 6);
	flatbuff.commit(6);

	// read data.
	auto cbuf = flatbuff.cdata();
	for( int i=0; i<flatbuff.size(); ++i)
		printf("%c \n", static_cast<const char*>(cbuf.data())[i]);
}


