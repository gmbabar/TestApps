#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <iostream>

#include <string>
#include <sstream>
#include <boost/asio.hpp>

#include <unistd.h>


boost::lockfree::spsc_queue<boost::asio::const_buffer> q{100};

char buff[1024];

void produce()
{
  std::cout << "produce: started" << std::endl;
  for (int i = 1; i <= 100; ++i) {
    std::ostringstream oss;
    oss << "producer item: " << i;
    std::string request = oss.str();
    int len = sprintf(buff, "producer item: %d", i );
    q.push( boost::asio::buffer(buff, len) );
  }
  std::cout << "produce: ended" << std::endl;
}

void consume()
{
  std::cout << "consume: started" << std::endl;
  boost::asio::const_buffer buff;
  int counter=0;
  while (q.pop(buff)) {
    std::cout << "consume: pop: " << (char*)buff.data() << std::endl;
    ++counter;
  }
  std::cout << "consume: ended, processed: " << counter << " items" << std::endl;
}

int main()
{
  std::cout << "main: starting producer thread" << std::endl;
  std::thread t1{produce};
  std::cout << "main: starting consumer thread" << std::endl;
  std::thread t2{consume};
  std::cout << "main: waiting for joining producer" << std::endl;
  t1.join();
  std::cout << "main: waiting for joining consumer" << std::endl;
  t2.join();
  std::cout << "main: calling consumer" << std::endl;
  consume();
  std::cout << "main: done." << '\n';
}

