#pragma once
#ifndef _STDA_CMD_UPDATE_ROOM_LOG_HPP
#define _STDA_CMD_UPDATE_ROOM_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/room_log.hpp"

namespace stdA {
	class CmdUpdateRoomLog : public pangya_db {
	public:
		explicit CmdUpdateRoomLog(RoomLog _log,
		bool _waiter = false);
		virtual ~CmdUpdateRoomLog();

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;
		// get Class name
		virtual std::string _getName() override { return "CmdUpdateRoomLog"; };
		virtual std::wstring _wgetName() override { return L"CmdUpdateRoomLog"; };

	private:
		RoomLog m_log;

		const char* m_szConsulta = "pangya.ProcUpdateLogHole";
	};
}

#endif // !_STDA_CMD_FIRST_ANNIVERSARY_HPP