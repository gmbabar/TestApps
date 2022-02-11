
all: authe opn trd feed err cncl sub neword

authe:
	g++ -std=c++11 auth.cpp -o auth -lboost_system

opn:
	g++ -std=c++11 open.cpp -o open -lboost_system

trd:
	g++ -std=c++11 trade.cpp -o trade -lboost_system

feed:
	g++ -std=c++11 feed.cpp -o feed -lboost_system

err:
	g++ -std=c++11 error.cpp -o error -lboost_system

# Same name of binary makes issues.
cncl:
	g++ -std=c++11 cancel.cpp -o cancel -lboost_system

sub:
	g++ -std=c++11 subscribe.cpp -o subscribe -lboost_system

neword:
	g++ -std=c++11 neworder.cpp -o neworder -lboost_system

clean:
	rm -f auth open trade feed error cancel subscribe neworder

