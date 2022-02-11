
all:
	gcc mcastrecv_liburing.c -o mcastrecv_liburing -luring
	gcc mcastrecv_liburing_ex.c -o mcastrecv_liburing_ex -luring
	gcc mcastsend_liburing.c -o mcastsend_liburing -luring
	gcc mcastsend_liburing_ex.c -o mcastsend_liburing_ex -luring
	g++ mcastrecv_iouring.cpp -o mcastrecv_iouring -std=c++11 -luring
	g++ iouring_msgsend.cpp -o iouring_msgsend -luring
	g++ iouring_msgrecv.cpp -o iouring_msgrecv -luring
	g++ iouring_read_fixed.cpp -o iouring_read_fixed -luring

one:
	g++ iouring_msgsend.cpp -o iouring_msgsend -luring

clean:
	rm -f mcastrecv_liburing mcastrecv_liburing_ex mcastsend_liburing mcastsend_liburing_ex mcastrecv_iouring iouring_msgsend iouring_msgrecv iouring_read_fixed
