// Arquivo login_manager.cpp
// Criado em 01/05/2018 as 17:44 por Acrisio
// Implementação da classe LoginManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "login_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../../Projeto IOCP/UTIL/reader_ini.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_server_list.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_login_info.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_auth_key_game.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_update_auth_key_login.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_chat_macro_user.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_register_logon.hpp"

#include "../PANGYA_DB/cmd_player_info.hpp"
#include "../PANGYA_DB/cmd_member_info.hpp"
#include "../PANGYA_DB/cmd_user_info.hpp"
#include "../PANGYA_DB/cmd_trofel_info.hpp"
#include "../PANGYA_DB/cmd_user_equip.hpp"
#include "../PANGYA_DB/cmd_map_statistics.hpp"
#include "../PANGYA_DB/cmd_character_info.hpp"
#include "../PANGYA_DB/cmd_caddie_info.hpp"
#include "../PANGYA_DB/cmd_mascot_info.hpp"
#include "../PANGYA_DB/cmd_warehouse_item.hpp"
#include "../PANGYA_DB/cmd_tutorial_info.hpp"
#include "../PANGYA_DB/cmd_cookie.hpp"
#include "../PANGYA_DB/cmd_coupon_gacha.hpp"
#include "../PANGYA_DB/cmd_achievement_info.hpp"
#include "../PANGYA_DB/cmd_card_info.hpp"
#include "../PANGYA_DB/cmd_card_equip_info.hpp"
#include "../PANGYA_DB/cmd_item_buff_info.hpp"
#include "../PANGYA_DB/cmd_trophy_special.hpp"
#include "../PANGYA_DB/cmd_my_room_config.hpp"
#include "../PANGYA_DB/cmd_my_room_item.hpp"
#include "../PANGYA_DB/cmd_dolfini_locker_info.hpp"
#include "../PANGYA_DB/cmd_mail_box_info.hpp"
#include "../PANGYA_DB/cmd_mail_box_info2.hpp"
#include "../PANGYA_DB/cmd_email_info.hpp"
#include "../PANGYA_DB/cmd_delete_email.hpp"
#include "../PANGYA_DB/cmd_item_left_from_email.hpp"
#include "../PANGYA_DB/cmd_msg_off_info.hpp"
#include "../PANGYA_DB/cmd_attendance_reward_info.hpp"
#include "../PANGYA_DB/cmd_last_player_game_info.hpp"
#include "../PANGYA_DB/cmd_premium_ticket_info.hpp"
#include "../PANGYA_DB/cmd_daily_quest_info_user.hpp"
#include "../PANGYA_DB/cmd_guild_info.hpp"
#include "../PANGYA_DB/cmd_old_daily_quest_info.hpp"
#include "../PANGYA_DB/cmd_add_daily_quest.hpp"
#include "../PANGYA_DB/cmd_delete_daily_quest.hpp"
#include "../PANGYA_DB/cmd_gera_web_key.hpp"
#include "../PANGYA_DB/cmd_check_achievement.hpp"
#include "../PANGYA_DB/cmd_last_player_game_info.hpp"
#include "../PANGYA_DB/cmd_friend_info.hpp"
#include "../PANGYA_DB/cmd_attendance_reward_info.hpp"
#include "../PANGYA_DB/cmd_register_logon_server.hpp"
#include "../PANGYA_DB/cmd_grand_zodiac_pontos.hpp"
#include "../PANGYA_DB/cmd_legacy_tiki_shop_info.hpp"

#include "../PANGYA_DB/cmd_update_user_info.hpp"

#include "../PANGYA_DB/cmd_grand_prix_clear.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../../Projeto IOCP/UTIL/iff.h"

#include <algorithm>

#include "papel_shop_system.hpp"
#include "premium_system.hpp"

#include "../Game Server/game_server.h"

#include "item_manager.h"
using namespace stdA;

bool LoginManager::m_same_id_login = false;
std::string LoginManager::m_client_version;

