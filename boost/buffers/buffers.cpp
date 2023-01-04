// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/flat_buffer.hpp> // flat_buffer

/** Summary:
 * prepare() takes data from buffer & make it mutable buffer
 * commit() makes data ready for read
 * size() tells read/write data available
 * clear() kind of reset buffer
 **/

int main () {

	// construct
	boost::beast::flat_buffer buffer(1024);
	buffer.max_size(1024);	// limit max_size

	// consume 7 bytes.
	auto prepBuff = buffer.prepare(10);
	char *p = static_cast<char*>(prepBuff.data());
	memcpy(p, "123456\0", 7);
	buffer.commit(7);

	// read data.
	auto cbuf = buffer.cdata();
	for( int i=0; i<buffer.size(); ++i)
		printf("%c \n", static_cast<const char*>(cbuf.data())[i]);

	printf("Size: %lu\n", buffer.size());
	printf("STR: %s \n", static_cast<const char*>(cbuf.data()));

	// consume... data still there.
	buffer.consume(buffer.size());
	printf("Size: %lu\n", buffer.size());
	printf("STR: %s \n", static_cast<const char*>(buffer.cdata().data()));
	printf("STR: %s \n", static_cast<const char*>(buffer.data().data()));

	// clear removes data from buffer.
	buffer.clear();
	printf("Size: %lu\n", buffer.size());
	printf("STR: %s \n", static_cast<const char*>(buffer.cdata().data()));
	printf("STR: %s \n", static_cast<const char*>(buffer.data().data()));

}


