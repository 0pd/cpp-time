dylib: main.cpp
	c++ main.cpp -Wall -Werror --std=c++14 -o dylib

shared: shared.cpp
	c++ -Wall -Werror --std=c++14 -shared -fPIC shared.cpp -o shared.dylib

clean:
	rm -f dylib shared.dylib
