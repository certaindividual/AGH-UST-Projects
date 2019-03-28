#CC - wykorzystywany kompilator
CC = gcc -Wall -std=c11 -ggdb

#	[cel/regula]: [pliki zalezne/reguly potrzebne do wykonania przed ta regula] 
#			polecenie
#cel - nazwa pliku, ktory ma powstac
#zaleznosci - lista plikow, od ktorych zalezy cel
#polecenie - polecenie, za pomoca ktorego tworzy sie cel/wykonuje regula

#$(CC) - uzycie zmiennej

complete: static shared clean

static:
	$(CC) -c blockManagement.c
#tworzenie biblioteki statycznej o rozszerzeniu .a - polecenie ar
	ar -crs blockManagement.a blockManagement.o 

shared:
#kompilacja kodu do uzycia jako biblioteka dynamiczna
#fPIC - Position Independent Code
#biblioteka wspoldzielona - rozszerzenie .so
	$(CC) -c -fPIC blockManagement.c
	$(CC) -shared -fPIC -o blockManagement.so blockManagement.o

clean:
#rm -f usuwanie plików o nazwie...
	rm -f *.o
