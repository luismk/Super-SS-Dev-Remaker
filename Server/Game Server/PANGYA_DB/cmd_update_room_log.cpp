#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_room_log.hpp"

using namespace stdA;

CmdUpdateRoomLog::CmdUpdateRoomLog(RoomLog _log,
	bool _waiter) : //char *_nomeSala,
	pangya_db(_waiter), 
	m_log(_log)
{
}

CmdUpdateRoomLog::~CmdUpdateRoomLog() {
}


void CmdUpdateRoomLog::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {
	//faz nada 
	checkColumnNumber(1, (uint32_t)_result->cols);

	//0 falhou, 1 sucesso :D
	/*auto sucess = (unsigned char)IFNULL(atoi, _result->data[0]);

	_smp::message_pool::getInstance().push(new message("[CmdRoomLog::lineResult][Log] " + sucess? "Sucesso" : "Falhou", CL_ONLY_CONSOLE));*/
}


response* CmdUpdateRoomLog::prepareConsulta(database& _db) {
	auto r = procedure(_db, m_szConsulta, m_log.to_string());
	checkResponse(r, "nao foi possivel atualizar o log game!");

	return r;
}