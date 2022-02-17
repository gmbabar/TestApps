// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/multi_buffer.hpp> // flat_buffer
#include <iostream>
#include <chrono>

void TestMultiBuffer (boost::beast::multi_buffer &buffer, const std::string& msg, const bool debug = true) {

	// construct
	//boost::beast::multi_buffer buffer(1024);
	buffer.max_size(1024);	// limit max_size
	if (debug) printf("created multi buffer. buff size: %ld\n", buffer.size());

	// consume 39 bytes.
	auto prepBuff = buffer.prepare(msg.size());
	for (auto it : prepBuff)
	{
		if (debug) printf("writable:[%ld]\n", it.size());
		//memset(it.data(),'a', it.size());
		memcpy(it.data(), msg.c_str(), it.size());
	}
	buffer.commit(msg.size());

	// prep and consume another 7 bytes
	if (debug) {
	    auto buf2 = buffer.prepare(7);
	    for (auto it : buf2)
	    {
		if (debug) printf("writable:[%ld]\n", it.size());
		memset(it.data(),'A', it.size());
	    }
	    buffer.commit(7);
	}

	// read available data.
	auto bufs = buffer.data();
	if (debug) {
	    size_t aa=bufs.buffer_bytes();
	    if (debug) printf("totalsize: %ld\n",aa);
	    if (debug) printf("multi buffer size: %ld\n", buffer.size());
	}
	char temp[100];
	for(auto it : bufs)  
	{
		if (debug) std::cout << "buff copied: " << std::endl;
		memcpy(temp, it.data(), it.size());
		if (debug) for (int i = 0; i < it.size(); ++i)  printf("[%c]", temp[i]); 
		if (debug) printf("\n");
	}
	buffer.clear();
}

int main () {
	std::string msg = "quick brown fox jumps over the lazy dog";
	boost::beast::multi_buffer buffer(1024);
        const unsigned count = 10000;
        auto t1 = std::chrono::steady_clock::now();
        for (int i=0; i<count; ++i)
	    TestMultiBuffer(buffer, msg, false); 
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "multi buffer Avg time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()/count << " nsec" << std::endl;

}


