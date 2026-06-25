#ifndef THORSANVIL_DB_MONGO_MONGO_ACTIONS_H
#define THORSANVIL_DB_MONGO_MONGO_ACTIONS_H

#include <cstddef>
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/PrinterConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/Serialize.h"

namespace ThorsAnvil::DB::Mongo::QueryOp
{

    template<typename... T>
    struct Actions: public std::tuple<T...>
    {
        Actions(T&&... args) : std::tuple<T...>{std::move(args)...} {}
    };
    template<typename... T>
    struct ActionsSerialize
    {
        void readCustom(ThorsAnvil::Serialize::DeSerializer& parent, ThorsAnvil::Serialize::ParserInterface& parser, Actions<T...>& object);
        void writeCustom(ThorsAnvil::Serialize::Serializer& parent, ThorsAnvil::Serialize::PrinterInterface& printer, Actions<T...>const& object)
        {
            printer.openMap(getPrintSize(printer, object));
            std::apply([&](auto&&... arg){((parent.printObjectMembers(arg)), ...);}, static_cast<std::tuple<T...> const&>(object));
            printer.closeMap();
        }
        template<typename B, typename M>
        static void getPrintSizeMember(ThorsAnvil::Serialize::PrinterInterface& printer, std::string_view name, M const& value, std::size_t& sizeOfValue)
        {
            std::string_view actual = ThorsAnvil::Serialize::Override<B>::nameOverride(name);
            sizeOfValue += printer.getSizeMember(actual);
            sizeOfValue += printer.getSizeValue(value);
        }

        template<typename M>
        static void getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, M const& member, std::size_t& sizeOfValue, std::size_t& countOfValue)
        {
            auto const& members = ThorsAnvil::Serialize::Traits<M>::getMembers();
            std::apply([&](auto&&... arg){(getPrintSizeMember<M>(printer, arg.first, member.*(arg.second), sizeOfValue), ...);}, members);
            ++countOfValue;
        }
        static std::size_t getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, ThorsAnvil::DB::Mongo::QueryOp::Actions<T...> const& object)
        {
            std::tuple<T...> const& objectVal = static_cast<std::tuple<T...> const&>(object);
            std::size_t sizeOfValue = 0;
            std::size_t countOfValue = 0;
            printer.pushLevel(true);
            ((getPrintSize(printer, std::get<T>(objectVal), sizeOfValue, countOfValue),...));
            printer.popLevel();

            ThorsAnvil::Serialize::CalcSizeHelper<Actions<T...>>  calcHelper;
            return calcHelper.getPrintSize(printer, object, countOfValue, sizeOfValue);
        }
    };
}

namespace ThorsAnvil::Serialize
{
    template<typename...Args>
    class Traits<ThorsAnvil::DB::Mongo::QueryOp::Actions<Args...>>
    {
        public:
        static constexpr TraitType type = TraitType::Custom_Serialize;
        using SerializingType   = ThorsAnvil::DB::Mongo::QueryOp::ActionsSerialize<Args...>;

        static std::size_t getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, ThorsAnvil::DB::Mongo::QueryOp::Actions<Args...> const& object, bool)
        {
            return ThorsAnvil::DB::Mongo::QueryOp::ActionsSerialize<Args...>::getPrintSize(printer, object);
        }
    };
}

#endif
