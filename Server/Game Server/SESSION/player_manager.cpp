// Arquivo player_manager.cpp
// Criado em 07/03/2018 as 22:17 por Acrisio
// Implemenatação da classe player_manager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "player_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include "../PACKET/packet_func_sv.h"

#include "../PANGYA_DB/cmd_update_caddie_info.hpp"
#include "../PANGYA_DB/cmd_update_character_all_part_equiped.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../GAME/premium_system.hpp"

#include "../GAME/item_manager.h"

#define CHECK_PLAYER(_p)	if (!(_p).getState()) \
								return; \
							if (!(_p).isConnected()) \
									return; \

using namespace stdA;

player_manager::player_manager(threadpool& _threadpool, uint32_t _max_session) : session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0u) {
		// Reserva na memória o tamanho de todos os players que pode ter o server
		m_sessions.reserve(m_max_session);

		for (auto i = 0u; i < m_max_session; ++i)
			m_sessions.push_back(new player(*(threadpool_base*)&m_threadpool));
	}
};

player_manager::~player_manager() {
	clear();
};

void player_manager::clear() {

	session_manager::clear();

	if (!m_indexes.empty())
		m_indexes.clear();
};

player* player_manager::findPlayer(uint32_t _uid, bool _oid) {

	player* _player = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
		if (
#if defined(_WIN32)
			el->m_sock != INVALID_SOCKET
#elif defined(__linux__)
			el->m_sock.fd != INVALID_SOCKET
#endif
			&& ((!_oid) ? el->getUID() : el->m_oid) == _uid) {
			_player = (player*)el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _player;
};

player* player_manager::findPlayerBySocketId(uint32_t _socket_id) {

	player* _player = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
		if (
#if defined(_WIN32)
			el->m_sock != INVALID_SOCKET && el->m_sock == _socket_id
#elif defined(__linux__)
			el->m_sock.fd != INVALID_SOCKET && el->m_sock.fd == _socket_id
#endif
			) {
			_player = (player*)el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _player;
};

std::vector< player* > player_manager::findAllGM() {

	std::vector< player* > v_gm;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions)
		if (
#if defined(_WIN32)
			el->m_sock != INVALID_SOCKET
#elif defined(__linux__)
			el->m_sock.fd != INVALID_SOCKET
#endif
			&& ((el->getCapability() & 4) || el->getCapability() & 128/*GM Player Normal*/))	// GM
			v_gm.push_back((player*)el);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_gm;
};

//bool player_manager::deleteSession(session* _session) {
//
//	if (_session == nullptr)
//		throw exception("[player_manager::deleteSession][ERR_SESSION] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MANAGER, 1, 0));
//
//	if (!_session->getState() &&
//#if defined(_WIN32)
//		_session->m_sock == INVALID_SOCKET
//#elif defined(__linux__)
//		_session->m_sock.fd == INVALID_SOCKET
//#endif
//		)
//		throw exception(std::string("[player_manager::deleteSession][ERR_SESSION] SESSION[IP=") + _session->getIP() + ", UID="
//			+ std::to_string(((player*)_session)->m_pi.uid) + ", OID=" + std::to_string(_session->m_oid)
//			+ "] _seession not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MANAGER, 2, 0));
//
//	bool ret = true;
//
//	// Block Session's
//#if defined(_WIN32)
//	EnterCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_lock(&m_cs);
//#endif
//
//	if (!_session->getState() &&
//#if defined(_WIN32)
//		_session->m_sock == INVALID_SOCKET
//#elif defined(__linux__)
//		_session->m_sock.fd == INVALID_SOCKET
//#endif
//		) {
//
//		// Se não liberar aqui da deadlock
//#if defined(_WIN32)
//		LeaveCriticalSection(&m_cs);
//#elif defined(__linux__)
//		pthread_mutex_unlock(&m_cs);
//#endif
//
//		throw exception(std::string("[player_manager::deleteSession][ERR_SESSION] SESSION[IP=") + _session->getIP() + ", UID="
//			+ std::to_string(((player*)_session)->m_pi.uid) + ", OID=" + std::to_string(_session->m_oid)
//			+ "] _seession not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_MANAGER, 2, 0));
//	}
//
//	// Block Session
//	_session->lock();
//
//	uint32_t tmp_oid = _session->m_oid;
//
//	if ((ret = _session->clear())) {
//		// Libera OID
//		freeOID(tmp_oid/*_session->m_oid*/);
//	}
//	// Unblock Session
//	_session->unlock();
//	// Unblock Session's
//#if defined(_WIN32)
//	LeaveCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_unlock(&m_cs);
//#endif
//
//	return ret;
//	};

void player_manager::checkPlayersItens() {

	try {

		// !@ WARNING tem que ter o thread safe aqui, pode testar um player, e ele não está online mais

		for (auto& s : m_sessions) {

			if (s->isCreated()) {

				// Item Buff
				checkItemBuff(*(player*)s);

				// Card Equipped Special
				checkCardSpecial(*(player*)s);

				// Caddie
				checkCaddie(*(player*)s);

				// Mascot
				checkMascot(*(player*)s);

				// Warehouse
				checkWarehouse(*(player*)s);
			}
		}

	}
	catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[player_manager::checkPlayersItens][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void player_manager::checkPlayersGameGuard() {

	try {

		// !@ WARNING tem que ter o thread safe aqui, pode testar um player, e ele não está online mais
		//EnterCriticalSection(&m_cs);

		for (auto& s : m_sessions) {

			if (s->isCreated() && s->isConnected() && checkGameGuard(*(player*)s)) // derruba player
#if defined(_WIN32)
				shutdown(s->m_sock, SD_RECEIVE); // derruba o player
#elif defined(__linux__)
				shutdown(s->m_sock.fd, SD_RECEIVE); // derruba o player
#endif
		}

		//LeaveCriticalSection(&m_cs);

	}
	catch (exception& e) {

		// libera
		//LeaveCriticalSection(&m_cs);

		_smp::message_pool::getInstance().push(new message("[player_manager::checkPlayersGameGuard][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void player_manager::blockOID(uint32_t _oid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = m_indexes.find(_oid);

	if (it != m_indexes.end())
		it->second.flag.block = 1;	// Block

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

void player_manager::unblockOID(uint32_t _oid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = m_indexes.find(_oid);

	if (it != m_indexes.end())
		it->second.flag.block = 0; // Unblock

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

inline void player_manager::checkItemBuff(player& _session) {
	CHECK_PLAYER(_session);

	// Item Buff
	for (auto it = _session.m_pi.v_ib.begin(); it != _session.m_pi.v_ib.end(); /*Incrementa em baixo*/) {

		// Acabou o tempo
		if (getLocalTimeDiffDESC(it->end_date) <= 0ll) {


			// Delete Item Buff from player vector
			it = _session.m_pi.v_ib.erase(it);

		}
		else
			it++;
	}
};

inline void player_manager::checkCardSpecial(player& _session) {
	CHECK_PLAYER(_session);

	// Card Equipped Special
	for (auto it = _session.m_pi.v_cei.begin(); it != _session.m_pi.v_cei.end(); /*Incrementa em baixo*/) {

		// Acabou o tempo
		if (it->tipo == IFF::Card::CARD_SUB_TYPE::T_SPECIAL && getLocalTimeDiffDESC(it->end_date) <= 0ll) {


			// Delete Item Buff from player vector
			it = _session.m_pi.v_cei.erase(it);

		}
		else
			it++;
	}
};

inline void player_manager::checkCaddie(player& _session) {
	CHECK_PLAYER(_session);

	// Caddie
	for (auto& el : _session.m_pi.mp_ci) {

		// Caddie por tempo
		if (el.second.rent_flag == 2 && getLocalTimeDiffDESC(el.second.end_date) <= 0ll) {

			// Put Update Item on vector update item of player
			if (_session.m_pi.findUpdateItemByIdAndType(el.second.id, UpdateItem::CADDIE) == _session.m_pi.mp_ui.end()) {

				_session.m_pi.mp_ui.insert(std::make_pair(PlayerInfo::stIdentifyKey(el.second._typeid, el.second.id), UpdateItem{ UpdateItem::CADDIE, el.second._typeid, el.second.id }));

				// Verifica se o Caddie está equipado e desequipa
				if ((_session.m_pi.ei.cad_info != nullptr && _session.m_pi.ei.cad_info->id == el.second.id) || _session.m_pi.ue.caddie_id == el.second.id) {

					_session.m_pi.ei.cad_info = nullptr;
					_session.m_pi.ue.caddie_id = 0;
				}
			}
		}

		// Parts Caddie End Date
		if (el.second.parts_typeid != 0u && !isEmpty(el.second.end_parts_date) && getLocalTimeDiffDESC(el.second.end_parts_date) <= 0ll) {

			// Put Update Item on vector update item of player
			if (_session.m_pi.findUpdateItemByIdAndType(el.second.id, UpdateItem::CADDIE_PARTS) == _session.m_pi.mp_ui.end()) {

				_session.m_pi.mp_ui.insert(std::make_pair(PlayerInfo::stIdentifyKey(el.second._typeid, el.second.id), UpdateItem{ UpdateItem::CADDIE_PARTS, el.second._typeid, el.second.id }));

				el.second.parts_typeid = 0u;
				el.second.parts_end_date_unix = 0u;
				el.second.end_parts_date = { 0 };
				NormalManagerDB::add(1, new CmdUpdateCaddieInfo(_session.m_pi.uid, el.second), player_manager::SQLDBResponse, nullptr);
			}
		}
	}
};

inline void player_manager::checkMascot(player& _session) {
	CHECK_PLAYER(_session);

	// Mascot
	for (auto& el : _session.m_pi.mp_mi) {

		// Mascot por Tempo
		if (el.second.tipo == 1 && getLocalTimeDiffDESC(el.second.data) <= 0ll) {

			// Put Update Item on vector update item of player
			if (_session.m_pi.findUpdateItemByIdAndType(el.second.id, UpdateItem::MASCOT) == _session.m_pi.mp_ui.end()) {

				_session.m_pi.mp_ui.insert(std::make_pair(PlayerInfo::stIdentifyKey(el.second._typeid, el.second.id), UpdateItem{ UpdateItem::MASCOT, el.second._typeid, el.second.id }));

				// Log
				
				// Verifica se o Mascot está equipado e desequipa
				if ((_session.m_pi.ei.mascot_info != nullptr && _session.m_pi.ei.mascot_info->id == el.second.id) || _session.m_pi.ue.mascot_id == el.second.id) {

					_session.m_pi.ei.mascot_info = nullptr;
					_session.m_pi.ue.mascot_id = 0;
				}
			}
		}
	}
};

inline void player_manager::checkWarehouse(player& _session) {
	// Warehouse
	for (auto& el : _session.m_pi.mp_wi) {

		// Item Por tempo
		if (((el.second.flag & 0x20) || (el.second.flag & 0x40) || (el.second.flag & 0x60))
			&& el.second.end_date_unix_local > 0) {

			auto st = UnixToSystemTime(el.second.end_date_unix_local);

			if (getLocalTimeDiffDESC(st) <= 0ll) {

				// Put Update Item on vector update item of player
				if (_session.m_pi.findUpdateItemByIdAndType(el.second.id, UpdateItem::WAREHOUSE) == _session.m_pi.mp_ui.end()) {

					_session.m_pi.mp_ui.insert(std::make_pair(PlayerInfo::stIdentifyKey(el.second._typeid, el.second.id), UpdateItem{ UpdateItem::WAREHOUSE, (uint32_t)el.second._typeid, el.second.id }));

					// Verifica se o item é um PART e se ele está equipado e deseequipa ele
					if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::PART && _session.m_pi.isPartEquiped(el.second._typeid, el.second.id)) {

						auto ci = _session.m_pi.findCharacterByTypeid((iff::CHARACTER << 26) | sIff::getInstance().getItemCharIdentify(el.second._typeid));

						if (ci != nullptr) {

							auto part = sIff::getInstance().findPart(el.second._typeid);

							if (part != nullptr) {

								// Deseequipa o Part do character e coloca os Parts Default do Character no lugar
								ci->unequipPart(part);

							}
							else {

								for (auto i = 0u; i < (sizeof(ci->parts_typeid) / sizeof(ci->parts_typeid[0])); ++i) {

									if (ci->parts_id[i] == el.second.id && ci->parts_typeid[i] == el.second._typeid) {
										ci->parts_typeid[i] = 0;
										ci->parts_id[i] = 0;
									}
								}

							}

							// Update no DB
							NormalManagerDB::add(2, new CmdUpdateCharacterAllPartEquiped(_session.m_pi.uid, *ci), player_manager::SQLDBResponse, nullptr);

						}
						else
							_smp::message_pool::getInstance().push(new message("[player_manager::checkWarehouse][Error][WARNING] player[UID=" + std::to_string(_session.m_pi.uid)
								+ "] nao tem o Character[TYPEID=" + std::to_string((iff::CHARACTER << 26) | sIff::getInstance().getItemCharIdentify(el.second._typeid)) + "]. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

					}

					// Verifica se é ClubSet e desequipa ele, e coloca o CV
					if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::CLUBSET && _session.m_pi.ei.clubset != nullptr
						&& _session.m_pi.ei.clubset->id == el.second.id || _session.m_pi.ue.clubset_id == el.second.id) {

						auto it = _session.m_pi.findWarehouseItemItByTypeid(AIR_KNIGHT_SET);

						if (it != _session.m_pi.mp_wi.end()) {

							_session.m_pi.ei.clubset = &it->second;
							_session.m_pi.ue.clubset_id = it->second.id;

							// Atualiza o ClubSet Enchant no Equiped Item do Player
							_session.m_pi.ei.csi = { it->second.id, it->second._typeid, it->second.c };

							IFF::ClubSet* cs = sIff::getInstance().findClubSet(it->second._typeid);

							if (cs != nullptr)
								for (auto i = 0u; i < (sizeof(_session.m_pi.ei.csi.enchant_c) / sizeof(_session.m_pi.ei.csi.enchant_c[0])); ++i)
									_session.m_pi.ei.csi.enchant_c[i] = cs->slot[i] + it->second.clubset_workshop.c[i];
						}

					}

					// Verifica se é um Comet(Ball) e desequipa ele, e coloca a bola padrão
					if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::BALL && _session.m_pi.ei.comet != nullptr
						&& _session.m_pi.ei.comet->id == el.second.id || _session.m_pi.ue.ball_typeid == el.second._typeid) {

						auto it = _session.m_pi.findWarehouseItemItByTypeid(DEFAULT_COMET_TYPEID);

						if (it != _session.m_pi.mp_wi.end()) {

							_session.m_pi.ei.comet = &it->second;
							_session.m_pi.ue.ball_typeid = DEFAULT_COMET_TYPEID;
						}

						// Verifica se é SKIN, para desequipar ele
						if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::SKIN) {

							for (auto i = 0u; i < (sizeof(_session.m_pi.ue.skin_typeid) / sizeof(_session.m_pi.ue.skin_typeid[0])); ++i) {

								if (_session.m_pi.ue.skin_typeid[i] == el.second._typeid && _session.m_pi.ue.skin_id[i] == el.second.id) {

									_session.m_pi.ue.skin_id[i] = 0;
									_session.m_pi.ue.skin_typeid[i] = 0;
									break;
								}
							}
						}

						// Verifica se é o Premium Ticket
						if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::ITEM && sPremiumSystem::getInstance().isPremiumTicket(el.second._typeid)) {

							sPremiumSystem::getInstance().removePremiumUser(_session);
						}
					}
				}
			}
		}
	};
}
bool player_manager::checkGameGuard(player& _session) {

	if (_session.getState()) {

#if INTPTR_MAX == INT64_MAX
		auto now = std::time(nullptr);

		if (!_session.m_gg.m_auth_reply)
			return true;	// Desconecta o player, ele não respondeu
		else if (_session.m_gg.m_auth_time < now) {

			DWORD err = _session.m_gg.m_csa.GetAuthQuery();

			if (err != ERROR_SUCCESS) {
				// Error, ao pegar GetAuthQuery
				return true;
			}
		}
#elif INTPTR_MAX == INT32_MAX
		auto now = time(nullptr);

		if (!_session.m_gg.m_auth_reply)
			return true;	// Desconecta o player ele não respondeu
		else if (_session.m_gg.m_auth_time < now) {

			DWORD err = _session.m_gg.m_csa.GetAuthQuery();

			if (err == ERROR_SUCCESS) {
				_session.m_gg.m_auth_time = now + (sRandomGen::getInstance().rIbeMt19937_64_chronoRange(0, AUTH_PERIOD_VARIATION_SEC) + AUTH_PERIOD_SEC);

				// Game Guard Auth
				packet p((unsigned short)0xD7);

				p.addBuffer(&_session.m_gg.m_csa.m_AuthQuery, sizeof(_GG_AUTH_DATA));

				packet_func::session_send(p, &_session, 1);

		}
			else {

				// Error, ao pegar GetAuthQuery
				return true;
			}
		}
#else
#error Unknown pointer size or missing size macros!
#endif
	}

	return false;
};

void player_manager::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[player_manager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	//auto pm = reinterpret_cast< player_manager* >(_arg);

	switch (_msg_id) {
	case 1:	// Update Caddie Info
	{
		auto cmd_uci = reinterpret_cast<CmdUpdateCaddieInfo*>(&_pangya_db);
		break;
	}
	case 2: // Update All parts of Character
	{
		break;
	}
	case 0:
	default:
		break;
	}
};

int32_t player_manager::findSessionFree() {

	for (auto i = 0u; i < m_sessions.size(); ++i)
		if (
#if defined(_WIN32)
			m_sessions[i]->m_sock == INVALID_SOCKET
#elif defined(__linux__)
			m_sessions[i]->m_sock.fd == INVALID_SOCKET
#endif
			)
			return (int)getNewOID();

	return -1;
};

uint32_t player_manager::getNewOID() {

	uint32_t oid = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// Find a index OID FREE
	auto it = std::find_if(m_indexes.begin(), m_indexes.end(), [](auto& el) {
		return el.second.ucFlag == 0;
		});

	if (it != m_indexes.end()) {	// Achei 1 index desocupado

		it->second.flag.busy = 1; // BUSY OCUPDADO

		oid = it->first;

	}
	else {	// Add um novo index no mapa de oid

		oid = (uint32_t)m_indexes.size();

		m_indexes[oid] = { 1/*BUSY OCUPDADO*/ };
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return oid;
	};

void player_manager::freeOID(uint32_t _oid) {

	auto it = m_indexes.find(_oid);

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (it != m_indexes.end()) {

		it->second.flag.busy = 0; // WAITING DESOCUPADO LIVRE

		if (it->second.flag.block)
			_smp::message_pool::getInstance().push(new message("[player_manager::freeOID][WARNING] index[OID=" + std::to_string(it->first) + "] esta bloqueado, nao pode liberar ele agora", CL_FILE_LOG_AND_CONSOLE));
	}
	else
		_smp::message_pool::getInstance().push(new message("[player_manager::freeOID][WARNING] index[OID=" + std::to_string(_oid) + "] nao esta no mapa.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};