#if defined(_WIN32)
DWORD LoginManager::_checkTaskFinish(LPVOID _lpParameter) {
#elif defined(__linux__)
void* LoginManager::_checkTaskFinish(LPVOID _lpParameter) {
#endif

	if (_lpParameter != nullptr) {
		auto lm = reinterpret_cast<LoginManager*>(_lpParameter);

		return lm->checkTaskFinish();
	}

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
}

#if defined(_WIN32)
DWORD LoginManager::checkTaskFinish() {
#elif defined(__linux__)
void* LoginManager::checkTaskFinish() {
#endif


#if defined(_WIN32)
	while (InterlockedCompareExchange(&m_check_task_finish_shutdown, 1u, 1u) == 1u) {
#elif defined(__linux__)
	uint32_t check_m = 1; // Compare

	while (__atomic_compare_exchange_n(&m_check_task_finish_shutdown, &check_m, 1u, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		for (auto i = 0u; i < v_task.size(); ++i) {
			if (v_task[i] != nullptr && v_task[i]->isFinished()) {
				delete v_task[i];

				v_task.erase(v_task.begin() + i--/*tira 1 do 'i' que esta sendo excluido 1 elemento do vector*/);
				v_task.shrink_to_fit();
			}
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		// Veririfca de 1 em 1 segundo
#if defined(_WIN32)
		Sleep(10000);
#elif defined(__linux__)
		usleep(1000000);
#endif
	}

	// Log
#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
	}

LoginManager::LoginManager() : v_task(), m_pThread(nullptr) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

#if defined(_WIN32)
	InterlockedExchange(&m_check_task_finish_shutdown, 1u);
#elif defined(__linux__)
	__atomic_store_n(&m_check_task_finish_shutdown, 1u, __ATOMIC_RELAXED);
#endif

	v_task.reserve(2000); // tarefas ao mesmo tempo no maximo

	try {
		loadIni();

		m_pThread = new thread(1, LoginManager::_checkTaskFinish, this, THREAD_PRIORITY_LOWEST);

	}
	catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][Error] " + e.getFullMessageError(), CL_ONLY_CONSOLE));
	}
}

LoginManager::~LoginManager() {

	clear();

	// Exit Thread
	try {

		if (m_pThread != nullptr) {

#if defined(_WIN32)
			InterlockedDecrement(&m_check_task_finish_shutdown);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_check_task_finish_shutdown, 1u, __ATOMIC_RELAXED);
#endif

			m_pThread->waitThreadFinish(INFINITE);

			delete m_pThread;
		}

	}
	catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[LoginManager::~LoginManager][ErrorSystem] " + e.getFullMessageError(), CL_ONLY_CONSOLE));

		if (m_pThread != nullptr) {

			m_pThread->exit_thread();

			delete m_pThread;
		}
	}

	m_pThread = nullptr;

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void LoginManager::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!v_task.empty()) {

		for (auto& el : v_task) {
			if (el != nullptr)
				delete el;	// Aqui pode da erro por que está desalocando uma instancia que pode esta sendo utilizar no momento ou depois, ver como vou fazer o delete aqui
		}

		v_task.clear();
		v_task.shrink_to_fit();
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

size_t LoginManager::getSize() {

	size_t size = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	size = v_task.size();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return size;
}

LoginTask* LoginManager::createTask(player & _session, KeysOfLogin & _kol, player_info & _pi, ClientVersion & _cv, void* _gs) {

	if (getSize() == 2000)
		throw exception("[LoginManager::createTask][Error] Chegou ao limite task de login ao mesmo tempo", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 1, 0));

	auto task = new LoginTask(_session, _kol, _pi, _cv, _gs);

	task->exec();

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	v_task.push_back(task);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return task;
}

