Butterfly : main.o Butterfly.o glad.c
		g++ main.o Butterfly.o glad.c -o Butterfly -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lpthread -lGL

main.o : main.cpp
		g++ -c main.cpp -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lpthread -lGL

Butterfly.o : Butterfly.hpp Butterfly.cpp
		g++ -c Butterfly.cpp -lGL

.PHONY: clean

clean : 
		rm Butterfly *.o