// Arquivo cmd_scratch_card_rate.cpp
// Implementa��o da classe CmdAddMemorialCoin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_memorial_coin.hpp"
#include "../TYPE/memorial_type.hpp"

using namespace stdA;

CmdAddMemorialCoin::CmdAddMemorialCoin(bool _waiter) : pangya_db(_waiter) {
}

CmdAddMemorialCoin::~CmdAddMemorialCoin() {
}


void CmdAddMemorialCoin::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {
	//faz nada 
}

response* CmdAddMemorialCoin::prepareConsulta(database& _db) {

	auto rares = sIff::getInstance().getMemorialShopRareItem();
	auto coins = sIff::getInstance().getMemorialShopCoinItem();
	response* r = nullptr;
	ctx_coin c{ 0 };
	ctx_coin_item_ex ci{ 0 };
	std::map< uint32_t, ctx_coin > m_coin;
	for (auto& el : coins) {
		c.clear();

		c.tipo = MEMORIAL_COIN_TYPE(el.second.type);
		c._typeid = el.second._typeid;
		c.probabilidade = el.second.probability;

		for (auto& el2 : rares) {

			if (!el.second.gacha_range.empty() && !el.second.gacha_range.isBetweenGacha(el2.gacha.number))
				continue;

			if (el.second.emptyFilter()) {
				ci.clear();

				ci.tipo = (MEMORIAL_RARE_TYPE)el2.rare_type;
				ci._typeid = el2._typeid;
				ci.probabilidade = el2.probability;
				ci.gacha_number = el2.gacha.number;
				ci.qntd = 1;

				c.item.push_back(ci);
			}
			else {
				for (auto i = 0u; i < (sizeof(el2.filter) / sizeof(el2.filter[0])); ++i) {

					if (el.second.hasFilter(el2.filter[i])) {
						ci.clear();

						ci.tipo = (MEMORIAL_RARE_TYPE)el2.rare_type;
						ci._typeid = el2._typeid;
						ci.probabilidade = el2.probability;
						ci.gacha_number = el2.gacha.number;
						ci.qntd = 1;

						c.item.push_back(ci);

						break;	// Sai do Loop de Filters
					}
				}	// Fim do loop de Filters
			}
		}	// Fim do loop de Rare Item

		// Add Coin ao Map
		auto it = m_coin.find(c._typeid);

		if (it == m_coin.end())	// Add Coin ao map
		{
			m_coin[c._typeid] = c;
		}

	}	// Fim do loop de Coin Item

	for (auto& el : m_coin) {
		auto item = el.second;
		
		r = procedure(_db, m_szConsulta, std::to_string(item.tipo) + ", " + std::to_string(el.first) + ", " +
			std::to_string(item.probabilidade));

		checkResponse(r, "nao conseguiu inserir dados do memorial 2.0 !");
	}	// Fim do loop de Coin Item

	if (r == nullptr)
	{
		checkResponse(r, "nao conseguiu inserir dados do memorial 2.0 !");
	}
	return r;
}
