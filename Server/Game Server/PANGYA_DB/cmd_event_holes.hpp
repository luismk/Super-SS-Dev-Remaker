#pragma once
#ifndef _STDA_CMD_UPDATE_ROOM_LOG_HPP
#define _STDA_CMD_UPDATE_ROOM_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/room_log.hpp"

namespace stdA {
	class CmdEventHoles : public pangya_db {
	public:
		explicit CmdEventHoles(uint32_t _uid, uint32_t _holes, bool _waiter = false);
		virtual ~CmdEventHoles();

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;
		// get Class name
		virtual std::string _getName() override { return "CmdEventHoles"; };
		virtual std::wstring _wgetName() override { return L"CmdEventHoles"; };

	private:
		uint32_t m_uid;
		uint32_t m_holes;

		const char* m_szConsulta = "pangya.ProcUpdateEventHole";
	};
}

#endif // !_STDA_CMD_FIRST_ANNIVERSARY_HPP