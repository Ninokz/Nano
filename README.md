# Nano 🌠

## Intro.

`Nano Frameworks` 是使用 `C++ 14` 编写的基于 `Boos.Asio` 异步 RPC 框架，端间采用 `json` 进行 `request` 与 `response` 传输，消息传输协议采用 [JSON-RPC 2.0](https://www.jsonrpc.org/specification) 规范. 框架结构如下图

![](https://observer-storage.oss-cn-chengdu.aliyuncs.com/github/Nano/nano-rpcdesign.png)

框架底层网络库采用 [Boost](https://www.boost.org/users/history/version_1_85_0.html) 的 `Asio` 库，其支持多平台，理论上`Nano Frameworks` 框架也能在 Linux 上编译，只不过目前建立的是一个 Visual Studio 的工程，此外这个框架还支持异步日志、多种类型线程池选用等常见的功能。

## Dependency.

- [Boost 1.85.0](https://www.boost.org/users/history/version_1_85_0.html)

## Use Example.

只需要向 `RpcServerStub::Pre` 对象中使用 `registReturn` 方法注册 RPC 方法，注册的信息需要方法需要的参数及其类型即可



## TODO.

- [ ] Generator for Service and Request
- [ ] More Test

## Ref.

- [libjson-rpc-cpp](https://github.com/cinemast/libjson-rpc-cpp): C++ framework for json-rpc
- [jrpc](https://github.com/guangqianpeng/jrpc): JSON-RPC implementation in C++17
- [llfc blog](https://www.llfc.club/category?catid=225RaiVNI8pFDD5L4m807g7ZwmF) 