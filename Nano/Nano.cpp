#include "utils.h"
#include "Log.h"
#include "Env.h"    

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::Env;

int main(int argc, char** argv) {
	auto sig = EnvMgr::GetInstance();
	auto rootPath = sig->GetRootPath();
	std::cout << rootPath << std::endl;
    return 0;
}