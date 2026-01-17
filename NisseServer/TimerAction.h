#ifndef THORSANVIL_NISSE_TIMER_ACTION_H
#define THORSANVIL_NISSE_TIMER_ACTION_H

/*
 * Virtual base class for timer events
 *
 */

#include "NisseConfig.h"

namespace ThorsAnvil::Nisse::Server
{

class TimerAction
{
    public:
        virtual ~TimerAction()                  {}
        virtual void handleRequest(int timerId) = 0;
};

}

#endif
