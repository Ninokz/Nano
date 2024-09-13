#pragma once
#include <iostream>

#include "Loginit.h"
#include "BaseServer.h"
#include "RpcService.h"

#include "ceventhandler.h"

#include "RpcProcedure.h"
#include "RpcServer.h"
#include "RpcServerStub.h"

#include "RpcClient.h"
#include "RpcClientStub.h"


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

void helloworldProcedure(Json::Value& request, const RpcDoneCallback& done) {
    std::string name = request["params"]["name"].asString();
    Json::Value response;
    response["result"] = "Hello, " + name + "!";
    done(response);
}

void hello()
{
	RpcService rpcService;
    std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
        {"name", Json::ValueType::stringValue}
    };
	auto helloProcedureReturn = std::make_unique<ProcedureReturn>(
        helloworldProcedure,
        paramsNameTypesMap
	);
    rpcService.addProcedureReturn("helloService", std::move(helloProcedureReturn));

	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "helloService";
	request["params"]["name"] = "World";
	rpcService.callProcedureReturn("helloService", request, hellocallbackDone);
}

void substractcallbackDone(Json::Value response) {
	std::cout << "Response: " << response["result"].asInt() << std::endl;
}

void subtractProcedure(Json::Value& request, const RpcDoneCallback& done) {
    int subtrahend = request["params"]["subtrahend"].asInt();
    int minuend = request["params"]["minuend"].asInt();
    Json::Value response;
    response["result"] = minuend - subtrahend;
    done(response);
}

void substract()
{
	RpcService rpcService;
    std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
      {"subtrahend", Json::ValueType::intValue},
      {"minuend", Json::ValueType::intValue}
    };
	auto subtractProcedureReturn = std::make_unique<ProcedureReturn>(
        subtractProcedure,
        paramsNameTypesMap
	);

	rpcService.addProcedureReturn("subtractService", std::move(subtractProcedureReturn));
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "subtractService";
	request["params"]["subtrahend"] = 23;
	request["params"]["minuend"] = 42;

	rpcService.callProcedureReturn("subtractService", request, substractcallbackDone);
}

void threadPoolTest()
{
	auto stealThreadPool = StealThreadPool::GetInstance();

    auto helloProcedure = std::make_shared<ProcedureReturn>(
        helloworldProcedure,
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


void helloworldReturnService(Json::Value& request, const RpcDoneCallback& done) {
    std::string name = request["name"].asString();
    Json::Value response;
    response["result"] = "Hello, " + name + "!";
    done(response);
}

void RpcServerStubHelloWorldTest() {
    InitLoggers();
    RpcServerStub::Ptr rpcServerStub = std::make_shared<RpcServerStub>(9800);
    std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
      {"name", Json::ValueType::stringValue}
    };

    rpcServerStub->registReturn("helloworldMethod", paramsNameTypesMap, helloworldReturnService);
    rpcServerStub->run();
    system("pause");
    rpcServerStub->stop();
}

void helloworldCallback(Json::Value response) {

};

void ClientStubHelloWorldTest() {
    InitLoggers();
    RpcClientStub::Ptr rpcClientStub = std::make_shared<RpcClientStub>();
	std::unordered_map<std::string, Json::Value> params = {
	  {"name", "World"}
	};
	rpcClientStub->rpcReturnCall("127.0.0.1", 9800, "1", "helloworldMethod", params, helloworldCallback, 3000);
}