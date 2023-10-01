#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_event_holes.hpp"

using namespace stdA;

CmdEventHoles::CmdEventHoles(uint32_t _uid, uint32_t _holes, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_holes(_holes)
{
}


CmdEventHoles::~CmdEventHoles() {
}


void CmdEventHoles::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {
	}


response* CmdEventHoles::prepareConsulta(database& _db) {
	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_holes));
	checkResponse(r, "nao foi possivel atualizar o evento game!");

	return r;
}