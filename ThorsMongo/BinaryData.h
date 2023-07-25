#ifndef THORSANVIL_DB_MONGO_INTERNAL_BINARY_DATA_H
#define THORSANVIL_DB_MONGO_INTERNAL_BINARY_DATA_H

// https://github.com/mongodb/specifications/blob/master/source/mongodb-handshake/handshake.rst

#include "ThorsMongoConfig.h"
#include "ThorSerialize/MongoUtility.h"
#include <string>

namespace ThorsAnvil::DB::Mongo::Internal
{

/*
 * Binary Blob used in authentication.
 * The "type" is the BSON Binary type
 * The "data" is the BSON Binary obect.
 */
class Binary
{
    private:
        int                 type;
        std::string         data;

    public:
        Binary(int type, std::string const& init)
            : type(type)
            , data(std::begin(init), std::end(init))
        {}
        Binary(int type)
            : type(type)
        {}
        std::string const&  getData() const             {return data;}
        int                 getType() const             {return type;}
        std::size_t         getSize() const             {return data.size();}
        void                resize(std::size_t size)    {data.resize(size);}
        char const*         getBuffer() const           {return &data[0];}
        char*               getBuffer()                 {return &data[0];}
};

/*
 * A utility class used by ThorsSerializer to serialize
 * It is registered below.
 * the binary object (As ThorsSerializer does not natively know how to serialize Binary).
 */
class BinarySerializer: public ThorsAnvil::Serialize::MongoUtility::BinarySerializer<Binary, '\x00'>
{
    public:
        virtual void writeJson(Serialize::JsonPrinter& printer, Binary const& object) const override
        {
            printer.addValue(object.getData());
        }
};


}

// Traits classes used to define how objects are serialized.
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::Internal::Binary, ThorsAnvil::DB::Mongo::Internal::BinarySerializer);

#endif
