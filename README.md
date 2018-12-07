# 😀Scanner
---
> FBI warning 该程序需要在linux上运行，而不是windows，请不要尝试在windows环境下编译或运行

`Scanner`是一个网络扫描器，可以扫描当前局域网下的所有主机，并显示他们的主机名、`IP`、开放的`TCP`端口，是`NUAA 网络通信实现技术`的课设

# 💻 编译
如果还没有安装编译套件，请先安装`build-essential`包，以`Ubuntu/Debian`系为例，指令如下：

```
sudo apt-get install build-essential
```

接下来，进入克隆的项目的根目录：

```
cd ./scanner
```

使用我写的编译脚本开始编译：

```
./build.sh
```

如果无法执行，尝试赋予更高的权限或以`sudo`账户执行：

```
sudo chmod 777 ./build.sh
sudo ./build.sh
```

# 🏃 运行
基本运行：

```
sudo ./scanner -ip ipAddress [startPort] [endPort] [threadNum]
sudo ./scanner -lan [startPort] [endPort] [threadNum]
```

参数及用法说明：
* `-ip`：扫描特定`IP`地址的主机的`TCP`端口
    * `ipAddress`：要扫描的主机的`IP`地址
    * `startPort`：扫描开始的端口号
    * `endPort`：扫描结束的端口号
    * `threadNum`：同时扫描的线程数
* `-lan`：扫描当前局域网下所有主机的`IP`地址、主机名，并且对所有主机进行`TCP`端口扫描
    * `startPort`：扫描开始的端口号
    * `endPort`：扫描结束的端口号
    * `threadNum`：同时扫描的线程数

# 😘 关于
* 江湖艺名：John Kindem
* ID in NUAA：161520311

觉得不错的话记得点个小星星⭐
