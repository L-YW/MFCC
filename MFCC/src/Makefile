app.out : main.o MFCC.o std_ft.o learn_classify.o intellino_spi.o
	g++ -o app.out main.o MFCC.o std_ft.o learn_classify.o intellino_spi.o

main.o : main.cpp
	g++ -c -o main.o main.cpp

MFCC.o : MFCC.cpp
	g++ -c -o MFCC.o MFCC.cpp 

std_ft.o : std_ft.cpp
	g++ -c -o std_ft.o std_ft.cpp

learn_classify.o : learn_classify.cpp
	g++ -c -o learn_classify.o learn_classify.cpp

intellino_spi.o : intellino_spi.cpp
	g++ -c -o intellino_spi.o intellino_spi.cpp

clean :
	rm -f *.o
	rm -f app.out