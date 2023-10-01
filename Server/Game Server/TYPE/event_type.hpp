#pragma once
#ifndef _STDA_EVENT_TYPE_HPP
#define _STDA_EVENT_TYPE_HPP

#include <vector>
#include <cstring> // Inclua <cstring> para usar a função memset

namespace stdA {

#if defined(__linux__)
#pragma pack(1)
#endif

    enum EventType : unsigned {
        HOLES,
        HALLOWEEN,
        MISSION
    };

    struct GameEvent {
        void clear() { std::memset(this, 0, sizeof(GameEvent)); }
        uint32_t eventId; // ID do evento
        EventType type;   // Tipo de evento
        uint32_t totalHoles;
    };

#if defined(__linux__)
#pragma pack()
#endif

} // namespace stdA

#endif // !_STDA_CARD_TYPE_HPP
