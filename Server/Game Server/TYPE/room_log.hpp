// Arquivo roohpp
// Criado em 12/02/2023 as 00:10 por LuisMK

#pragma once
#ifndef _STDA_ROOM_LOG_HPP
#define _STDA_ROOM_LOG_HPP

#include <memory>
#include "../../Projeto IOCP/UTIL/util_time.h"
#include "../TYPE/pangya_game_st.h"
#include <memory.h>
#include "pangya_game_st.h"


namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif
	struct RoomLog {
		RoomLog(uint32_t _ul = 0u) {

			memset(this, 0, sizeof(RoomLog));
		};
		void clear()
		{
			memset(this, 0, sizeof(RoomLog));
		};
		void setRomInfo(RoomInfoEx _ul, GUID _id) {
			roomId = _id;
			this->master = _ul.master;
			this->qntd_hole = _ul.qntd_hole;
			this->tipo_show = _ul.tipo_show;		// esse é o tipo que mostra no pacote, esse pode mudar dependendo do tipo real da sala, fica ou camp, ou VS ou especial, não coloca todos os tipos aqui
			this->numero = _ul.numero;
			this->modo = _ul.modo;
			this->course = unsigned char(_ul.course);
			this->trofel = _ul.trofel;
			this->tipo_ex = _ul.tipo_ex;	// tipo extended, que fala o tipo da sala certinho
			this->Is_natural = _ul.natural.stBit.natural;		// Aqui usa para Short Game Também
			this->Is_short_game = _ul.natural.stBit.short_game;		// Aqui usa para Short Game Também
			this->Is_GP = _ul.grand_prix.active;
			this->tipo = _ul.tipo;				// Tipo real da sala
			this->Is_hole_repeat = _ul.hole_repeat;			// Número do hole que vai ser repetido
			this->Is_GM_Event = _ul.flag_gm;
		};
		std::string GuidToString()
		{
			char guid_cstr[39];
			snprintf(guid_cstr, sizeof(guid_cstr),
				"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
				roomId.Data1, roomId.Data2, roomId.Data3,
				roomId.Data4[0], roomId.Data4[1], roomId.Data4[2], roomId.Data4[3],
				roomId.Data4[4], roomId.Data4[5], roomId.Data4[6], roomId.Data4[7]);

			return std::string(guid_cstr);
		}
		std::string to_string()
		{

			auto result = std::to_string(uid) + ", "
				"'"+GuidToString()+"'" ", "
				+ std::to_string(character) + ", "
				+ std::to_string(tipo) + ", "
				+ std::to_string(modo) + ", "
				+ std::to_string(qntd_hole) + ", "
				+ std::to_string(course) + ", "
				+ std::to_string(hole) + ", "
				+ std::to_string(score) + ", "
				+ std::to_string(exp) + ", "
				+ std::to_string(pang) + ", "
				+ std::to_string(bonus_pang) + ", "
				+ std::to_string(tacada_num) + ", "
				+ std::to_string(total_tacada_num) + ", "
				+ std::to_string(giveup) + ", "
				+ std::to_string(timeout) + ", "
				+ std::to_string(enter_after_started) + ", "
				+ std::to_string(finish_game) + ", "
				+ std::to_string(assist_flag) + ", "
				+ std::to_string(Win_trofeu) + ", "
				+ std::to_string(master) + ", "
				+ std::to_string(Is_short_game) + ", "
				+ std::to_string(Is_natural);

			return result;
		};
		void setPlayerInfo(int _uid, int _character,
			uint32_t _hole,
			char _score,
			uint32_t _exp,
			uint64_t _pang,
			uint64_t _bonus_pang,
			uint64_t _tacada_num,
			uint64_t _total_tacada_num,
			uint64_t _specialshot,
			char _premium,
			uint64_t _giveup,
			uint32_t _timeout,
			uint32_t _enter_after_started,
			uint32_t _finish_game,
			uint32_t _assist_flag,
			uint32_t _Win_trofeu)
		{
			uid = _uid;
			character = _character;
			hole = _hole;
			score = _score;
			exp = _exp;
			pang = _pang;
			bonus_pang = _bonus_pang;
			tacada_num = _tacada_num;
			total_tacada_num = _total_tacada_num;
			specialshot = _specialshot;			
			premium = _premium;
			giveup = _giveup;
			timeout = _timeout;
			enter_after_started = _enter_after_started;
			finish_game = _finish_game;
			assist_flag = _assist_flag;
			Win_trofeu = _Win_trofeu;
		};

		unsigned char qntd_hole;
		unsigned char tipo_show;			// esse é o tipo que mostra no pacote, esse pode mudar dependendo do tipo real da sala, fica ou camp, ou VS ou especial, não coloca todos os tipos aqui
		short numero;
		unsigned char modo;
		unsigned char course;
		
		uint32_t trofel;
		unsigned char tipo_ex;			// tipo extended, que fala o tipo da sala certinho
		uint32_t Is_GM_Event;
		uint32_t Is_natural;			// Natural Modo
		uint32_t Is_short_game;	// Short Game Modo
		uint32_t Is_GP;
		unsigned char tipo;					// Tipo real da sala
		unsigned char Is_hole_repeat;			// Número do hole que vai ser repetido		int32_t uid;
		uint32_t uid;
		int master;//dono da sala
		GUID roomId;
		int character;
		uint32_t hole;
		char score;
		uint32_t exp;
		uint64_t pang;
		uint64_t bonus_pang;
		uint64_t tacada_num;
		uint64_t total_tacada_num;
		uint64_t specialshot;
		char premium;
		uint64_t giveup;
		uint32_t timeout;
		uint32_t enter_after_started;
		uint32_t finish_game;
		uint32_t assist_flag;
		uint32_t Win_trofeu;
	};
#if defined(__linux__)
#pragma pack()
#endif
}

#endif // !_STDA_ROOM_LOG_HPP
