all:
	gcc -O0 -g -Wall main.c -o pinmuxdump

arm:
	$(CROSS_COMPILE)gcc -O0 -D__ARM -g -static -Wall main.c -o pinmuxdump

clean:
	rm -f *.o pinmuxdump
