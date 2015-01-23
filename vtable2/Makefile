all: Main Main.cpp.o.objdump

Main: Main.cpp.o Derive.cpp.o Base.cpp.o Base1.cpp.o
	g++ -g -o $@ $^

%.cpp.o: %.cpp
	g++ -c -g -o $@ $^

%.cpp.o.objdump : %.cpp.o
	objdump -DS $^ > $@

clean:
	rm -f Main *.o *.objdump
