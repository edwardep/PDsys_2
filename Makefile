all_sse:
	$(MAKE) sse sse_s
	@echo "\n\n"
	@./sse_s $(N)
	@./sse $(N)

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

sse_s: sse_s.o
	gcc -o sse_s sse_s.c

sse_s.o : sse_s.c
	gcc -c -o sse_s.o sse_s.c

sse: sse.o
	gcc -o sse sse.c -msse4.2 -Wall

sse.o : sse.c
	gcc -c -o sse.o sse.c -msse4.2 -Wall

clean:
	rm -f dummy dummy.o
	rm -f dummy_s dummy_s.o
	rm -f sse sse.o
	rm -f sse_s sse_s.o