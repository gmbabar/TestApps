/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <chrono>
#include <unistd.h>

namespace
{
  struct in_addr localInterface;
  struct sockaddr_in groupSockAddr;
  int sockfd;
  char databuf[1024] = "Obesity in the United States is a major health issue resulting in numerous diseases," 
    "specifically increased risk of certain types of cancer, coronary artery disease, type 2 diabetes, stroke, "
    "as well as significant increases in early mortality and economic costs.";
  // int datalen = sizeof(databuf);
  int datalen = strlen(databuf)+1;
}

int main(int argc, char *argv[])
{

  std::string interface_ip = "127.0.0.1";
  std::string multicast_ip = "239.0.0.1";
  unsigned short multicast_port = 12345;
  int count = 10000;

  std::string input;
  std::cout << "Enter interface address, enter to use default [" << interface_ip << "]: ";
  std::getline(std::cin, input);
  if (input.size() > 0)
  {
    interface_ip = input;
  }

  std::cout << "Enter multicast address, enter to use detault [" << multicast_ip << "]: ";
  std::getline(std::cin, input);
  if (input.size() > 0)
  {
    multicast_ip = input;
  }

  std::cout << "Enter multicast port, enter to use detault [" << multicast_port << "]: ";
  std::getline(std::cin, input);
  if (input.size() > 0)
  {
    multicast_port = static_cast<unsigned short>(std::atoi(input.c_str()));
  }

  /* Create a datagram socket on which to send. */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("Opening datagram socket error");
    exit(1);
  }
  std::cout << "Opening the datagram socket...OK" << std::endl;

  /* Initialize the group sockaddr structure with a */
  /* group address of 225.1.1.1 and port 5555. */
  memset((char *)&groupSockAddr, 0, sizeof(groupSockAddr));
  groupSockAddr.sin_family = AF_INET;
  groupSockAddr.sin_addr.s_addr = inet_addr(multicast_ip.c_str());
  groupSockAddr.sin_port = htons(multicast_port);

  /* Disable loopback so you do not receive your own datagrams.
  {
    char loopch = 0;
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0) {
      perror("Setting IP_MULTICAST_LOOP error");
      close(sd);
      exit(1);
    } else {
      printf("Disabling the loopback...OK.\n");
    }
  }
  */

  /* Set local interface for outbound multicast datagrams. */
  /* The IP address specified must be associated with a local, */
  /* multicast capable interface. */
  localInterface.s_addr = inet_addr(interface_ip.c_str());
  if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&localInterface, sizeof(localInterface)) < 0)
  {
    perror("Setting local interface error");
    exit(1);
  }
  std::cout << "Setting the local interface...OK" << std::endl;

  /* Send a message to the multicast group specified by the*/
  /* groupSock sockaddr structure. */
  /*int datalen = 1024;*/
  std::cout << "Sending datagram message..." << std::endl;
  int idx=-1;
  int bytes_sent = 0, total_data_size = 0;
  int divide = std::chrono::system_clock::period::den / 1000000;
  std::chrono::system_clock::time_point t1;
  while (++idx<count)
  {
    t1 = std::chrono::system_clock::now();
    datalen = sprintf(databuf, "%ld Sender Time. %d", static_cast<long int>(t1.time_since_epoch().count()/divide), idx);
    bytes_sent = sendto(sockfd, databuf, datalen+1, 0, (struct sockaddr *)&groupSockAddr, sizeof(groupSockAddr));
    if (bytes_sent < 0) {
      perror("Sending datagram message error");
      break;
    }
    usleep(10);
    total_data_size += bytes_sent;
  }
  std::cout << "Sent " << idx << " datagram messages, total size: " << total_data_size << std::endl; 

  /* Try the re-read from the socket if the loopback is not disable
  if(read(sd, databuf, datalen) < 0) {
    perror("Reading datagram message error\n");
    close(sd);
    exit(1);
  } else {
    printf("Reading datagram message from client...OK\n");
    printf("The message is: %s\n", databuf);
  }
  */
  return 0;
}
