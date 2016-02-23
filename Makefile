mkv-chap-gen: mkv-chap-gen.c
	gcc -Wall -g -O2 -std=c99 -o mkv-chap-gen mkv-chap-gen.c

clean:
	rm -f mkv-chap-gen
