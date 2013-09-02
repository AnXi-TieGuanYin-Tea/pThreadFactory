ALL: unittest

unittest: unittest.cc pool.h pool.cc libgtest.a
	g++ -o unittest pool.cc unittest.cc libgtest.a -lpthread
