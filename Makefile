all_dumm:
	$(MAKE) dummy dummy_s
	./dummy_s $(N)
	./dummy $(N)

dummy_s: dummy_s.o
	gcc -o dummy_s dummy_s.c

dummy_s.o : dummy_s.c
	gcc -c -o dummy_s.o dummy_s.c

dummy: dummy.o
	gcc -o dummy dummy.c -msse4.2

dummy.o : dummy.c
	gcc -c -o dummy.o dummy.c -msse4.2

sse: sse.o
	gcc -o sse sse.c -msse4.2

sse.o : sse.c
	gcc -c -o sse.o sse.c -msse4.2


clean:
	rm -f dummy dummy.o
	rm -f dummy_s dummy_s.o