#pragma once
#include <iostream>

#include "Loginit.h"
#include "BaseServer.h"
#include "RpcService.h"
#include "RpcProcedure.h"
#include "RpcServer.h"

#include "stealThreadPool.h"
#include "functionWrapper.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::JrpcProto;
using namespace Nano::Communication;
using namespace Nano::Rpc;
using namespace Nano::Concurrency;


void hellocallbackDone(Json::Value response) {
	std::cout << "Response: " << response["result"].asString() << std::endl;
}

void helloCallback(Json::Value& request, const RpcDoneCallback& done) {
    std::string name = request["params"]["name"].asString();
    Json::Value response;
    response["result"] = "Hello, " + name + "!";
    done(response);
}

void helloPc()
{
    // 创建 hello RpcProcedure
    auto helloProcedure = std::make_shared<ProcedureReturn>(
        helloCallback,
        "name", Json::ValueType::stringValue
    );

    // 模拟 RPC 请求
    Json::Value request;
    request["jsonrpc"] = "2.0";
    request["method"] = "hello";
    request["params"]["name"] = "World";

    // 调用过程
    helloProcedure->invoke(request, [](Json::Value response) {
        std::cout << "Response: " << response["result"].asString() << std::endl;
    });
}

void hello()
{
	RpcService rpcService;
	auto helloProcedure = std::make_unique<ProcedureReturn>(
		helloCallback,
		"name", Json::ValueType::stringValue
	);
    rpcService.addProcedureReturn("helloService", std::move(helloProcedure));
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "helloService";
	request["params"]["name"] = "World";

	rpcService.callProcedureReturn("helloService", request, [](Json::Value response) {
		std::cout << "Response: " << response["result"].asString() << std::endl;
	});
}

void subtractCallback(Json::Value& request, const RpcDoneCallback& done) {
    int subtrahend = request["params"]["subtrahend"].asInt();
    int minuend = request["params"]["minuend"].asInt();
    Json::Value response;
    response["result"] = minuend - subtrahend;
    done(response);
}

void substractPc()
{
    // 创建 subtract RpcProcedure
    auto subtractProcedure = std::make_shared<ProcedureReturn>(
        subtractCallback,
        "subtrahend", Json::ValueType::intValue,
        "minuend", Json::ValueType::intValue
    );

    // 模拟 RPC 请求
    Json::Value request;
    request["jsonrpc"] = "2.0";
    request["method"] = "subtract";
    request["params"]["subtrahend"] = 23;
    request["params"]["minuend"] = 42;

    // 调用过程
    subtractProcedure->invoke(request, [](Json::Value response) {
        std::cout << "Response: " << response["result"].asInt() << std::endl;
    });
}

void substract()
{
	RpcService rpcService;
	auto subtractProcedure = std::make_unique<ProcedureReturn>(
		subtractCallback,
		"subtrahend", Json::ValueType::intValue,
		"minuend", Json::ValueType::intValue
	);
	rpcService.addProcedureReturn("subtractService", std::move(subtractProcedure));
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "subtractService";
	request["params"]["subtrahend"] = 23;
	request["params"]["minuend"] = 42;

	rpcService.callProcedureReturn("subtractService", request, [](Json::Value response) {
		std::cout << "Response: " << response["result"].asInt() << std::endl;
		});
}

void sub()
{
    const char* jsonStr = "{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":{\"subtrahend\":23,\"minuend\":42},\"id\":\"132\"}";
    bool flag = false;
    JsonRpcRequest::Ptr request = JrpcRequestGenerator::generate(jsonStr, &flag);

	if (flag) {
		std::cout << "Request: " << request->toJsonStr() << std::endl;
	}
	else {
		std::cout << "Invalid JSON-RPC request" << std::endl;
	}
}

void threadPoolTest()
{
	auto stealThreadPool = StealThreadPool::GetInstance();

    auto helloProcedure = std::make_shared<ProcedureReturn>(
        helloCallback,
        "name", Json::ValueType::stringValue
    );

    Json::Value request;
    request["jsonrpc"] = "2.0";
    request["method"] = "hello";
    request["params"]["name"] = "World";

    // 使用 lambda 函数提交任务到线程池
    auto future = stealThreadPool->submit([helloProcedure, request]() mutable {
        helloProcedure->invoke(request, [](Json::Value response) {
            std::cout << "Response: " << response["result"].asString() << std::endl;
		/*	ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "BaseServer") << "Response: " << response["result"].asString();*/
        });
    });

    // 如果您确实想使用 std::bind，可以这样做：
    //auto future = stealThreadPool->submit(std::bind(
    //    static_cast<void (ProcedureReturn::*)(Json::Value&, const RpcDoneCallback&)>(&ProcedureReturn::invoke),
    //    helloProcedure.get(),
    //    std::ref(request),
    //    [](const Json::Value& response) {
    //        std::cout << "Response: " << response["result"].asString() << std::endl;
    //    }
    //));

    try {
        future.get();
    }
    catch (const std::exception& e) {
        std::cerr << "Error occurred: " << e.what() << std::endl;
    }
}

void rpcserverTest()
{
	InitLoggers();
	RpcServer rpcServer(9800);
	rpcServer.Start();
	system("pause");
	rpcServer.Stop();
}