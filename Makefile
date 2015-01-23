
b.objdump: b
	objdump -DS b > b.objdump

b: b.cpp
	g++ -g -ob b.cpp

clean:
	rm -f b b.objdump
