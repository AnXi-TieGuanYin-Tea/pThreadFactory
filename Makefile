ALL: unittest

unittest: unittest.cc factory.h factory.cc libgtest.a
	g++ -o unittest factory.cc unittest.cc libgtest.a -lpthread
