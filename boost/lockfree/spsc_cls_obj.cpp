#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <iostream>

struct Test {
	int val;
	explicit Test(int x) : val(x) {
	}
	Test() = default;
};

struct Queue {
	boost::lockfree::spsc_queue<Test> que;

	Queue(int size) : que(size) {
	}
};


int sum = 0;
Queue q(100);

void produce()
{
  std::cout << "produce: started" << std::endl;
  for (int i = 1; i <= 100; ++i)
    q.que.push(Test(i));
  std::cout << "produce: ended" << std::endl;
}

void consume()
{
  std::cout << "consume: started" << std::endl;
  int counter=0;
  Test t;
  while (q.que.pop(t)) {
    sum += t.val; ++counter;
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
  std::cout << "main: sum: " << sum << '\n';
}

