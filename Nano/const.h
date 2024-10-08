#pragma once

namespace Nano {
	namespace Const {
		const int PACKET_HEAD_SIZE = 4;
		const int PACKET_MAX_SIZE = 1024;

		const int MAX_SESSION_SEND_QUEUE_PENDING_SIZE = 64;
		const int MAS_SESSION = 64;

		const int THREAD_POOL_SIZE = 4;

		const int STEAL_THREAD_POOL_FAIL_COUNT_LIMIT = 10;
		const int STEAL_THREAD_POOL_MAX_BACKOFF_TIME = 200;
	}
}