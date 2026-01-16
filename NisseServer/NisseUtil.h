#ifndef THORSANVIL_NISSE_NISSE_UTIL_H
#define THORSANVIL_NISSE_NISSE_UTIL_H

#include "NisseServerConfig.h"
#include <ThorsSocket/Server.h>
#include <ThorsSocket/SocketStream.h>
#include <boost/coroutine2/all.hpp>
#include <functional>

namespace TASock   = ThorsAnvil::ThorsSocket;

namespace ThorsAnvil::Nisse::Server
{
/*
 * Forward declare Store objects.
 * See Store.h for details
 */
struct ServerData;
struct StreamData;

/*
 * Action to be performed by EventHandler
 * When the work code is blocked.
 * See EventHandler for details.
 */
enum class TaskYieldState        {RestoreRead, RestoreWrite, WaitForMore, Remove};
struct TaskYieldAction
{
    TaskYieldState      state;
    int                 fd;
};

/*
 * The types need to do the work.
 * Created by the Server object.
 */
using CoRoutine     = boost::coroutines2::coroutine<TaskYieldAction>::pull_type;
using Yield         = boost::coroutines2::coroutine<TaskYieldAction>::push_type;
using ServerTask    = std::function<void(TASock::Server& stream, Yield& yield)>;
using StreamTask    = std::function<void(TASock::SocketStream& stream, Yield& yield)>;
using ServerCreator = std::function<CoRoutine(ServerData&)>;
using StreamCreator = std::function<CoRoutine(StreamData&)>;

}

#endif
