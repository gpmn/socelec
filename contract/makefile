all:
	eosiocpp -g soccer.abi soccer.cpp
	eosiocpp -o soccer.wast soccer.cpp
setup:
#	cleos set contract soccer ./ ./soccer.wast ./soccer.abi -p soccer
	cleos set contract fifagambling ./ ./soccer.wast ./soccer.abi -p fifagambling

clean:
	-rm -rf soccer.wast soccer.abi
