ALL: COMPILE RUN

COMPILE: src/*.c
	x86_64-w64-mingw32-gcc -o build/main.exe -std=c89 src/*.c -lgdi32

RUN:
	wine build/main.exe
