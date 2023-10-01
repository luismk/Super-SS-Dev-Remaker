#pragma once
#ifndef _STDA_EVENT_SYSTEM_HPP
#define _STDA_EVENT_SYSTEM_HPP

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "../TYPE/event_type.hpp"
#include "../SESSION/player.hpp"

#include <map>
#include <vector>

namespace stdA {
	class EventSystem {
		public:
			EventSystem();
			virtual ~EventSystem();

			/*static*/ void load();

			/*static*/ bool isLoad();

			/*static*/ bool IsEventHole();


			/*static*/ void GetPrize(uint32_t _typeid, uint32_t holes);

			/*static*/ void GetPrize(uint32_t _typeid);

		protected:
			/*static*/ void initialize();

			/*static*/ void clear();

		private:
			/*static*/ std::map< EventType, GameEvent > m_event;

			/*static*/ bool m_load;

			/*static*/ int TypeEvent();

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< EventSystem > sEventSystem;
}

#endif // !_STDA_BOX_SYSTEM_HPP
