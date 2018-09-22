
.DEFAULT=build

build:
	gcc main.c -o bin/timenator

clear:
	rm timenator