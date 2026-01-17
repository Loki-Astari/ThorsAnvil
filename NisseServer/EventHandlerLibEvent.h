#ifndef THORSANVIL_NISSE_EVENT_HANDLER_LIBEVENT_H
#define THORSANVIL_NISSE_EVENT_HANDLER_LIBEVENT_H

/*
 * Wrapper class to give LibEvent types proper RIAA characteristics.
 *
 * EventBase is not copyable or movable.    libEvent    event_base
 * Event     is moveable.                   libEvent    event
 */

#include "NisseConfig.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <event2/event.h>
#include <utility>

namespace ThorsAnvil::Nisse::Server
{

using LibEventEventBase     = ::event_base;
using LibEventEvent         = ::event;
using LibEventTimeOut       = ::timeval;

class EventHandler;
struct TimerData;
enum class EventType : short{Read = EV_READ, Write = EV_WRITE};

/*
 * This may expand as we find limitations.
 *
 * When libEvent uses 'epoll', then we can do not support listening for Read/Write on a `File`.
 * So Feature flag:
 *      FileReadWriteEvent:         true on all systems apart from 'epoll' systems.
 */
enum Feature { FileReadWriteEvent = 1};


class Event;
class EventBase
{
    friend class Event;
    LibEventEventBase*      eventBase;
    public:
        EventBase()
            : eventBase(event_base_new())
        {}
        ~EventBase()
        {
            event_base_free(eventBase);
        }

        EventBase(EventBase const&)                 = delete;
        EventBase(EventBase&&)                      = delete;
        EventBase& operator=(EventBase const&)      = delete;
        EventBase& operator=(EventBase&&)           = delete;

        void run()
        {
            event_base_loop(eventBase, EVLOOP_NO_EXIT_ON_EMPTY);
        }
        void loopBreak()
        {
            event_base_loopbreak(eventBase);
        }

        bool isFeatureEnabled(Feature feature) const
        {
            using namespace std::string_literals;
            static const std::size_t flags = buildFeatureFlags();
            return flags & static_cast<std::size_t>(feature);
        }
    private:
        std::size_t buildFeatureFlags() const
        {
            using namespace std::string_literals;
            std::size_t result = 0;

            // epoll does not support Read/Write events on file.
            if ("epoll"s != event_base_get_method(eventBase)) {
                result |= static_cast<std::size_t>(FileReadWriteEvent);
            }
            return result;
        }
};

class Event
{
    LibEventEvent*          event;

    public:
        Event(EventBase& eventBase, TimerData& timerData);
        Event(EventBase& eventBase, int fd, EventType type, EventHandler& eventHandler);

        Event()
            : event(nullptr)
        {}
        Event(Event&& move)
            : event(std::exchange(move.event, nullptr))
        {}
        Event& operator=(Event&& move)
        {
            event = std::exchange(move.event, nullptr);
            return *this;
        }
        ~Event()
        {
            if (event) {
                event_free(event);
            }
        }
        Event(Event const&)                         = delete;
        Event& operator=(Event const&)              = delete;

        void add()
        {
            int result = event_add(event, nullptr);
            if (result != 0) {
                /*
                 * If this is flagged this will probably cause issues in the code.
                 * See AsyncStream::AsyncStream (in Context.cpp) for a situation where this caused an error.
                 *      In this case we check for a specific situation and don't try and install listener.
                 * Please figure out why this is being logged and try and prevent this issue from happening as
                 * the side effects of this failing is going to be unpredictable.
                 */
                ThorsLogError("ThorsAnvil::Nisse::Server::Event", "add", "Faied to call event_add(): errno: ", errno, " Msg: ", ThorsAnvil::Utility::systemErrorMessage());
            }
        }
        void add(int microsecondsPause)
        {
            LibEventTimeOut timeout = {0, microsecondsPause};
            int result = evtimer_add(event, &timeout);
            if (result != 0) {
                ThorsLogError("ThorsAnvil::Nisse::Server::Event", "add", "Timer: ", microsecondsPause, " Faied to call event_add(): errno: ", errno, " Msg: ", ThorsAnvil::Utility::systemErrorMessage());
            }
        }
};


}

#endif
