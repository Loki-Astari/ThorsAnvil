#ifndef THORSANVIL_SLACK_EVENT_GRID_H
#define THORSANVIL_SLACK_EVENT_GRID_H

// Documentation: https://docs.slack.dev/reference/events/grid_migration_started

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/grid_migration_finished
  "event": { "type": "grid_migration_finished", "enterprise_id": "EXXXXXXXX" }
// https://docs.slack.dev/reference/events/grid_migration_started
  "event": { "type": "grid_migration_started", "enterprise_id": "EXXXXXXXX" }
#endif

struct GridMigrationFinished
{
    // std::string                 type;
    std::string                 enterprise_id;
    static std::string const& typeName() {static std::string const name{"Event/GridMigrationFinished"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GridMigrationFinished, grid_migration_finished);
    ThorsAnvil_TypeFieldName(type);
};

struct GridMigrationStarted
{
    // std::string                 type;
    std::string                 enterprise_id;
    static std::string const& typeName() {static std::string const name{"Event/GridMigrationStarted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GridMigrationStarted, grid_migration_started);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GridMigrationFinished, enterprise_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GridMigrationStarted, enterprise_id);

#endif
