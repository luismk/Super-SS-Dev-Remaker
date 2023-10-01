#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_room_log.hpp"

using namespace stdA;

CmdRoomLog::CmdRoomLog(RoomLog _log,
	bool _waiter) : //char *_nomeSala,
	pangya_db(_waiter), 
	m_log(_log)
{
}

CmdRoomLog::~CmdRoomLog() {
}


void CmdRoomLog::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {
	//faz nada 
	checkColumnNumber(1, (uint32_t)_result->cols);
	//retorna uid da sala
	//char id[36];
	//if (is_valid_c_string(_result->data[0]))
	//{
	//	STRCPY_TO_MEMORY_FIXED_SIZE(id, sizeof(id), _result->data[0]);
	//}
}


response* CmdRoomLog::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, m_log.to_string());
	checkResponse(r, "nao foi possivel criar log game!");

	return r;
}