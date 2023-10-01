// Arquivo login_task.cpp
// Criado em 01/05/2018 as 17:58 por Acrisio
// Implementação da classe LoginTask

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "login_task.hpp"
#include "../PANGYA_DB/cmd_update_clubset_equiped.hpp"
#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_member_info.hpp"
#include "../PANGYA_DB/cmd_user_equip.hpp"

#include "../../Projeto IOCP/DATABASE/normal_db.hpp"
#include "login_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#include "login_reward_system.hpp"
#include "../PANGYA_DB/cmd_update_clubset_time.hpp"
using namespace stdA;

#define GET_GS(_gs_in, _gs_out, _proc_name) { \
	if ((_gs_in) == nullptr) \
		throw exception("[LoginTask::" + std::string((_proc_name)) + "][Error] _gs is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_TASK, 1, 0)); \
	(_gs_out) = reinterpret_cast<game_server*>(_gs_in); \
} \

LoginTask::LoginTask(player& _session, KeysOfLogin& _kol, player_info& _pi, ClientVersion& _cv, void* _gs) 
	: m_session(_session), m_kol(_kol), m_pi(_pi), m_cv(_cv), m_gs(_gs), m_count(0u), m_finish(false) {
}

LoginTask::~LoginTask() {

}

void LoginTask::exec() {

	//NormalManagerDB::add(0, new CmdPlayerInfo(m_pi.uid), LoginManager::SQLDBResponse, this);

	// Troquei a parte de verificação inicial para o game server
	NormalManagerDB::add(2, new CmdUserEquip(m_pi.uid), LoginManager::SQLDBResponse, this);
}

player& LoginTask::getSession() {
	return m_session;
}

KeysOfLogin& LoginTask::getKeysOfLofin() {
	return m_kol;
}

player_info& LoginTask::getInfo() {
	return m_pi;
}

ClientVersion& LoginTask::getClientVersion() {
	return m_cv;
}

void LoginTask::finishSessionInvalid() {

	// Terminou a Tarefa, muda o estádo do task para finish
	m_finish = true;
}

void LoginTask::sendFailLogin() {

	try {
	
		packet p((unsigned short)0x44);

		p.addUint8(1);
		p.addInt32(1);	// Fail Login

		packet_func::session_send(p, &m_session, 1);
		_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][SendFailLogin] ", CL_ONLY_CONSOLE));

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[LoginTask::sendFailLogin][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	// Terminou a Tarefa, muda o estádo do task para finish
	m_finish = true;
}

void LoginTask::sendCompleteData() {

	// Verifica se a session ainda é valida, essas funções já é thread-safe
	if (!m_session.isConnected() || !m_session.isCreated() || !m_session.getState()) {

		_smp::message_pool::getInstance().push(new message("[LoginTask::sendCompleteData][Error] session is invalid.", CL_FILE_LOG_AND_CONSOLE));

		finishSessionInvalid();

		return;
	}

	try {

		packet p;
		auto pi = &m_session.m_pi;
		game_server* gs = nullptr;

		// Check All Character All Item Equiped is on Warehouse Item of Player
		for (auto& el : m_session.m_pi.mp_ce) {

			// Check Parts of Character e Check Aux Part of Character
			m_session.checkCharacterAllItemEquiped(el.second);

		}

		// Check All Item Equiped
		m_session.checkAllItemEquiped(m_session.m_pi.ue);

		if (checkClubSetEquiped())
			NormalManagerDB::add(0, new CmdUpdateClubsetEquiped(m_session.m_pi.uid, m_session.m_pi.ue.clubset_id), nullptr, nullptr);

		GET_GS(m_gs, gs, "sendCompleteData");

		// Envia todos pacotes aqui, alguns envia antes, por que agora estou usando o jeito o pangya original
		packet_func::pacote044(p, &m_session, gs->getInfo(), 0, pi);
		packet_func::session_send(p, &m_session, 0);

		if (packet_func::pacote070(p, &m_session, pi->mp_ce))
			packet_func::session_send(p, &m_session, 0);
		if (packet_func::pacote071(p, &m_session, pi->mp_ci))
			packet_func::session_send(p, &m_session, 0);
		if (packet_func::pacote073(p, &m_session, pi->mp_wi))
			packet_func::session_send(p, &m_session, 0);
		packet_func::pacote0E1(p, &m_session, pi->mp_mi);
		packet_func::session_send(p, &m_session, 0);
		packet_func::pacote072(p, &m_session, pi->ue);
		packet_func::session_send(p, &m_session, 0);

		gs->sendChannelListToSession(m_session);

		// Treasure Hunter Info
		packet_func::pacote131(p);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote102(p, &m_session, pi);
		packet_func::session_send(p, &m_session, 0);		// Pacote novo do JP, passa os coupons do Gacha JP

		pi->mgr_achievement.sendCounterItemToPlayer(m_session);
		pi->mgr_achievement.sendAchievementToPlayer(m_session);

		CmdServerList cmd_sl(CmdServerList::MSN, true);	// waitable

		NormalManagerDB::add(0, &cmd_sl, nullptr, nullptr);

		cmd_sl.waitEvent();

		if (cmd_sl.getException().getCodeError() != 0)
			_smp::message_pool::getInstance().push(new message("[LoginTask::sendCompleteData][Line157] " + cmd_sl.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		auto& v_si = cmd_sl.getServerList();
		if (v_si.size() > 0)//requesita o messenger server aqui, mas fiz bloco para melhorar o codigo!
		{
			packet_func::pacote0F1(p, &m_session);
			packet_func::session_send(p, &m_session, 0);
			packet_func::pacote135(p, &m_session);
			packet_func::session_send(p, &m_session, 0);
		}

		packet_func::pacote144(p, &m_session);
		packet_func::session_send(p, &m_session, 0);		// Pacote novo do JP

		packet_func::pacote138(p, &m_session, pi->v_card_info);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote136(p, &m_session);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote137(p, &m_session, pi->v_cei);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote13F(p, &m_session);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote181(p, &m_session, pi->v_ib);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote096(p, &m_session, pi);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote169(p, &m_session, pi->ti_current_season, 5/*season atual*/);
		packet_func::session_send(p, &m_session, 0);
		packet_func::pacote169(p, &m_session, pi->ti_rest_season);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote0B4(p, &m_session, pi->v_tsi_current_season, 5/*season atual*/);
		packet_func::session_send(p, &m_session, 0);
		packet_func::pacote0B4(p, &m_session, pi->v_tsi_rest_season);
		packet_func::session_send(p, &m_session, 0);

		packet_func::pacote158(p, &m_session, pi->uid, pi->ui, 0);
		packet_func::session_send(p, &m_session, 0);	// Total de season, 5 atual season

		if (packet_func::pacote25D(p, &m_session, pi->v_tgp_current_season, 5/*season atual*/))
			packet_func::session_send(p, &m_session, 0);
		if (packet_func::pacote25D(p, &m_session, pi->v_tgp_rest_season, 0))
			packet_func::session_send(p, &m_session, 0);

		//copiado do acrisio ss
		packet_func::pacote1B1(p, &m_session, 0);
		packet_func::session_send(p, &m_session, 1);

		// Login Reward System - verifica se o player ganhou algum item por logar
		if (sgs::gs::getInstance().getInfo().rate.login_reward_event)
			sLoginRewardSystem::getInstance().checkRewardLoginAndSend(m_session);
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[LoginTask::sendCompleteData][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	// Terminou a Tarefa, muda o estádo do task para finish
	m_finish = true;
}

void LoginTask::sendReply(uint32_t _msg_id) {

	packet p((unsigned short)0x44);

	p.addUint8(0xD2);
	p.addInt32(_msg_id);

	packet_func::session_send(p, &m_session, 1);
}



bool LoginTask::checkClubSetEquiped() {
	bool upt_on_db = false;
	auto _ue = m_session.m_pi.ue;
	auto m_pi = m_session.m_pi;
	int32_t tmp_id = _ue.clubset_id;
	if (_ue.clubset_id != 0) {

		auto atual = m_pi.findWarehouseItemItById(_ue.clubset_id);
		auto clubset = sIff::getInstance().findClubSet(atual->second._typeid);
		auto pWi = atual->second;

		if (clubset != NULL && ((pWi.flag & 0x20) || (pWi.flag & 0x40) || (pWi.flag & 0x60)) && pWi.end_date_unix_local == 0)//verifica se é taco de temporario
		{
			if (pWi.apply_date_unix_local == 0u) {

				// Não tem add o ClubSet padrão para ele(CV1)
				NormalManagerDB::add(0, new CmdUpdateClubSetTime(m_pi.uid, pWi), nullptr, nullptr);
				
				try {

					// Equipa ClubSet Padrão
					m_session.equipDefaultClubSet(_ue);
					m_session.m_pi.deleteWarehouseItemItById(tmp_id);
				}
				catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[LoginTask::checkClubSetEquiped][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				upt_on_db = true;
			}
		}
	}

	return upt_on_db;
}
uint32_t LoginTask::getCount() {
	return m_count;
}

uint32_t LoginTask::incremenetCount() {
	return ++m_count;
}

bool LoginTask::isFinished() {
	return m_finish;
}
