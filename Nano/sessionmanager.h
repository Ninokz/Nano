#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>

#include "ceventhandler.h"
#include "session.h"
#include "Log.h"

namespace Nano {
	namespace Communication {
		class SessionManager : public Noncopyable, public ICloseEventHandler, public IConnectEventHandler
		{
		public:
			typedef std::shared_ptr<SessionManager> Ptr;

			SessionManager(size_t maxSessions) : m_MAX_SESSIONS(maxSessions) {}
			virtual ~SessionManager() {}

			virtual void OnClosed(std::shared_ptr<Session> sender) override;
			virtual void OnConnected(std::shared_ptr<Session> sender) override;
		private:
			inline bool addSession(std::shared_ptr<Session> session);
			inline void removeSession(std::string& uid);

		private:
			const size_t m_MAX_SESSIONS;
			std::unordered_map<std::string, std::shared_ptr<Session>> m_sessions;
		};
	}
}