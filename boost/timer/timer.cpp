
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void onTimeout(const boost::system::error_code& ec) { std::cout << __func__ << "------------------------------------" << std::endl; }
void onTimeout2(const boost::system::error_code& ec) { std::cout << __func__ << "------------------------------------" << std::endl; }

int main()
{
  boost::asio::io_service io;

    // check if we set multplie timeouts.
    boost::asio::deadline_timer _timer(io);
    _timer.expires_from_now(boost::posix_time::seconds(1));
    _timer.async_wait(onTimeout);

  for(int i = 0; i < 5; i++) {
    //boost::asio::deadline_timer timer(io, boost::posix_time::seconds(i));
    boost::asio::deadline_timer timer(io);
    std::cout << i << ": timer created. expires_at: " << timer.expires_at() << ", special: " << timer.expires_at().is_special() << std::endl;
    std::cout << i << ": timer created. expires_from_now: " << timer.expires_from_now() << ", special: " << timer.expires_from_now().is_special() << std::endl;
    std::cout << "setting up expiry." << std::endl;
    timer.expires_from_now(boost::posix_time::seconds(i));
    std::cout << i << ": timer created. expires_at: " << timer.expires_at() << std::endl;
    std::cout << i << ": timer created. expires_from_now: " << timer.expires_from_now() << std::endl;
    timer.wait();
    std::cout << "Blocking wait(): " << i << " second-wait\n";
    timer.cancel();
    timer.expires_from_now(boost::date_time::not_a_date_time);
    std::cout << i << ": timer cancelled. expires_at: " << timer.expires_at() << ", special: " << timer.expires_at().is_special() << std::endl;
    std::cout << i << ": timer cancelled. expires_from_now: " << timer.expires_from_now() << ", special: " << timer.expires_from_now().is_special() << std::endl;

    std::cout << std::endl;
  }

    _timer.expires_from_now(boost::posix_time::seconds(2));
    _timer.async_wait(onTimeout2);

  io.run();
  return 0;
}
