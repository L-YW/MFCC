app.out : main.o MFCC.o std_ft.o 
	g++ -o app.out main.o MFCC.o std_ft.o

main.o : main.cpp
	g++ -c -o main.o main.cpp

MFCC.o : MFCC.cpp
	g++ -c -o MFCC.o MFCC.cpp 

std_ft.o : std_ft.cpp
	g++ -c -o std_ft.o std_ft.cpp

clean :
	rm -f *.o
	rm -f app.out