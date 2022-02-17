// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/flat_static_buffer.hpp> // flat_static_buffer
#include <chrono>
#include <iostream>

void TestFlatBuffer(boost::beast::flat_static_buffer<1024> &flatbuff, const std::string &msg, const bool debug = true) {
	// construct
	//boost::beast::flat_static_buffer<1024> flatbuff;
	if (debug) printf("created flat buffer. flatbuff size: %ld\n", flatbuff.size());

	// write/commit 39 bytes.
	auto fbuf = flatbuff.prepare(msg.size());
	if (debug) printf("prepared %ld bytes. fbuf size: %ld, flatbuff size: %ld\n", msg.size(), fbuf.size(), flatbuff.size());
	char *p = static_cast<char*>(fbuf.data());
	memcpy(p, msg.c_str(), msg.size());
	flatbuff.commit(msg.size());
	if (debug) printf("commit %ld bytes. fbuf size: %ld, flatbuff size: %ld\n", msg.size(), fbuf.size(), flatbuff.size());

	// read data.
	auto cbuf = flatbuff.cdata();
	if (debug) {
	    printf("Data: ");
	    for( int i=0; i<flatbuff.size(); ++i)
		printf("%c", static_cast<const char*>(cbuf.data())[i]);
	    printf("\n");
	}
	flatbuff.clear();
	if (debug) printf("read %ld bytes. cbuf size: %ld, flatbuff size: %ld\n", msg.size(), cbuf.size(), flatbuff.size());

}

int main () {
	boost::beast::flat_static_buffer<1024> flatbuff;
	std::string msg = "quick brown fox jumps over the lazy dog";
	const unsigned count = 1000;
	auto t1 = std::chrono::steady_clock::now();
	for (int i=0; i<count; ++i)
	    TestFlatBuffer(flatbuff, msg, false);
	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Flat buffer Avg time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()/count << " nsec" << std::endl;
}


