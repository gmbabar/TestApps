// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/overview/core/buffers.html

#include <boost/asio.hpp>   // muteable_buffer, const_buffer and buffer()

#include <boost/beast/core/multi_buffer.hpp> // flat_buffer

int main () {

	// construct
	boost::beast::multi_buffer buffer(1024);
	buffer.max_size(1024);	// limit max_size

	// consume 6 bytes.
	auto prepBuff = buffer.prepare(10);
	for (auto it : prepBuff)
	{
		printf("writable:[%ld]\n", it.size());
		memset(it.data(),'a', it.size());
	}
	buffer.commit(6);

	// prep and consume another 7 bytes
	auto buf2 = buffer.prepare(7);
	for (auto it : buf2)
	{
		printf("writable:[%ld]\n", it.size());
		memset(it.data(),'A', it.size());
	}
	buffer.commit(7);

	// read available data.
	auto bufs = buffer.data();
	size_t aa=bufs.buffer_bytes();
	printf("totalsize:%ld\n",aa);
	char temp[100];
	for(auto it : bufs)  
	{
		memcpy(temp, it.data(), it.size());
		for (int i = 0; i < it.size(); ++i)  printf("[%c]", temp[i]); printf("\n");
	}
}


