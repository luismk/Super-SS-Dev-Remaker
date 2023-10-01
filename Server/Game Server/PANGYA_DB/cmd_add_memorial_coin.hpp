// Arquivo cmd_scratch_card_rate.hpp
// Criado em 29/06/2022 as 20:27 luisMK
// Defini��o da classe CmdAddMemorialCoin

#pragma once
#ifndef _STDA_CMD_ADD_MEMORIAL_COIN_HPP
#define _STDA_CMD_ADD_MEMORIAL_COIN_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/UTIL/iff.h"

namespace stdA {
	class CmdAddMemorialCoin : public pangya_db {
	public:
		explicit CmdAddMemorialCoin(bool _waiter = false);
		virtual ~CmdAddMemorialCoin();

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;

		// get Class name
		virtual std::string _getName() override { return "CmdAddMemorialCoin"; };
		virtual std::wstring _wgetName() override { return L"CmdAddMemorialCoin"; };

	private:
		const char* m_szConsulta = "pangya.ProcAddMemorialCoin";
	};
}

#endif // !_STDA_CMD_ADD_MEMORIAL_COIN_HPP