void LoginManager::deleteTask(LoginTask * _task) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find(v_task.begin(), v_task.end(), _task);

	if (it != v_task.end()) {

		delete* it;

		v_task.erase(it);
		v_task.shrink_to_fit();		// isso em concorrencia pode da erro se não usar proteção de thread(ex: CRITICAL_SECTION)
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void LoginManager::SQLDBResponse(uint32_t _msg_id, pangya_db & _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[LoginManager::SQLDBResponse][Error] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_ONLY_CONSOLE));
		return;
	}

	auto task = reinterpret_cast<LoginTask*>(_arg);

	try {

		// usa session, para ela não poder ser excluída(disconnectada) antes de ser tratada aqui
		task->getSession().usa();

		// Verifica se a session ainda é valida, essas funções já é thread-safe
		if (!task->getSession().isConnected() || !task->getSession().isCreated() || !task->getSession().getState())
			throw exception("[LoginManager::SQLDBResponse][Error] session is invalid, para tratar o pangya_db", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 25, 0));

		// Por Hora só sai, depois faço outro tipo de tratamento se precisar
		if (_pangya_db.getException().getCodeError() != 0)
			throw exception("[LoginManager::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 26, 0));

		switch (_msg_id) {
		case 0:	// Info Player
		{

			break;
		}
		case 1:	// Key Login
		{

			break;
		}
		case 2:	// Member Info - User Equip
		{


			auto pi = &task->getSession().m_pi;

			task->getSession().m_pi.ue = reinterpret_cast<CmdUserEquip*>(&_pangya_db)->getEquip();

			packet p;

			// Verifica se tem o Pacote de verificação de bots ativado
			uint32_t ttl = sgs::gs::getInstance().getBotTTL();

			packet_func::pacote1A9(p, &task->getSession(), ttl/*milliseconds*/);
			packet_func::session_send(p, &task->getSession(), 0); // Tempo para enviar um pacote, ant Bot

			NormalManagerDB::add(5, new CmdTutorialInfo(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(6, new CmdCouponGacha(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(7, new CmdUserInfo(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(8, new CmdGuildInfo(pi->uid, 0), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(9, new CmdDolfiniLockerInfo(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(10, new CmdCookie(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(11, new CmdTrofelInfo(pi->uid, CmdTrofelInfo::CURRENT), LoginManager::SQLDBResponse, task);

			// Esses que estavam aqui coloquei no resposta do CmdUserEquip, por que eles precisam da resposta do User Equip

			NormalManagerDB::add(16, new CmdMyRoomConfig(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(18, new CmdCheckAchievement(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(20, new CmdDailyQuestInfoUser(pi->uid, CmdDailyQuestInfoUser::GET), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(21, new CmdCardInfo(pi->uid, CmdCardInfo::ALL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(22, new CmdCardEquipInfo(pi->uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(23, new CmdTrophySpecial(pi->uid, CmdTrophySpecial::CURRENT, CmdTrophySpecial::NORMAL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(24, new CmdTrophySpecial(pi->uid, CmdTrophySpecial::CURRENT, CmdTrophySpecial::GRAND_PRIX), LoginManager::SQLDBResponse, task);

			break;
		}
		case 3: // User Equip - Desativa
		{

			break;
		}
		case 4:	// Premium Ticket
		{
			auto pi = &task->getSession().m_pi;

			pi->pt = reinterpret_cast<CmdPremiumTicketInfo*>(&_pangya_db)->getInfo();
			//pi->pt = pangya_db::getPremiumTicket(pi->uid);

			// Att Capability do player
			// Verifica se tem premium ticket para mandar o pacote do premium user e a comet
			if (sPremiumSystem::getInstance().isPremiumTicket(pi->pt._typeid) && pi->pt.id != 0 && pi->pt.unix_sec_date > 0) {

				sPremiumSystem::getInstance().updatePremiumUser(task->getSession());
			}

			break;
		}
		case 5: // Tutorial Info
		{

			task->getSession().m_pi.TutoInfo = reinterpret_cast<CmdTutorialInfo*>(&_pangya_db)->getInfo();	// cmd_tti.getInfo();
			//pi->TutoInfo = pangya_db::getTutorialInfo(pi->uid);

			// Manda pacote do tutorial aqui
			packet p;
			packet_func::pacote11F(p, &task->getSession(), &task->getSession().m_pi, 3/*tutorial info, 3 add do zero init*/);
			packet_func::session_send(p, &task->getSession(), 0);

			break;
		}
		case 6:	// Coupon Gacha
		{
			task->getSession().m_pi.cg = reinterpret_cast<CmdCouponGacha*>(&_pangya_db)->getCouponGacha();	// cmd_cg.getCouponGacha();
			//pi->cg = pangya_db::getCouponGacha(pi->uid);

			// Não sei se o que é esse pacote, então não sei o que ele busca no banco de dados, mas depois descubro
			// Deixar ele enviando aqui por enquanto
			packet p;
			packet_func::pacote101(p, &task->getSession());
			packet_func::session_send(p, &task->getSession(), 0);	// pacote novo do JP

			break;
		}
		case 7:	// User Info
		{

			auto pi = &task->getSession().m_pi;

			pi->ui = reinterpret_cast<CmdUserInfo*>(&_pangya_db)->getInfo();	// cmd_ui.getInfo();

			NormalManagerDB::add(26, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::NORMAL, CmdMapStatistics::M_NORMAL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(27, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::ASSIST, CmdMapStatistics::M_NORMAL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(28, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::NORMAL, CmdMapStatistics::M_NATURAL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(29, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::ASSIST, CmdMapStatistics::M_NATURAL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(30, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::NORMAL, CmdMapStatistics::M_GRAND_PRIX), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(31, new CmdMapStatistics(task->getSession().m_pi.uid, CmdMapStatistics::CURRENT, CmdMapStatistics::ASSIST, CmdMapStatistics::M_GRAND_PRIX), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(36, new CmdChatMacroUser(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(38, new CmdFriendInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			break;
		}
		case 8:	// Guild Info
		{
			task->getSession().m_pi.gi = reinterpret_cast<CmdGuildInfo*>(&_pangya_db)->getInfo();	// cmd_gi.getInfo();
			break;
		}
		case 9:		// Donfini Locker Info
		{
			task->getSession().m_pi.df = reinterpret_cast<CmdDolfiniLockerInfo*>(&_pangya_db)->getInfo();	// cmd_df.getInfo();
			break;
		}
		case 10:	// Cookie
		{
			task->getSession().m_pi.cookie = reinterpret_cast<CmdCookie*>(&_pangya_db)->getCookie();	// cmd_cookie.getCookie();

			//NormalManagerDB::add(32, new CmdMailBoxInfo(task->getSession().m_pi.uid, CmdMailBoxInfo::NAO_LIDO), LoginManager::SQLDBResponse, task);
			NormalManagerDB::add(32, new CmdMailBoxInfo2(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(33, new CmdCaddieInfo(task->getSession().m_pi.uid, CmdCaddieInfo::FERIAS), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(34, new CmdMsgOffInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(35, new CmdItemBuffInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(37, new CmdLastPlayerGameInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(39, new CmdAttendanceRewardInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(42, new CmdGrandPrixClear(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(43, new CmdGrandZodiacPontos(task->getSession().m_pi.uid, CmdGrandZodiacPontos::eCMD_GRAND_ZODIAC_TYPE::CGZT_GET), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(44, new CmdLegacyTikiShopInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			break;
		}
		case 11:	// Trofel Info atual
		{
			task->getSession().m_pi.ti_current_season = reinterpret_cast<CmdTrofelInfo*>(&_pangya_db)->getInfo();	// cmd_ti.getInfo();

			NormalManagerDB::add(12, new CmdCharacterInfo(task->getSession().m_pi.uid, CmdCharacterInfo::ALL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(13, new CmdCaddieInfo(task->getSession().m_pi.uid, CmdCaddieInfo::ALL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(14, new CmdMascotInfo(task->getSession().m_pi.uid, CmdMascotInfo::ALL), LoginManager::SQLDBResponse, task);

			NormalManagerDB::add(15, new CmdWarehouseItem(task->getSession().m_pi.uid, CmdWarehouseItem::ALL), LoginManager::SQLDBResponse, task);

			break;
		}
		case 12:	// Character Info
		{

			auto pi = &task->getSession().m_pi;

			pi->mp_ce = reinterpret_cast<CmdCharacterInfo*>(&_pangya_db)->getAllInfo();	// cmd_ci.getAllInfo();

			pi->ei.char_info = nullptr;

			// Add Structure de estado do lounge para cada character do player
			for (auto& el : pi->mp_ce)
				pi->mp_scl.insert(std::make_pair(el.second.id, StateCharacterLounge()));

			// Att Character Equipado que não tem nenhum character o player
			if (pi->ue.character_id == 0 || pi->mp_ce.size() <= 0)
				pi->ue.character_id = 0;
			else { // Character Info(CharEquip)

				// É um Map, então depois usa o find com a Key, que é mais rápido que rodar ele em um loop
				auto it = pi->mp_ce.find(pi->ue.character_id);

				if (it != pi->mp_ce.end())
					pi->ei.char_info = &it->second;
			}

			// teste Calcula a condição do player e o sexo
			// Só faz calculo de Quita rate depois que o player
			// estiver no level Beginner E e jogado 50 games
			if (pi->level >= 6 && pi->ui.jogado >= 50) {
				float rate = pi->ui.getQuitRate();

				if (rate < GOOD_PLAYER_ICON)
					pi->mi.state_flag.stFlagBit.azinha = 1;
				else if (rate >= QUITER_ICON_1 && rate < QUITER_ICON_2)
					pi->mi.state_flag.stFlagBit.quiter_1 = 1;
				else if (rate >= QUITER_ICON_2)
					pi->mi.state_flag.stFlagBit.quiter_2 = 1;
			}

			if (pi->ei.char_info != nullptr && pi->ui.getQuitRate() < GOOD_PLAYER_ICON)
				pi->mi.state_flag.stFlagBit.icon_angel = pi->ei.char_info->AngelEquiped();
			else
				pi->mi.state_flag.stFlagBit.icon_angel = 0u;

			pi->mi.state_flag.stFlagBit.sexo = pi->mi.sexo;

			break;
		}
		case 13:	// Caddie Info
		{

			auto pi = &task->getSession().m_pi;

			pi->mp_ci = reinterpret_cast<CmdCaddieInfo*>(&_pangya_db)->getInfo();	// cmd_cadi.getInfo();

			// Check Caddie Times
			player_manager::checkCaddie(task->getSession());

			pi->ei.cad_info = nullptr;

			// Att Caddie Equipado que não tem nenhum caddie o player
			if (pi->ue.caddie_id == 0 || pi->mp_ci.size() <= 0)
				pi->ue.caddie_id = 0;
			else { // Caddie Info

				// É um Map, então depois usa o find com a Key, qui é mais rápido que rodar ele em um loop
				auto it = pi->mp_ci.find(pi->ue.caddie_id);

				if (it != pi->mp_ci.end())
					pi->ei.cad_info = &it->second;
			}
			break;
		}
		case 14:	// Mascot Info
		{

			auto pi = &task->getSession().m_pi;

			pi->mp_mi = reinterpret_cast<CmdMascotInfo*>(&_pangya_db)->getInfo(); // cmd_mi.getInfo();

			// Check Mascot Times
			player_manager::checkMascot(task->getSession());

			pi->ei.mascot_info = nullptr;

			// Att Mascot Equipado que não tem nenhum mascot o player
			if (pi->ue.mascot_id == 0 || pi->mp_mi.size() <= 0)
				pi->ue.mascot_id = 0;
			else { // Mascot Info

				// É um Map, então depois usa o find com a Key, qui é mais rápido que rodar ele em um loop
				auto it = pi->mp_mi.find(pi->ue.mascot_id);

				if (it != pi->mp_mi.end())
					pi->ei.mascot_info = &it->second;
			}
			break;
		}
		case 15:	// Warehouse Item
		{

			auto pi = &task->getSession().m_pi;

			pi->mp_wi = reinterpret_cast<CmdWarehouseItem*>(&_pangya_db)->getInfo();	// cmd_wi.getInfo();

			// Check Warehouse Item Times
			player_manager::checkWarehouse(task->getSession());

			// Iterator
			std::multimap< PlayerInfo::stIdentifyKey, UpdateItem >::iterator ui_ticket_report_scroll;

			// Verifica se tem Ticket Report Scroll no update item para abrir ele e excluir. Todos que estiver, não só 1
			while ((ui_ticket_report_scroll = pi->findUpdateItemByTypeidAndType(TICKET_REPORT_SCROLL_TYPEID, UpdateItem::WAREHOUSE)) != pi->mp_ui.end()) {

				try {

					auto pWi = pi->findWarehouseItemById(ui_ticket_report_scroll->second.id);

					if (pWi != nullptr)
						item_manager::openTicketReportScroll(task->getSession(), pWi->id, ((pWi->c[1] * 0x800) | pWi->c[2]));

				}
				catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[LoginManager::checkWarehouse][ErrorSystem] " + e.getFullMessageError(), CL_ONLY_CONSOLE));

					// Session inválida
					if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::_ITEM_MANAGER, 2550))
						throw exception("[LoginManager::SQLDBResponse][Error] " + e.getFullMessageError(), STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 2550, 0));
					else
						throw;	// Relança
				}
			}

			pi->ei.csi.clear();

			std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator it;

			// Att ClubSet Equipado que não tem nenhum clubset o player
			if (pi->ue.clubset_id != 0 && (it = pi->mp_wi.find(pi->ue.clubset_id)) != pi->mp_wi.end()) { // ClubSet Info

				pi->ei.clubset = &it->second;

				// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
				// que no original fica no warehouse msm, eu só confundi quando fiz
				// [AJEITEI JA] (tem que ajeitar na hora que coloca no DB e no DB isso)
				pi->ei.csi = { it->second.id, it->second._typeid, it->second.c };

				IFF::ClubSet* cs = sIff::getInstance().findClubSet(it->second._typeid);

				if (cs != nullptr) {

					for (auto i = 0u; i < (sizeof(pi->ei.csi.enchant_c) / sizeof(pi->ei.csi.enchant_c[0])); ++i)
						pi->ei.csi.enchant_c[i] = cs->slot[i] + it->second.clubset_workshop.c[i];

				}
				else
					throw exception("[LoginManager::SQLDBResponse][Erro] player[UID=" + std::to_string(pi->uid) + "] tentou inicializar ClubSet[TYPEID="
						+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] equipado, mas ClubSet Not exists on IFF_STRUCT do Server. Bug",
						STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 24, 0));

			}
			else {

				auto it = pi->findWarehouseItemItByTypeid(AIR_KNIGHT_SET);
				auto atual = pi->findWarehouseItemItById(pi->ue.clubset_id);
				auto item = sIff::getInstance().findClubSet(atual->second._typeid);
				auto pWi = atual->second;
				if (it != pi->mp_wi.end()) {

					pi->ue.clubset_id = it->second.id;
					pi->ei.clubset = &it->second;

					// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
					// que no original fica no warehouse msm, eu só confundi quando fiz
					// [AJEITEI JA] (tem que ajeitar na hora que coloca no DB e no DB isso)
					pi->ei.csi = { it->second.id, it->second._typeid, it->second.c };

					IFF::ClubSet* cs = sIff::getInstance().findClubSet(it->second._typeid);

					if (cs != nullptr) {

						for (auto i = 0u; i < (sizeof(pi->ei.csi.enchant_c) / sizeof(pi->ei.csi.enchant_c[0])); ++i)
							pi->ei.csi.enchant_c[i] = cs->slot[i] + it->second.clubset_workshop.c[i];

					}
					else
						throw exception("[LoginManager::SQLDBResponse][Erro] player[UID=" + std::to_string(pi->uid) + "] tentou inicializar ClubSet[TYPEID="
							+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] equipado, mas ClubSet Not exists on IFF_STRUCT do Server. Bug",
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 23, 0));

				}
				
				else {	// Não tem add o ClubSet padrão para ele(CV1)

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = AIR_KNIGHT_SET;
					bi.qntd = 1;

					item_manager::initItemFromBuyItem(*pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0 && (item.id = item_manager::addItem(item, task->getSession(), 2/*Padrão item*/, 0)) != item_manager::RetAddItem::T_ERROR
						&& (it = pi->findWarehouseItemItById(item.id)) != pi->mp_wi.end()) {

						pi->ue.clubset_id = it->second.id;
						pi->ei.clubset = &it->second;

						// Esse C do WarehouseItem, que pega do DB, não é o ja updado inicial da taqueira é o que fica tabela enchant, 
						// que no original fica no warehouse msm, eu só confundi quando fiz
						// [AJEITEI JA] (tem que ajeitar na hora que coloca no DB e no DB isso)
						pi->ei.csi = { it->second.id, it->second._typeid, it->second.c };

						IFF::ClubSet* cs = sIff::getInstance().findClubSet(it->second._typeid);

						if (cs != nullptr) {

							for (auto i = 0u; i < (sizeof(pi->ei.csi.enchant_c) / sizeof(pi->ei.csi.enchant_c[0])); ++i)
								pi->ei.csi.enchant_c[i] = cs->slot[i] + it->second.clubset_workshop.c[i];

						}
						else
							throw exception("[LoginManager::SQLDBResponse][Erro] player[UID=" + std::to_string(pi->uid) + "] tentou inicializar ClubSet[TYPEID="
								+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] equipado, mas ClubSet Not exists on IFF_STRUCT do Server. Bug",
								STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 22, 0));

					}
					else
						throw exception("[LoginManager::SQLDBResponse][Error] Player[UID=" + std::to_string(pi->uid)
							+ "] nao conseguiu adicionar o ClubSet[TYPEID=" + std::to_string(AIR_KNIGHT_SET) + "] padrao para ele. Bug",
							+STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 21, 0));
				}

}

			// Atualiza Comet(Ball) Equipada
			if (pi->ue.ball_typeid != 0 && (it = pi->findWarehouseItemItByTypeid(pi->ue.ball_typeid)) != pi->mp_wi.end()) {

				pi->ei.comet = &it->second;

			}
			else { // Default Ball

				pi->ue.ball_typeid = DEFAULT_COMET_TYPEID;

				auto it = pi->findWarehouseItemItByTypeid(DEFAULT_COMET_TYPEID);

				if (it != pi->mp_wi.end())
					pi->ei.comet = &it->second;
				else {	// não tem add a bola padrão para ele

					BuyItem bi{ 0 };
					stItem item{ 0 };

					bi.id = -1;
					bi._typeid = DEFAULT_COMET_TYPEID;
					bi.qntd = 1;

					item_manager::initItemFromBuyItem(*pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);

					if (item._typeid != 0 && (item.id = item_manager::addItem(item, task->getSession(), 2/*Padrão item*/, 0)) != item_manager::RetAddItem::T_ERROR
						&& (it = pi->findWarehouseItemItById(item.id)) != pi->mp_wi.end()) {

						pi->ei.comet = &it->second;

					}
					else
						throw exception("[LoginManager::SQLDBResponse][Error] Player[UID=" + std::to_string(pi->uid)
							+ "] nao conseguiu adicionar a Comet(Ball)[TYPEID=" + std::to_string(DEFAULT_COMET_TYPEID) + "] padrao para ele. Bug",
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_MANAGER, 20, 0));

				}
			}

			// add title gm do player
			if (pi->ue.skin_typeid[5] >= 0 && pi->mi.capability.stBit.game_master) {

				auto it = pi->findWarehouseItemItByTypeid(TITLE_GM_TYPEID);
				if (it != pi->mp_wi.end() && pi->ue.skin_typeid[5] == 0)
				{
					pi->ue.m_title = TITLE_GM_TYPEID;
				}
				else
				{
					if (it->second._typeid != TITLE_GM_TYPEID)//para add
					{
						BuyItem bi{ 0 };
						stItem item{ 0 };
						bi.id = -1;
						bi._typeid = TITLE_GM_TYPEID;
						bi.qntd = 1;
						item_manager::initItemFromBuyItem(*pi, item, bi, false, 0, 0, 1/*Não verifica o Level*/);
						if (item._typeid != 0 && (item.id = item_manager::addItem(item, task->getSession(), 2/*Padrão item*/, 0)) != item_manager::RetAddItem::T_ERROR
							&& (it = pi->findWarehouseItemItById(item.id)) != pi->mp_wi.end()) {

							pi->ue.m_title = TITLE_GM_TYPEID;
						}
					}


				}
			}


			// Premium Ticket Tem que ser chamado depois que o Warehouse Item ja foi carregado
			NormalManagerDB::add(4, new CmdPremiumTicketInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);

			break;
		}
		case 16:	// Config MyRoom
		{

			task->getSession().m_pi.mrc = reinterpret_cast<CmdMyRoomConfig*>(&_pangya_db)->getMyRoomConfig();	// cmd_mrc.getMyRoomConfig();

			NormalManagerDB::add(17, new CmdMyRoomItem(task->getSession().m_pi.uid, CmdMyRoomItem::ALL), LoginManager::SQLDBResponse, task);
			break;
		}
		case 17:	// MyRoom Item Info
		{
			task->getSession().m_pi.v_mri = reinterpret_cast<CmdMyRoomItem*>(&_pangya_db)->getMyRoomItem();	// cmd_mri.getMyRoomItem();

			break;
		}
		case 18:	// Check if have Achievement
		{
			// --------------------- AVISO ----------------------
			// esse aqui os outros tem que depender dele para, não ir sem ele
			auto cmd_cAchieve = reinterpret_cast<CmdCheckAchievement*>(&_pangya_db);

			// Cria Achievements do player
			if (!cmd_cAchieve->getLastState()) {

				// Aqui pode lançar uma excession esse block dentro do if
				auto pi = &task->getSession().m_pi;

				pi->mgr_achievement.initAchievement(pi->uid, true/*Create sem verifica se o player tem achievement, por que aqui ele já verificou*/);

				// Add o Task + 1 por que não pede o achievement do db, porque criou ele aqui e salvo no DB
				task->incremenetCount();

			}
			else {

				NormalManagerDB::add(19, new CmdAchievementInfo(task->getSession().m_pi.uid), LoginManager::SQLDBResponse, task);
			}
			break;
		}
		case 19:	// Achievement Info
		{
			auto cmd_ai = reinterpret_cast<CmdAchievementInfo*>(&_pangya_db);
			auto pi = &task->getSession().m_pi;

			// Inicializa o Achievement do player
			pi->mgr_achievement.initAchievement(pi->uid, cmd_ai->getInfo());

			break;
		}
		case 20:	// Daily Quest User Info
		{

			auto pi = &task->getSession().m_pi;

			pi->dqiu = reinterpret_cast<CmdDailyQuestInfoUser*>(&_pangya_db)->getInfo();	// cmd_dqiu.getInfo();
			// fim daily quest info player

			break;
		}
		case 21:	// Card Info
		{
			task->getSession().m_pi.v_card_info = reinterpret_cast<CmdCardInfo*>(&_pangya_db)->getInfo();	// cmd_cardi.getInfo();

			break;
		}
		case 22:	// Card Equipped Info
		{
			task->getSession().m_pi.v_cei = reinterpret_cast<CmdCardEquipInfo*>(&_pangya_db)->getInfo();	// cmd_cei.getInfo();

			// Check Card Special Times
			player_manager::checkCardSpecial(task->getSession());

			break;
		}
		case 23:	// Trofel especial normal atual
		{
			task->getSession().m_pi.v_tsi_current_season = reinterpret_cast<CmdTrophySpecial*>(&_pangya_db)->getInfo();	// cmd_tei.getInfo();

			break;
		}
		case 24:	// Trofel especial grand prix atual
		{
			task->getSession().m_pi.v_tgp_current_season = reinterpret_cast<CmdTrophySpecial*>(&_pangya_db)->getInfo();	// cmd_tei.getInfo();

			break;
		}
		case 26:	// MapStatistics normal, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();

			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_ms_normal[i->course] = *i;

			break;
		}
		case 27:	// MapStatistics Normal, assist, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();

			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_msa_normal[i->course] = *i;

			break;
		}
		case 28:	// MapStatistics Natural, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();


			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_ms_natural[i->course] = *i;

			break;
		}
		case 29:	// MapStatistics Natural, assist, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();

			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_msa_natural[i->course] = *i;

			break;
		}
		case 30:	// MapStatistics GrandPrix, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();

			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_ms_grand_prix[i->course] = *i;

			break;
		}
		case 31:	// MapStatistics GrandPrix, Assist, atual
		{
			auto& v_ms = reinterpret_cast<CmdMapStatistics*>(&_pangya_db)->getMapStatistics();	// cmd_ms.getMapStatistics();

			for (std::vector< MapStatistics >::iterator i = v_ms.begin(); i != v_ms.end(); i++)
				task->getSession().m_pi.a_msa_grand_prix[i->course] = *i;

			break;
		}
		case 32:	// [MailBox] New Email(s), Agora é a inicialização do Cache do Mail Box
		{
			//auto& v_mb = reinterpret_cast<CmdMailBoxInfo*>(&_pangya_db)->getInfo();	// cmd_mbi.getInfo();
			auto cmd_mbi2 = reinterpret_cast<CmdMailBoxInfo2*>(&_pangya_db);

			task->getSession().m_pi.m_mail_box.init(cmd_mbi2->getInfo(), task->getSession().m_pi.uid);

			auto v_mb = task->getSession().m_pi.m_mail_box.getAllUnreadEmail();

			packet p;
			packet_func::pacote210(p, &task->getSession(), v_mb);
			packet_func::session_send(p, &task->getSession(), 0);

			break;
		}
		case 33:	// Aviso Caddie Ferias
		{
			auto& v_cif = reinterpret_cast<CmdCaddieInfo*>(&_pangya_db)->getInfo();	// cmd_cadi.getInfo();

			if (!v_cif.empty()) {

				packet p;
				packet_func::pacote0D4(p, &task->getSession(), v_cif);
				packet_func::session_send(p, &task->getSession(), 0);

			}

			break;
		}
		case 34:	// Msg Off Info
		{
			auto& v_moi = reinterpret_cast<CmdMsgOffInfo*>(&_pangya_db)->getInfo();	// cmd_moi.getInfo();

			if (!v_moi.empty()) {

				packet p;
				packet_func::pacote0B2(p, &task->getSession(), v_moi);
				packet_func::session_send(p, &task->getSession(), 0);

			}

			break;
		}
		case 35:	// YamEquipedInfo ItemBuff(item que da um efeito, por tempo)
		{
			task->getSession().m_pi.v_ib = reinterpret_cast<CmdItemBuffInfo*>(&_pangya_db)->getInfo();	// cmd_yei.getInfo();

			// Check Item Buff Times
			player_manager::checkItemBuff(task->getSession());

			break;
		}
		case 36:	// Chat Macro User
		{
			task->getSession().m_pi.cmu = reinterpret_cast<CmdChatMacroUser*>(&_pangya_db)->getMacroUser();
			break;
		}
		case 37:	// Last 5 Player Game Info
		{
			task->getSession().m_pi.l5pg = reinterpret_cast<CmdLastPlayerGameInfo*>(&_pangya_db)->getInfo();
			break;
		}
		case 38:	// Friend List
		{
			task->getSession().m_pi.mp_fi = reinterpret_cast<CmdFriendInfo*>(&_pangya_db)->getInfo();
			break;
		}
		case 39:	// Attendance Reward Info
		{
			task->getSession().m_pi.ari = reinterpret_cast<CmdAttendanceRewardInfo*>(&_pangya_db)->getInfo();
			break;
		}
		case 40:	// Register Player Logon ON DB
		{
			// Não usa por que é um UPDATE
			break;
		}
		case 41:	// Register Logon of player on Server in DB
		{
			// Não usa por que é um UPDATE
			break;
		}
		case 42:	// Grand Prix Clear
		{
			task->getSession().m_pi.v_gpc = reinterpret_cast<CmdGrandPrixClear*>(&_pangya_db)->getInfo();

			break;
		}
		case 43: // Grand Zodiac Pontos
		{
			task->getSession().m_pi.grand_zodiac_pontos = reinterpret_cast<CmdGrandZodiacPontos*>(&_pangya_db)->getPontos();

			break;
		}
		case 44: // Legacy Tiki Shop(PointShop)
		{
			task->getSession().m_pi.m_legacy_tiki_pts = reinterpret_cast<CmdLegacyTikiShopInfo*>(&_pangya_db)->getInfo();

			break;
		}
		default:
			break;
		}

		// Incrementa o contador
		task->incremenetCount();

		if (task->getCount() == 39)	// 44 - 5 (38 deixei o 1, 2, 3, 40 e 41 para o game server)
			task->sendCompleteData();
		else if (task->getCount() > 0)
			task->sendReply(_msg_id + 1);

		// Devolve (deixa a session livre) ou desconnecta ela se foi requisitado
		if (task->getSession().devolve())
		{
			_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][Test1] ", CL_ONLY_CONSOLE));
			sgs::gs::getInstance().DisconnectSession(&task->getSession());
		}
	}
	catch (exception& e) {


		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LOGIN_MANAGER, 25/*Session invalid*/))
		{
			_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][Test2] ", CL_ONLY_CONSOLE));
			// Finaliza a tarefa, sem enviar nada para o player por que não pode mais a session é inválida
			task->finishSessionInvalid();
		}
		else
		{
			_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][Test4] ", CL_ONLY_CONSOLE));
			task->sendFailLogin();
		}
		// Devolve (deixa a session livre) ou desconnecta ela se foi requisitado
		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
		{
			if (task->getSession().devolve())
			{
				_smp::message_pool::getInstance().push(new message("[LoginManager::LoginManager][Test5] ", CL_ONLY_CONSOLE));
				sgs::gs::getInstance().DisconnectSession(&task->getSession());
			}
		}
	}
}

bool LoginManager::canSameIDLogin() {
	return LoginManager::m_same_id_login;
}

const std::string& LoginManager::getClientVersionSideServer() {
	return LoginManager::m_client_version;
}

void LoginManager::loadIni() {

#if defined(_WIN32)
	ReaderIni ini("\\server.ini");
#elif defined(__linux__)
	ReaderIni ini("/server.ini");
#endif

	ini.init();

	try {
		LoginManager::m_same_id_login = ini.readInt("OPTION", "SAME_ID_LOGIN");
	}
	catch (exception& e) {
		// Não precisa exibir mensagem se não encontrar essa opção no arquivo .ini
		UNREFERENCED_PARAMETER(e);
	}

	LoginManager::m_client_version = ini.readString("SERVERINFO", "CLIENTVERSION");
}
