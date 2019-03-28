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
	
zad3_static:
	make static
	$(CC) -o testS main.c blockManagement.a -I .
	make test1
	make test2
	
zad3_shared:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I .
	make test1
	make test2

zad3_dynamic:
	make shared
	$(CC) -o testS main_D.c -ldl -I .
	make test1
	make test2

test1:
	#static
	./testS 300000 600 0 search_element 3024 remove 100000 0 add 100000 0 remove_and_add 30000
test2:
	#shared
	./testS 300000 600 1 search_element 3024 remove 100000 0 add 100000 0 remove_and_add 30000

