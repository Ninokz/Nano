#include "utils.h"
#include "Log.h"
#include "Env.h"
#include "config.h"

using namespace Nano::Log;
using namespace Nano::Utils;
using namespace Nano::Env;
using namespace Nano::Configuration;

int main() {
	auto envsig = EnvMgr::GetInstance();
	auto  configsig = Nano::Configuration::Config::GetInstance();
	configsig->LoadFromConfDir(envsig->GetConfigFolderPath());
	configsig->PrintAllConfigVars();

	return 0;
}