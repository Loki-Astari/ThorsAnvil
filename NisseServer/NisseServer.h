#ifndef THORSANVIL_NISSE_SERVER_H
#define THORSANVIL_NISSE_SERVER_H

/*
 * NisseServer:
 *  Holds
 *      JobQueue:       This is a set of background thread to do any work set by the user.
 *      Store:          All stage information needed by the server.
 *                      Storage is thread safe assuming:
 *                          Only main thread adds new data.
 *                          Each thread only reads the object that it is acting on.
 *      EventHandler:   LibEvent wrapper.
 *                      It hold's all the information needed to processes a connection.
 *  The server puts appropriate "lambdas" into the Event Handler to processes a socket.
 */

#include "NisseServerConfig.h"
#include "JobQueue.h"
#include "Store.h"
#include "EventHandler.h"
#include <ThorsSocket/SocketStream.h>

namespace TASock = ThorsAnvil::ThorsSocket;

namespace ThorsAnvil::Nisse::Server
{

class Context;

class NisseServer
{
    friend class Context;

    JobQueue                        jobQueue;
    Store                           store;
    EventHandler                    eventHandler;

    public:
        NisseServer(std::size_t workerCount = 1);

        void run(std::function<void()>&& notice = [](){});
        void stopSoft();
        void stopHard();
        bool isFeatureEnabled(Feature feature) const {return eventHandler.isFeatureEnabled(feature);}
        void listen(TASock::ServerInit&& listenerInit, Pynt& pynt);
        template<typename T, typename rep>
        int  addTimer(std::chrono::duration<T, rep> time, TimerAction& action)
        {
            return eventHandler.addTimer(std::chrono::microseconds(time).count(), action);
        }

    private:
        CoRoutine  createStreamJob(StreamData& info);
        CoRoutine  createAcceptJob(ServerData& info);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "Context.h"
#include "Context.source"
#include "PyntControl.h"
#include "PyntControl.source"
#include "NisseServer.source"
#endif

#endif
