

all: main.o arrayList.o stringBuilder.o
	gcc -o main.out main.o ../common/datastructures/arrayList.o ../common/datastructures/stringBuilder.o

main.o: main.c
	gcc -c -o main.o main.c

arrayList.o: ../common/datastructures/arrayList.c
	gcc -c -o ../common/datastructures/arrayList.o ../common/datastructures/arrayList.c

stringBuilder.o: ../common/datastructures/stringBuilder.c
	gcc -c -o ../common/datastructures/stringBuilder.o ../common/datastructures/stringBuilder.c