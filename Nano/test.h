#pragma once
#include "BaseServer.h"
#include "RpcService.h"
#include "RpcProcedure.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::JrpcProto;
using namespace Nano::Communication;
using namespace Nano::Rpc;

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
    helloProcedure->invoke(request, [](Json::Value response) {
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
    subtractProcedure->invoke(request, [](Json::Value response) {
        std::cout << "Response: " << response["result"].asInt() << std::endl;
        });
}

void sub()
{
    const char* jsonStr = "{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":{\"subtrahend\":23,\"minuend\":42},\"id\":\"132\"}";
    bool flag = false;
    JsonRpcRequest::Ptr request = JrpcRequestGenerator::generate(jsonStr, &flag);

}
