#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_memorial_item.hpp"

using namespace stdA;

CmdMemorialItemInfo::CmdMemorialItemInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_typeid(0) {
}

CmdMemorialItemInfo::~CmdMemorialItemInfo() {
}

CmdMemorialItemInfo::CmdMemorialItemInfo(uint32_t _uid, uint32_t _typeID, bool _waiter) :
	pangya_db(_waiter), m_uid(_uid), m_typeid(_typeID) {
}



void CmdMemorialItemInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {
	checkColumnNumber((uint32_t)_result->cols, (uint32_t)_result->cols);

	ctx_coin c{ 0 };
	ctx_coin_item_ex ctx_bi{ 0 };
	auto& el = sIff::getInstance().getMemorialShopCoinItemInfo(m_typeid);

	// Box Item
	c._typeid = el._typeid;
	if (c._typeid!=0)
	{
		//moeda
		c.tipo = MEMORIAL_COIN_TYPE(el.type);
		c._typeid = el._typeid;
		c.probabilidade = el.probability;	
		//items
		ctx_bi._typeid = IFNULL(atoi, _result->data[0]);
		ctx_bi.probabilidade = IFNULL(atoi, _result->data[1]);
		 IFNULL(atoi, _result->data[2]);//moeda id
		ctx_bi.qntd = IFNULL(atoi, _result->data[3]);
		ctx_bi.gacha_number = IFNULL(atoi, _result->data[4]);
		ctx_bi.tipo = IFNULL(atoi, _result->data[5]);//tipo do item
		auto it = m_rares.find(m_typeid);
		if (it != m_rares.end()) {	// já tem essa coin
			// Add o Item a coin
			it->second.item.push_back(ctx_bi);
		}
		else {	// Ainda nao tem essa coin no map, add ela ao map


			// Add o Item a Box
			c.item.push_back(ctx_bi);

			// Add a Box ao map
			m_rares.insert(std::make_pair(m_typeid, c));
		}

	}
}

response* CmdMemorialItemInfo::prepareConsulta(database& _db) {
	
	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));
	checkResponse(r, "nao conseguiu pegar dados do memorial 2.0 !");
	return r;
}

ctx_coin& stdA::CmdMemorialItemInfo::getItems()
{	
	auto it = m_rares.begin();
	if (it != m_rares.end()) {
		return it->second;
	}
}
