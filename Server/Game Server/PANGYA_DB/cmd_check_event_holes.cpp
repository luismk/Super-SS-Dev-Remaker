#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_check_event_holes.hpp"

using namespace stdA;

CmdCheckEventHoles::CmdCheckEventHoles(bool _waiter) : pangya_db(_waiter)
{
}


CmdCheckEventHoles::~CmdCheckEventHoles() {
}


void CmdCheckEventHoles::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {
}


response* CmdCheckEventHoles::prepareConsulta(database& _db) {
	auto r = procedure(_db, m_szConsulta, "");
	checkResponse(r, "nao foi possivel atualizar o evento game!");

	return r;
}