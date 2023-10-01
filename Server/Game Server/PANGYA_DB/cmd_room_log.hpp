#pragma once
#ifndef _STDA_CMD_ROOM_LOG_HPP
#define _STDA_CMD_ROOM_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/room_log.hpp"

namespace stdA {
	class CmdRoomLog : public pangya_db {
	public:
		explicit CmdRoomLog(RoomLog _log,
		bool _waiter = false);
		virtual ~CmdRoomLog();

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;

		// get Class name
		virtual std::string _getName() override { return "CmdRoomLog"; };
		virtual std::wstring _wgetName() override { return L"CmdRoomLog"; };

	private:
		RoomLog m_log;

		const char* m_szConsulta = "pangya.ProcInsertRoomLog";
	};
}

#endif // !_STDA_CMD_FIRST_ANNIVERSARY_HPP