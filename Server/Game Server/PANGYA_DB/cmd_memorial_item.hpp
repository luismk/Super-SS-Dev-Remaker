// Arquivo cmd_scratch_card_rate.hpp
// Criado em 29/06/2022 as 20:27 luisMK
// Defini��o da classe CmdMemorialItemInfo

#pragma once
#ifndef _STDA_CMD_MEMORIAL_ITEM_HPP
#define _STDA_CMD_MEMORIAL_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/UTIL/iff.h"
#include "../TYPE/memorial_type.hpp"

namespace stdA {
	class CmdMemorialItemInfo : public pangya_db {
	public:
		explicit CmdMemorialItemInfo(bool _waiter = false);
		virtual ~CmdMemorialItemInfo();
		CmdMemorialItemInfo(uint32_t _uid, uint32_t _typeID, bool _waiter = false);

	protected:
		void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
		response* prepareConsulta(database& _db) override;


		// get Class name
		virtual std::string _getName() override { return "CmdMemorialItemInfo"; };
		virtual std::wstring _wgetName() override { return L"CmdMemorialItemInfo"; };
	public:
		ctx_coin& getItems();

	private:
		uint32_t m_uid;
		uint32_t m_typeid;
		std::map<uint32_t, ctx_coin> m_rares;
			const char* m_szConsulta = "pangya.[ProcGetNewMemorialItem]";
	};
}

#endif // !_STDA_CMD_ADD_MEMORIAL_ITEM_HPP
