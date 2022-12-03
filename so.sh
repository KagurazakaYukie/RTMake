clang main.c Compile.c MultitThreadMemoryManager.c a/b/c.c MArrayList.c -c

clang main.o Compile.o MultitThreadMemoryManager.o c.o MArrayList.o -lpthread -o demo

cp demo /run/media/yukie/Yukie存储盘/SSLServer/RTMake

cd /home/yukie/RTMake || ! echo "Failure" && ./RTMake
