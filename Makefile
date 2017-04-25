mkv-chap-gen: mkv-chap-gen.c
	gcc -Wall -g -O2 -std=c99 -pedantic -fstack-protector-strong -fPIC -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -Wl,-z,now -pie -o mkv-chap-gen mkv-chap-gen.c

clean:
	rm -f mkv-chap-gen
