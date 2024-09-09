#include "utils.h"
#include "Log.h"
#include "Env.h"
#include "config.h"
#include "jrpcproto.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::Env;
using namespace Nano::Configuration;
using namespace Nano::JrpcProto;

int main() {
    std::string request = JrpcRequestGenerator::generate("subtract", 4, "minuend", 42);
    std::cout << request << std::endl;


    return 0;
}