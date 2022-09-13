g++.exe -Wall -fexceptions -g -std=gnu++11 -Iinclude -c ".\main.cpp" -o main.o
g++.exe  -o Superficie.exe main.o   -lglut32 -lopengl32 -lglu32
