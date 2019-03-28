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
	rm -f *.o *.a *.so testS
	
all:
	make clean
	make all_O
	make clean
	make all_Os
	make clean
	make all_O0
	make clean
	make all_O1
	make clean
	make all_O2
	make clean
	make all_O3
	make clean	
	
all_O:
	make zad3_static_O
	make zad3_dynamic_O
	make zad3_shared_O

all_O0:
	make zad3_static_O0
	make zad3_dynamic_O0
	make zad3_shared_O0
all_O1:
	make zad3_static_O1
	make zad3_dynamic_O1
	make zad3_shared_O1

all_O2:
	make zad3_static_O2
	make zad3_dynamic_O2
	make zad3_shared_O2

all_O3:
	make zad3_static_O3
	make zad3_dynamic_O3
	make zad3_shared_O3

all_Os:
	make zad3_static_Os
	make zad3_dynamic_Os
	make zad3_shared_Os
	
zad3_static_O:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -O
	make test1
	make test2
	
zad3_shared_O:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -O
	make test1
	make test2

zad3_dynamic_O:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -O
	make test1
	make test2
	
zad3_static_O1:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -O1
	make test1
	make test2
	
zad3_shared_O1:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -O1
	make test1
	make test2

zad3_dynamic_O1:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -O1
	make test1
	make test2


zad3_static_O2:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -O2
	make test1
	make test2
	
zad3_shared_O2:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -O2
	make test1
	make test2

zad3_dynamic_O2:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -O2
	make test1
	make test2


zad3_static_O3:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -O3
	make test1
	make test2
	
zad3_shared_O3:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -O3
	make test1
	make test2

zad3_dynamic_O3:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -O3
	make test1
	make test2

zad3_static_O0:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -O0
	make test1
	make test2
	
zad3_shared_O0:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -O0
	make test1
	make test2

zad3_dynamic_O0:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -O0
	make test1
	make test2

zad3_static_Os:
	make static
	$(CC) -o testS main.c blockManagement.a -I . -Os
	make test1
	make test2
	
zad3_shared_Os:
	make shared
	$(CC) -o testS main.c -L. blockManagement.a -I . -Os
	make test1
	make test2

zad3_dynamic_Os:
	make shared
	$(CC) -o testS main_D.c -ldl -I . -Os
	make test1
	make test2

test1:
	#staticAlloction
	./testS 300000 600 0 search_element 3024 remove 100000 0 add 100000 0 remove_and_add 30000
test2:
	#dynamicAllocation
	./testS 300000 600 1 search_element 3024 remove 100000 0 add 100000 0 remove_and_add 30000

