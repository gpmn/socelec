all:
	enumivocpp -g soccer.abi soccer.cpp
	enumivocpp -o soccer.wast soccer.cpp
setup:
#	clenu set contract soccer ./ ./soccer.wast ./soccer.abi -p soccer
	enucli set contract fifagambling ./ ./soccer.wast ./soccer.abi -p fifagambling

clean:
	-rm -rf soccer.wast soccer.abi
