clang main.c compile/Command.c compile/MultitThreadMemoryManager.c compile/MArrayList.c -c

clang main.o Command.o MultitThreadMemoryManager.o MArrayList.o -O3 -W -lpthread -o RTMake

rm -f main.o Command.o MultitThreadMemoryManager.o MArrayList.o

cp RTMake /mnt/5C24-7D13/DataLibrary_Lib/AutoBuild/RTMake

cd /mnt/5C24-7D13/DataLibrary_Lib/AutoBuild || ! echo "Failure" && ./RTMake
