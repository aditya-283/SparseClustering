cluster: cluster.cpp spectra.cpp 
	g++ -std=c++11 -o cluster cluster.cpp

clean: 
	rm cluster