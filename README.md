# CSAPP Lab

## 项目说明
本项目是计算机系统基础的实验课程及计算机系统结构的实验课程，内容为CSAPPlab的前三个实验：数据表示、二进制炸弹、缓冲区溢出攻击和第五个实验：高速缓冲器模拟

## 实验环境
Linux 64bit、C

## 文件组织结构
reference 目录存放项目的任务书和实验初始文件<br>
report 目录存放项目的报告<br>
lab1-Datalab.c 文件是数据表示实验中bits.c的完成代码<br>
lab2-BinaryBomb.txt 文件是二进制炸弹实验的答案文本<br>
lab3-BufferOverflowAttacks 目录存放的是5个难度级别的攻击字符串txt文件<br>
lab5-CacheLab 目录存放的是Part A：cache模拟实验的任务代码csim.c和Part B：优化矩阵转置的任务代码trans.c<br>

## 代码检查方法
### lab1
```c
语法检查
 $ ./dlc lab1-Datalab.c             //简单语法检查
 $ ./dlc -e lab1-Datalab.c          //检查操作运算符是否符合需求
编译生成可执行文件
 $ make                             //make完成编译，链接，执行文件生成
正确性检查               
 $ ./btest                         //检查bits所有函数功能，失败给出测试用例
 $ ./btest -f byteNot               //检查单个函数，失败给出测试用例
 $ ./btest -f byteNot -1 0xf -2 1   //规定测试用例检查
```

### lab2
```c
 $ ./bomb lab2-BinaryBomb.txt
//程序会自动读取文本文件中的字符串，并依次检查对应每一阶段的字符串来决定炸弹拆除成败。
```

### lab3
```c
//阶段1 Smoke
 $ cat smoke_U202015332.txt |./hex2raw |./bufbomb -u U202015332
//阶段2 Fizz
 $ cat fizz_U202015332.txt |./hex2raw |./bufbomb -u U202015332
//阶段3 Bang
 $ cat bang_U202015332.txt |./hex2raw |./bufbomb -u U202015332
//阶段4 Boom
 $ cat boom_U202015332.txt |./hex2raw |./bufbomb -u U202015332
//阶段5 Nitro
 $ cat nitro_U202015332.txt |./hex2raw |./bufbomb -u U202015332
```

### lab5
```c
//Part A
 $ make && ./test-csim
//Part A + Part B
 $ make && ./driver.py
```