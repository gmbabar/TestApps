// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/flat_buffer.hpp> // flat_buffer

int main () {

	// construct
	boost::beast::flat_buffer buffer(1024);
	buffer.max_size(1024);	// limit max_size

	// consume 6 bytes.
	auto prepBuff = buffer.prepare(10);
	char *p = static_cast<char*>(prepBuff.data());
	memcpy(p, "123456", 6);
	buffer.commit(6);

	// read data.
	auto cbuf = buffer.cdata();
	for( int i=0; i<buffer.size(); ++i)
		printf("%c \n", static_cast<const char*>(cbuf.data())[i]);
}


