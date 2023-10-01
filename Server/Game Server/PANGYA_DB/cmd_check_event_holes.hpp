#pragma once
#ifndef _STDA_CMD_UPDATE_ROOM_LOG_HPP
#define _STDA_CMD_UPDATE_ROOM_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/room_log.hpp"

namespace stdA {
	class CmdCheckEventHoles : public pangya_db {
	public:
		explicit CmdCheckEventHoles(bool _waiter = false);
		virtual ~CmdCheckEventHoles();

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;
		// get Class name
		virtual std::string _getName() override { return "CmdCheckEventHoles"; };
		virtual std::wstring _wgetName() override { return L"CmdCheckEventHoles"; };

		const char* m_szConsulta = "pangya.ProcCheckEventHole";
	};
}

#endif // !_STDA_CMD_FIRST_ANNIVERSARY_HPP