//// Arquivo event_system.cpp
//// Criado em 15/07/2018 as 21:48 por Acrisio
//// Implementa��o da classe EventSystem
//
//#if defined(_WIN32)
//#pragma pack(1)
//#endif
//
//#if defined(_WIN32)
//#include <WinSock2.h>
//#endif
//
//#include "event_system.hpp"
//#include "../../Projeto IOCP/UTIL/exception.h"
//#include "../../Projeto IOCP/TYPE/stda_error.h"
//#include "../../Projeto IOCP/UTIL/message_pool.h"
//
//#include "../PANGYA_DB/cmd_box_info.hpp"
//
//#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"
//
//#include <algorithm>
//
//#include "../UTIL/lottery.hpp"
//
//#include "../../Projeto IOCP/UTIL/iff.h"
//
//#define CHECK_SESSION(_method) { \
//	if (!_session.getState() || !_session.isConnected() || _session.isQuit()) \
//		throw exception("[EventSystem::" + std::string((_method)) + "][Error] session is not connected", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 1, 0)); \
//} \
//
//#define CHECK_SESSION_AND_BOX(_method) { \
//	CHECK_SESSION((_method)); \
//	\
//	if (!isLoad()) \
//		throw exception("[EventSystem::" + std::string((_method)) + "][Error] Box System not loadded, please call load method first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 2, 0)); \
//	\
//	if (_ctx_b._typeid == 0) \
//		throw exception("[EventSystem::" + std::string((_method)) + "][Error] box _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 3, 0)); \
//	\
//	if (_ctx_b.item.empty()) \
//		throw exception("[EventSystem::" + std::string((_method)) + "][Error] box is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::BOX_SYSTEM, 4, 0)); \
//} \
//
//#if defined(_WIN32)
//#define TRY_CHECK			 try { \
//								EnterCriticalSection(&m_cs);
//#elif defined(__linux__)
//#define TRY_CHECK			 try { \
//								pthread_mutex_lock(&m_cs);
//#endif
//
//#if defined(_WIN32)
//#define LEAVE_CHECK				LeaveCriticalSection(&m_cs);
//#elif defined(__linux__)
//#define LEAVE_CHECK				pthread_mutex_unlock(&m_cs);
//#endif
//
//#if defined(_WIN32)
//#define CATCH_CHECK(_method) }catch (exception& e) { \
//								LeaveCriticalSection(&m_cs); \
//								\
//								_smp::message_pool::getInstance().push(new message("[EventSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#elif defined(__linux__)
//#define CATCH_CHECK(_method) }catch (exception& e) { \
//								pthread_mutex_unlock(&m_cs); \
//								\
//								_smp::message_pool::getInstance().push(new message("[EventSystem::" + std::string(_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif
//
//#define END_CHECK			 } \
//
//using namespace stdA;
//
//EventSystem::EventSystem() : m_load(false), m_event{} {
//
//#if defined(_WIN32)
//	InitializeCriticalSection(&m_cs);
//#elif defined(__linux__)
//	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
//	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
//	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
//#endif
//
//	// Inicializa
//	initialize();
//}
//
//EventSystem::~EventSystem() {
//
//	clear();
//
//#if defined(_WIN32)
//	DeleteCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_destroy(&m_cs);
//#endif
//}
//
//void EventSystem::initialize() {
//
//	TRY_CHECK;
//
////	// Carrega as box do banco de dados
////	CmdBoxInfo cmd_bi(true);	// Waiter
////
////	NormalManagerDB::add(0, &cmd_bi, nullptr, nullptr);
////
////	cmd_bi.waitEvent();
////
////	if (cmd_bi.getException().getCodeError() != 0)
////		throw cmd_bi.getException();
////
////	m_event = cmd_bi.getInfo();
////	if (m_event.size() <= 0)
////	{
////#ifdef _DEBUG
////		// !@ Teste
////		_smp::message_pool::getInstance().push(new message("[EventSystem::initialize][Log] Box System nao foi Carregado com sucesso!", CL_FILE_LOG_AND_CONSOLE));
////#endif // _RELEASE
////		//#ifdef _DEBUG
////	}
//
//	// Carregou com sucesso
//	m_load = true;
//
//	LEAVE_CHECK;
//
//	CATCH_CHECK("initialize");
//
//	// Relan�a para o server tomar as provid�ncias
//	throw;
//
//	END_CHECK;
//}
//
//void EventSystem::clear() {
//
//#if defined(_WIN32)
//	EnterCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_lock(&m_cs);
//#endif
//
//	if (!m_event.empty())
//		m_event.clear();
//
//	m_load = false;
//
//#if defined(_WIN32)
//	LeaveCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_unlock(&m_cs);
//#endif
//}
//
//void EventSystem::load() {
//
//	if (!isLoad())
//		clear();
//
//	initialize();
//}
//
//bool EventSystem::isLoad() {
//
//	bool isLoad = false;
//
//#if defined(_WIN32)
//	EnterCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_lock(&m_cs);
//#endif
//
//	isLoad = (m_load && !m_event.empty());
//
//#if defined(_WIN32)
//	LeaveCriticalSection(&m_cs);
//#elif defined(__linux__)
//	pthread_mutex_unlock(&m_cs);
//#endif
//
//	return isLoad;
//}
//
//bool stdA::EventSystem::IsEventHole()
//{
//	return TypeEvent() == 1;
//}
//
//void EventSystem::GetPrize(uint32_t _typeid, uint32_t holes)
//{
//}
//
//void EventSystem::GetPrize(uint32_t _typeid)
//{
//}
//
//
//int stdA::EventSystem::TypeEvent()
//{
//	for (auto& el : m_event) 
//	{	
//		switch (el.second.tipo)
//		{
//		case HOLES:
//			return 1;
//		case HALLOWEN:
//			return 2;
//		default:
//			return 3;
//			break;
//		}
//
//	}
//}
//
