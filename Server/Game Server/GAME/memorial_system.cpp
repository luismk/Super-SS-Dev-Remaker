// Arquivo memorial_system.cpp
// Criado em 21/07/2018 as 18:58 por Acrisio
// Implementa��o da classe MemorialSystem

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "memorial_system.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/iff.h"
#include "../UTIL/lottery.hpp"

#include <string>

#include <algorithm>

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PANGYA_DB/cmd_memorial_normal_item_info.hpp"
#include "../PANGYA_DB/cmd_memorial_level_info.hpp"

#include "../Game Server/game_server.h"
#include "../PANGYA_DB/cmd_memorial_item.hpp"
#include "../PANGYA_DB/cmd_add_memorial_coin.hpp"
#include "../PANGYA_DB/cmd_add_memorial_item.hpp"

#define CHECK_SESSION(_method) { \
	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 1, 0)); \
} \

#define CHECK_SESSION_AND_COIN(_method) { \
	CHECK_SESSION((_method)); \
	\
	if (!isLoad()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] Memorial System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 2, 0)); \
	\
	if (_ctx_c._typeid == 0) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] coin _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 3, 0)); \
	\
	if (_ctx_c.item.empty()) \
		throw exception("[MemorialSystem::" + std::string((_method)) + "][Error] coin is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 4, 0)); \
} \

#if defined(_WIN32)
#define TRY_CHECK			 try { \
								EnterCriticalSection(&m_cs);
#elif defined(__linux__)
#define TRY_CHECK			 try { \
								pthread_mutex_lock(&m_cs);
#endif

#if defined(_WIN32)
#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
#endif

#if defined(_WIN32)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								LeaveCriticalSection(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[MemorialSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#elif defined(__linux__)
#define CATCH_CHECK(_method) }catch (exception& e) { \
								pthread_mutex_unlock(&m_cs); \
								\
								_smp::message_pool::getInstance().push(new message("[MemorialSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

#define END_CHECK			 } \

using namespace stdA;
MemorialSystem::MemorialSystem() : m_load(false), m_level{}, m_consolo_premio{} {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Inicializa
	initialize();
}

MemorialSystem::~MemorialSystem() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void MemorialSystem::initialize() {

	TRY_CHECK;
	// Add os Itens Padr�es, para quando n�o ganha o rare item
	CmdAddMemorialCoin cmd_mnii2(true);	// Waiter

	NormalManagerDB::add(0, &cmd_mnii2, nullptr, nullptr);

	cmd_mnii2.waitEvent();

	if (cmd_mnii2.getException().getCodeError() != 0)
		throw cmd_mnii2.getException();


	// Add os Itens Padr�es, para quando n�o ganha o rare item
	CmdMemorialNormalItemInfo cmd_mnii(true);	// Waiter

	NormalManagerDB::add(0, &cmd_mnii, nullptr, nullptr);

	cmd_mnii.waitEvent();

	if (cmd_mnii.getException().getCodeError() != 0)
		throw cmd_mnii.getException();

	m_consolo_premio = cmd_mnii.getInfo();

	// Levels
	CmdMemorialLevelInfo cmd_mli(true);	// Waiter

	NormalManagerDB::add(0, &cmd_mli, nullptr, nullptr);

	cmd_mli.waitEvent();

	if (cmd_mli.getException().getCodeError() != 0)
		throw cmd_mli.getException();

	m_level = cmd_mli.getInfo();

	if (m_level.empty() || m_consolo_premio.empty())
	{
		_smp::message_pool::getInstance().push(new message("[MemorialSystem::initialize][Log] Memorial System nao Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
	}

	// Carregado com sucesso
	m_load = true;

	LEAVE_CHECK;

	CATCH_CHECK("initialize");

	// Relan�a para o server tomar as provid�ncias
	throw;

	END_CHECK;
}

void MemorialSystem::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_level.empty())
		m_level.clear();

	if (!m_consolo_premio.empty())
		m_consolo_premio.clear();

	m_load = false;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

uint32_t MemorialSystem::calculeMemorialLevel(uint32_t _achievement_pontos) {

	if (_achievement_pontos == 0)
		return 0u;	// Level 0

	auto level = ((_achievement_pontos - 1) / 300);

	return (level > MEMORIAL_LEVEL_MAX ? MEMORIAL_LEVEL_MAX : level);
}

bool MemorialSystem::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// + 1 no MEMORIAL_LEVEL_MAX por que � do 0 a 24, da 25 Levels
	isLoad = (m_load && (!m_level.empty() && m_level.size() == (MEMORIAL_LEVEL_MAX + 1)) && !m_consolo_premio.empty());

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
}

void MemorialSystem::load() {

	if (isLoad())
		clear();

	initialize();
}
ctx_coin* stdA::MemorialSystem::findCoin(uint32_t _typeid, uint32_t _uid)
{
	TRY_CHECK;

	// Carrega as Coin e os Itens
	CmdMemorialItemInfo cmd_mli(_uid, _typeid, true);	// Waiter

	NormalManagerDB::add(0, &cmd_mli, nullptr, nullptr);

	cmd_mli.waitEvent();

	if (cmd_mli.getException().getCodeError() != 0)
		throw cmd_mli.getException();

	LEAVE_CHECK;
	ctx_coin* result = new ctx_coin(cmd_mli.getItems());
	return result;
	CATCH_CHECK("findCoin");
	END_CHECK;

	return nullptr;
}
std::vector<ctx_coin_item_ex> MemorialSystem::drawCoin(player& _session, ctx_coin& _ctx_c) {
	CHECK_SESSION_AND_COIN("drawCoin");

	std::vector<ctx_coin_item_ex> v_item;

	TRY_CHECK;

	Lottery lottery(reinterpret_cast<uint64_t>(&_ctx_c));

	ctx_coin_item_ex* ci = nullptr;
	ctx_coin_set_item* csi = nullptr;
	// Calcula Memorial Level Pelos Achievement Pontos
	uint32_t level = calculeMemorialLevel(_session.m_pi.mgr_achievement.getPontos());
	// Initialize Rare Item e add à roleta
	for (auto& el : _ctx_c.item) {
		switch (_ctx_c.tipo) {
		case MCT_NORMAL:
			if (el.gacha_number < 0 || static_cast<uint32_t>(el.gacha_number) <= m_level[level].gacha_number) {
				lottery.push(el.probabilidade, reinterpret_cast<size_t>(&el));
			}
			break;
		case MCT_PREMIUM:
			if (el.gacha_number < 0 || static_cast<uint32_t>(el.gacha_number) <= m_level[MEMORIAL_LEVEL_MAX].gacha_number) {
				lottery.push(el.probabilidade, reinterpret_cast<size_t>(&el));
			}
			break;
		case MCT_CHARACTER:
		case MCT_SPECIAL:
			lottery.push(el.probabilidade, reinterpret_cast<size_t>(&el));
			break;
		default:
			throw exception("[MemorialSystem::drawCoin][Error] Memorial Coin[TYPE=" + std::to_string(_ctx_c.tipo) + "] desconhecido. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 6, 0));
		}
	}

	// Init Common Itens
	auto limit_prob = lottery.getLimitProbilidade();

	auto count_item = std::count_if(m_consolo_premio.begin(), m_consolo_premio.end(), [&](auto& el) {
		return el.second.tipo == (_ctx_c.tipo == MCT_PREMIUM ? 1 : 0);
		});

	// Cálculo da probabilidade baseada nos segundos conectados
	uint32_t secondsConnected = _session.getSecondsConnected();
	double probability = static_cast<double>(secondsConnected) / _session.MAX_SECONDS_CONNECTED;
	limit_prob = static_cast<uint64_t>(limit_prob * probability);

	if (count_item > 0)
		count_item = (uint32_t)limit_prob / count_item;

	// Add Common Itens à roleta
	for (auto& el : m_consolo_premio) {
		lottery.push(static_cast<uint32_t>(count_item) * 2, reinterpret_cast<size_t>(&el.second));
	}

	Lottery::LotteryCtx* lc = nullptr;
	uint32_t count = 1; // Quantidade de prêmios sorteados

	do {
		CHECK_SESSION("drawCoin");

		lc = lottery.spinRoleta(true); // Remove os itens que já foram sorteados

		if (lc == nullptr)
			throw exception("[MemorialSystem::drawCoin][Error] não conseguiu sortear um item. Erro na hora de rodar a roleta", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 5, 0));

		if (lc->value == 0u)
			throw exception("[MemorialSystem::drawCoin][Error] não conseguiu sortear um item. lc->value is invalid(0).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MEMORIAL_SYSTEM, 5, 1));

		// Verifica se é um SetItem ou Item
		auto test = *(int32_t*)lc->value;

		if (test == -100) { // SetItem
			csi = (ctx_coin_set_item*)lc->value;

			for (auto& el : csi->item) {
				// Continua se o jogador já possui esse item e não permite duplicatas
				if ((!sIff::getInstance().IsCanOverlapped(el._typeid) || sIff::getInstance().getItemGroupIdentify(el._typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(el._typeid))
					continue;

				v_item.push_back(el);
			}
		}
		else { // Item
			ci = (ctx_coin_item_ex*)lc->value;

			// Continua se o jogador já possui esse item e não permite duplicatas
			if ((!sIff::getInstance().IsCanOverlapped(ci->_typeid) || sIff::getInstance().getItemGroupIdentify(ci->_typeid) == iff::CAD_ITEM) && _session.m_pi.ownerItem(ci->_typeid))
				continue;

			v_item.push_back(*ci);
		}

		// Decrementa a quantidade, pois um item foi sorteado
		--count;
	} while (count > 0);

	LEAVE_CHECK;

	CATCH_CHECK("drawCoin");
	END_CHECK;

	return v_item;
}