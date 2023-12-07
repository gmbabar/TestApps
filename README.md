# GBabar

This is my personal area to store and share my small tools which I build to support my work. Please feel free to copy and reuse any code suitable to your project without any guarentee or liability. Most of these tools are isoloated boiler plates which I found hard to compose or search on web and AI tools.

-Ghulam M. Babar

------------------
NOTES:
------------------
qdist:
------
- BinaryTickBufferParser   —> BinaryTickBufferParserIouring
- BinaryTickBufferSession —> BinaryTickBufferSessionIouring

- ClientHandler::unsubscribe() -> tickSessionPtr->parser().unsubscribe( socketfd )
- ClientHandler::onQuoteSubscription() -> tickSessionPtr->parser().subscribe( buffer, symbol, socketfd )
- ClientHandler::onNewSession() -> aSessionPtr->parser().subscribe( buffer, symbol, socketfd )

- BinaryTickBufferSessionIoUring = McastTickSessionIouring<BinaryMsgRcvBuf<BinaryTickBufferParserIouring>>
    - McastTickSessionIouring:
        - Asio::udp::socket(io_service*)  —>  x
        - AsioAllocHandler(io_service*)  —>  x
        - struct msghdr m_datagrams
        - IoUring::MulticastReceiver
        - InitUdp()  ———>  initReceiver():
            - Socket’s multicast setup
            - io_service::post( udpReceive[Mmsg] )
        - InitRecvMsg() ———> initReceiver():
            - Initialize m_datagrams (msghdr)
        - UdpReceiveMmsg() 
            - Socket::async_receive( callback )
        - stop()  ———> x
        - handleStop()  ———>  x
- 
