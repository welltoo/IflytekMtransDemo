# README

## 编译
系统 ubuntu18.04

### 安装工具g++、cmake

```
sudo apt install g++
sudo apt install cmake
```



### 安装依赖库ssl、curl

安装openssl

```
sudo apt install openssl
sudo apt install libssl-dev
```

安装curl

```
sudo apt install -y libcurl4 curl
```



## 执行

首先cd切换到demo目录

生成makefile

```
cmake .
```

然后执行make

```
make
```

当前目录会生成可执行文件xfyunfy

执行可执行文件`./xfyunfy`，会在当前目录生成结果文件`curlposttest.log`，结果示例如下：

```
HTTP/1.1 200 OK
Content-Type: application/json; charset=utf-8
Content-Length: 197
Connection: keep-alive
Date: Tue, 18 Feb 2020 17:43:59 GMT
X-Kong-Upstream-Latency: 3
X-Kong-Proxy-Latency: 0
Via: kong/1.3.0

{"code":0,"data":{"result":{"from":"cn","to":"en","trans_result":{"dst":"How is the weather today?","src":"今天天气怎么样？"}}},"message":"success","sid":"its00088a55@dx170596646dca11d902"}
```

