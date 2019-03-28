CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c

clean:

	rm -f main *.o *.out

tests:
	#generating test
	for recordSize in 4 512 4096 8192; do \
		for numberOfRecords in 5000 10000 100000; do \
			echo "Task: generate file with $$numberOfRecords records of size $$recordSize"; \
			./main generate dane $$numberOfRecords $$recordSize; \
			rm dane; \
		done; \
	done

	#copy test	
	for recordSize in 4 512 4096 8192; do \
		for numberOfRecords in 5000 100000; do \
		echo "-----"; \
		echo "-----"; \
		echo "-----"; \
		echo "Task: copy $$numberOfRecords records of size $$recordSize"; \
		echo "Generating needed file"; \
		./main generate dane $$numberOfRecords $$recordSize; \
			for testVersion in sys lib; do \
				echo "------------------------------------------------"; \
				echo "Version: $$testVersion functions"; \
				./main copy dane dane_CPY $$numberOfRecords $$recordSize $$testVersion; \
				rm dane_CPY; \
			done; \
		done; \
	done


	#sorting test
	for recordSize in 4 512 4096 8192; do \
		for numberOfRecords in 2000 10000; do \
			echo "-----"; \
			echo "-----"; \
			echo "-----"; \
			echo "Task: sort $$numberOfRecords records of size $$recordSize"; \
			echo "Generating needed file"; \
			./main generate dane $$numberOfRecords $$recordSize; \
			cp dane dane_COPY; \
			echo "------------------------------------------"; \
			echo "Version: lib functions"; \
			./main sort dane $$numberOfRecords $$recordSize lib; \
			echo "------------------------------------------"; \
			echo "Version: sys functions"; \
			./main sort dane_COPY $$numberOfRecords $$recordSize sys; \
			rm dane; \
			rm dane_COPY; \
		done; \
	done



	