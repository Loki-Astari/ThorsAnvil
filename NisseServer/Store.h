#ifndef THORSANVIL_NISSE_STORE_H
#define THORSANVIL_NISSE_STORE_H

#include "NisseServerConfig.h"
#include "NisseUtil.h"
#include "EventHandlerLibEvent.h"
#include <ThorsSocket/Server.h>
#include <ThorsSocket/SocketStream.h>
#include <variant>
#include <functional>
#include <map>
#include <deque>
#include <vector>
#include <mutex>

/*
 * The store can hold two types of object:
 *      ServerData:             Data needed by the listeners.
 *                              The ThorsSocket::Server object.
 *      StreamData:             Data needed by a connected socket that is communicating.
 *                              The ThorsSocket::SocketStream object.
 *
 * Addition both types hold the following information.
 *  2: Task                     A lambda that is executed.
 *  3: CoRoutine                A boost CoRoutine2 that allows code to yield if the socket blocks.
 *  4: LibEvent objects         These interact with the EventHandler to make sure that
 *                              callbacks activate the Co-Routine.
 *
 * When an object is first created we also active the LibEvent read Listener of that object.
 *
 * To make sure that this object is thread safe state change is only done by the master thread
 * via the method `processUpdateRequest()`. All other threads request state change
 * via the `requestChange()` method that enqueues a request to be done by the main thread.
 */

namespace TASock   = ThorsAnvil::ThorsSocket;

namespace ThorsAnvil::Nisse::Server
{

class Pynt;
class TimerAction;

/*
 * Data that can be stored.
 */
struct ServerData
{
    TASock::Server          server;
    CoRoutine               coRoutine;
    Event                   readEvent;
    Pynt*                   pynt;
};
struct StreamData
{
    TASock::SocketStream    stream;
    CoRoutine               coRoutine;
    Event                   readEvent;
    Event                   writeEvent;
    Pynt*                   pynt;
};
struct OwnedFD
{
    CoRoutine*              linkedStreamCoRoutine;
    Event                   readEvent;
    Event                   writeEvent;
};
struct SharedFD
{
    std::deque<CoRoutine*>  readWaiting;
    std::deque<CoRoutine*>  writeWaiting;
    Event                   readEvent;
    Event                   writeEvent;
};
struct TimerData
{
    int                     timerId;
    int                     waitTime;       // microseconds. Be careful => 1,000,000 per second.
    TimerAction*            timerAction;
    EventHandler*           eventHandler;
    Event                   timerEvent;
};

using StoreData = std::variant<ServerData, StreamData, OwnedFD, SharedFD, TimerData>;


/*
 * Change request objects.
 * The following are objects that can be enqueued by requestChange()
 */
struct StateUpdateCreateServer
{
    int                     fd;
    TASock::Server          server;
    ServerCreator           coRoutineCreator;
    Event                   readEvent;
    Pynt&                   pynt;
};

struct StateUpdateCreateStream
{
    int                     fd;
    TASock::SocketStream    stream;
    StreamCreator           coRoutineCreator;
    Event                   readEvent;
    Event                   writeEvent;
    Pynt&                   pynt;
};

struct StateUpdateCreateOwnedFD
{
    int                     fd;
    int                     linkedStream;
    EventType               initialWait;
    Event                   readEvent;
    Event                   writeEvent;
};

struct StateUpdateCreateSharedFD
{
    int                     fd;
    Event                   readEvent;
    Event                   writeEvent;
};

struct StateUpdateCreateTimer
{
    int                     timerId;
    int                     waitTime;       // microseconds. Be careful => 1,000,000 per second.
    TimerAction*            timerAction;
    EventBase*              eventBase;
    EventHandler*           eventHandler;
};

struct StateUpdateExternallClosed
{
    int fd;
};

struct StateUpdateRemove
{
    int fd;
};

struct StateUpdateRestoreRead
{
    int fd;
    int owner;
};

struct StateUpdateRestoreWrite
{
    int fd;
    int owner;
};


using StateUpdate = std::variant<StateUpdateCreateServer, StateUpdateCreateStream, StateUpdateCreateOwnedFD, StateUpdateCreateSharedFD, StateUpdateCreateTimer, StateUpdateExternallClosed, StateUpdateRemove, StateUpdateRestoreRead, StateUpdateRestoreWrite>;

/*
 * The store data
 */
class Store
{
    static CoRoutine    invalid;

    std::map<int, StoreData>    data;
    std::vector<StateUpdate>    updates;
    std::mutex                  updateMutex;
    int                         openStreamCount = 0;

    public:
        StoreData&      getStoreData(int fd);
        template<typename T>
        T&              getData(int fd)    {return std::get<T>(getStoreData(fd));}
        int             getOpenConnections() const {return openStreamCount;}
        void            processUpdateRequest();

        template<typename T>
        void requestChange(T&& update);

        void incActive()    {++openStreamCount;}
        void decActive()    {--openStreamCount;}
    private:
        struct ApplyUpdate
        {
            Store& store;
            ApplyUpdate(Store& store)
                : store(store)
            {}
            void operator()(StateUpdateCreateServer& update)    {store(update);}
            void operator()(StateUpdateCreateStream& update)    {store(update);}
            void operator()(StateUpdateCreateOwnedFD& update)   {store(update);}
            void operator()(StateUpdateCreateSharedFD& update)  {store(update);}
            void operator()(StateUpdateCreateTimer& update)     {store(update);}
            void operator()(StateUpdateExternallClosed& update) {store(update);}
            void operator()(StateUpdateRemove& update)          {store(update);}
            void operator()(StateUpdateRestoreRead& update)     {store(update);}
            void operator()(StateUpdateRestoreWrite& update)    {store(update);}
        };
        void operator()(StateUpdateCreateServer& update);
        void operator()(StateUpdateCreateStream& update);
        void operator()(StateUpdateCreateOwnedFD& update);
        void operator()(StateUpdateCreateSharedFD& update);
        void operator()(StateUpdateCreateTimer& update);
        void operator()(StateUpdateExternallClosed& update);
        void operator()(StateUpdateRemove& update);
        void operator()(StateUpdateRestoreRead& update);
        void operator()(StateUpdateRestoreWrite& update);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "Store.source"
#endif

#endif
