mkv-chap-gen: mkv-chap-gen.c
	gcc -Wall -Wextra -g -O2 -std=c99 -fstack-protector-strong -fPIC -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -Wl,-z,now,-z,defs,-z,relro,--as-needed -pie -o mkv-chap-gen mkv-chap-gen.c

clean:
	rm -f mkv-chap-gen
