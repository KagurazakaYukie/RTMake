RTMake(1.5) Doc
================================

一个自动化调用编译命令工具，带没有脑子的自动释放，免去手动释放

未完成的功能：
可视化
历史代码管理
多用户
本地远程调库
编译文件实时检测
带脑子的内存检测
更多错误

### 编译方法(Linux CUI)
- 下载本人的MArrayList、MultitThreadMemoryManager并放在根目录
- 运行run.sh

### 使用方法(Linux CUI)
- 创建 library 文件夹(无library请忽略)
    - 进入 library 文件夹
    - 把需要的library文件直接复制进该文件夹(如果有)
    - 创建 config 文件(如有需要引用已安装library)
    - 写入已安装的library名字，如下面方式写(如果有)
```
pthread
openssl
...
```
- 创建 AutoFree 文件，如下面方式写(可选) 
```
malloc(size)@free(memory)
```

- 如果需要自动释放功能，请在分配函数后面写上 //MemoryFree ，如下面所示(可选) 

```c
void *abc = malloc(1024);//MemoryFree
```

```
malloc为对应需要编译C文件内的分配函数，free为对应需要编译C文件内的释放函

@为区分符

示例 malloc(size)@free(memory)

注意:括号内单词为标记，如果分配和释放函数括号内单词一致，则会引用相同的变量
memory为特殊标记，分配函数所分配的指针
```

```c
//正确使用,变量将正确释放

int main(){
	void *abc = malloc(1024);//MemoryFree
}
```

```c
//错误使用,一定会造成二次释放

int main(){
	void *abc = malloc(1024);//MemoryFree
	free(abc);
}
```
- 执行编译命令(默认)：
    - 复制执行文件到项目根目录
    - 顺序运行下面命令
```sh
./RTMake
cd build
ls
# 全部命令成功执行，执行 ls 命令，将会看到run, lib.so, lib.a
```
