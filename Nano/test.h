#pragma once
#include <iostream>

#include "Loginit.h"
#include "BaseServer.h"
#include "RpcService.h"

#include "ceventhandler.h"

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

void hellocallbackDone(Json::Value& response) {
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
	// ���� hello RpcProcedure
	auto helloProcedure = std::make_shared<ProcedureReturn>(
		helloCallback,
		"name", Json::ValueType::stringValue
	);

	// ģ�� RPC ����
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "hello";
	request["params"]["name"] = "World";

	// ���ù���
	helloProcedure->invoke(request, hellocallbackDone);
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

void subtractCallbackDone(Json::Value& response) {
	std::cout << "Response: " << response["result"].asInt() << std::endl;
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
	// ���� subtract RpcProcedure
	auto subtractProcedure = std::make_shared<ProcedureReturn>(
		subtractCallback,
		"subtrahend", Json::ValueType::intValue,
		"minuend", Json::ValueType::intValue
	);

	// ģ�� RPC ����
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "subtract";
	request["params"]["subtrahend"] = 23;
	request["params"]["minuend"] = 42;

	// ���ù���
	subtractProcedure->invoke(request, subtractCallbackDone);
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

	// ʹ�� lambda �����ύ�����̳߳�
	auto future = stealThreadPool->submit([helloProcedure, request]() mutable {
		helloProcedure->invoke(request, [](Json::Value response) {
			std::cout << "Response: " << response["result"].asString() << std::endl;
			/*	ASYNC_LOG_INFO(ASYNC_LOG_NAME("STD_LOGGER"), "BaseServer") << "Response: " << response["result"].asString();*/
			});
		});

	// ʹ�� std::bind��������������
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

void testBaseServer()
{
	InitLoggers();
	BaseServer baseServer(9800);
	baseServer.Start();
	system("pause");
}

void rpcserverTest()
{
	InitLoggers();
	RpcServer::Ptr rpcServer = RpcServer::Create(9800);
	rpcServer->Init();
	rpcServer->Start();
	system("pause");
}

void eventhanldertest()
{
	class A : public IDataReadyEventHandler, public std::enable_shared_from_this<A>
	{
	public:
		A() {
		}

		/// weak_from_this() �� std::enable_shared_from_this ��һ���֣�
		/// ����ֻ���ڶ����Ѿ��� std::shared_ptr ����ʱ��������������
		/// ���� init �����е��� shared_from_this() ����ȡ std::shared_ptr<A> ����
		void init() {
			eventHandler.AddDataReadyHandler(weak_from_this());
		}

		void OnDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet) override {
			if (sender && packet) {
				std::cout << "A: OnDataReady" << std::endl;
			}
			else {
				std::cout << "A: OnDataReady nullptr" << std::endl;
			}
		}
	public:
		CEventHandler eventHandler;
	};

	// ʹ�� std::shared_ptr ������ A ����
	std::shared_ptr<A> a = std::make_shared<A>();
	a->init();
	a->eventHandler.OnDataReady(nullptr, nullptr);
}

void testRpcserverregist() {
	// �������ӳ��
	std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
		{"subtrahend", Json::ValueType::intValue},
		{"minuend", Json::ValueType::intValue}
	};

	// ģ�� RPC ����
	Json::Value request;
	request["jsonrpc"] = "2.0";
	request["method"] = "subtract";
	request["params"]["subtrahend"] = 23;
	request["params"]["minuend"] = 42;

	auto subtractProcedure = std::make_shared<ProcedureReturn>(
		subtractCallback,
		paramsNameTypesMap
	);

	// ���ù���
	subtractProcedure->invoke(request, [](Json::Value response) {
		std::cout << "Response: " << response["result"].asInt() << std::endl;
		});
}

void testRpcserverregist2() {
	RpcService rpcService;
	std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
	  {"subtrahend", Json::ValueType::intValue},
	  {"minuend", Json::ValueType::intValue}
	};
	auto subtractProcedure = std::make_unique<ProcedureReturn>(
		subtractCallback,
		paramsNameTypesMap
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