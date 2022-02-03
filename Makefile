CXX=g++
CXXFLAGS=-Wall -Werror -Wpedantic -std=c++11
LIBS=

minify: minify.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

test: example.css minify
	./minify example.css example.min.css

clean:
	rm -rf example.min.css minify
