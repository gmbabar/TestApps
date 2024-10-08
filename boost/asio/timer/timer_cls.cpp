#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

class printer {
public:
  printer(boost::asio::io_context& io) : strand_(io),
      timer1_(io, boost::asio::chrono::seconds(1)),
      timer2_(io, boost::asio::chrono::seconds(1)),
      count_(0) {
    timer1_.async_wait(
	boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this))
    );

    timer2_.async_wait(
	boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this))
    );
  }

  ~printer() {
    std::cout << "Final count is " << count_ << std::endl;
  }

  void print1() {
    if (count_ < 10) {
      std::cout << "Timer 1: " << count_ << std::endl;
      ++count_;

      timer1_.expires_after(boost::asio::chrono::seconds(1));
      timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print1, this)));
    }
  }

  void print2() {
    if (count_ < 10) {
      std::cout << "Timer 2: " << count_ << std::endl;
      ++count_;

      timer2_.expires_after(boost::asio::chrono::seconds(1));
      timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printer::print2, this)));
    }
  }

private:
  /**
   * strand ensures saftey of shared resources and make sure only one callback executes at any given time
   * and blocks parallel execution even if we have multiple threads attached to io_context.
   **/
  boost::asio::io_context::strand strand_;
  boost::asio::steady_timer timer1_;
  boost::asio::steady_timer timer2_;
  int count_;
};

int main() {
  boost::asio::io_context io;
  std::cout << __func__ << ": Creating printer..." << std::endl;
  printer p(io);
  std::cout << __func__ << ": Creating boost..." << std::endl;
  boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
  std::cout << __func__ << ": Running io_context..." << std::endl;
  io.run();
  std::cout << __func__ << ": Waiting for thread to join..." << std::endl;
  t.join();
  std::cout << __func__ << ": Done." << std::endl;
  return 0;
}



