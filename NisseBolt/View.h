#ifndef  THORSANVIL_NISSE_BOLT_VIEW_H
#define  THORSANVIL_NISSE_BOLT_VIEW_H


#include "NisseBoltConfig.h"
#include "Handlers.h"
#include "ThorsSlack/APIViews.h"

namespace ThorsAnvil::Nisse::Bolt
{

class App;
class View
{
    friend class App;
        ThorsAnvil::Slack::API::Views::View     display;
        ThorsAnvil::Slack::View                 handlers;

    public:
        View(ThorsAnvil::Slack::API::Views::View display,ViewSubmitHandler&& submitHandler);
        View(ThorsAnvil::Slack::API::Views::View display, ViewSubmitHandler&& submitHandler, ViewClosedHandler&& closeHandler);
        virtual ~View() {}

        // Handle User Actions.
        void action(std::string const& actionId, ActionHandler&& handler);

        ThorsAnvil::Slack::API::Views::View const& getDisplayView() const;
};


}
#endif
