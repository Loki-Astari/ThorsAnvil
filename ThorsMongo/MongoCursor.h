#ifndef THORSANVIL_DB_MONGO_MONGO_CURSOR_H
#define THORSANVIL_DB_MONGO_MONGO_CURSOR_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include "ThorsMongoGetMore.h"
#include "ThorsMongoKillCursor.h"

namespace ThorsAnvil::DB::Mongo
{

// Forward Declarations.
    template<typename T>
    class CursorFirst;

    template<typename T>
    class CursorData;

    template<typename>
    class FindResult;

    class ListCollectionResult;

    class ThorsMongo;


// Basic Cursor class.
// We stream data into this type from Mongo.
// Note: This class does NOT hold any of the data.
class Cursor
{
    friend class ThorsAnvil::Serialize::Traits<Cursor>;
    bool                        partialResultsReturned  = false;
    std::uint64_t               id                      = 0;
    std::string                 ns;
    public:
        std::uint64_t           getId() const {return id;}
        friend void swap(Cursor& lhs, Cursor& rhs) noexcept
        {
            using std::swap;
            swap(lhs.partialResultsReturned, rhs.partialResultsReturned);
            swap(lhs.id,                     rhs.id);
            swap(lhs.ns,                     rhs.ns);
        }
};

// The Cursor for the results of GetMore.
// Note: Once the GetMore has complete the data is moved.
//       into an object of type CursorFirst.
//       Thus the functionality all uses CursorFirst.
template<typename T>
class CursorNext: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorNext<T>>;
    public:
        std::vector<T>          nextBatch;
        std::vector<T> const&   data()  const {return nextBatch;}
};


template<typename T>
class CursorFirst: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorFirst<T>>;
    friend class CursorData<T>;

    std::vector<T>              firstBatch;
    public:
        std::vector<T> const&   data()  const {return firstBatch;}
        friend void swap(CursorFirst<T>& lhs, CursorNext<T>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<Cursor&>(lhs), static_cast<Cursor&>(rhs));
            swap(lhs.firstBatch,            rhs.nextBatch);
        }
};

template<typename R>
class CursorIterator
{
    using T = typename R::ValueType;
    Nref<R>                 cursorData;
    bool                    input;

    public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::remove_cv_t<T>;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::input_iterator_tag;
        CursorIterator(R& cursorData, bool input)
            : cursorData(cursorData)
            , input(input)
        {}

        // Input iterator.
        // If they are both the end they are equal.
        // Since this is an input iterator then if they are not the end they are also always equal.
        friend bool operator==(CursorIterator const& lhs, CursorIterator const& rhs)    {return lhs.input == rhs.input;}
        friend bool operator!=(CursorIterator const& lhs, CursorIterator const& rhs)    {return !(lhs == rhs);}

        T& operator*()  {return cursorData.get().current();}
        T* operator->() {return &cursorData.get().current();}
        CursorIterator& operator++()     /* prefix */
        {
            // increment() returns true if more data is
            // available after getting more data.
            input = cursorData.get().increment();
            return *this;
        }
        // Postfix ++ is not supported on input iterators.
        //CursorIterator  operator++(int)  /* postfix */
};

template<typename R> // FindType<T>
struct Range
{
    using T = typename R::ValueType;

    std::shared_ptr<R>   rangeData;

    bool cursorhasData() const
    {
        // If there is no valid data: !ok
        // Or if there is no data     size() == 0
        // Then return false.
        return ((!rangeData->ok) || (rangeData->size() == 0)) ? false : true;
    }
    public:
        Range()
            : rangeData(nullptr)
        {}
        Range(std::unique_ptr<R> rangeData)
            : rangeData(std::move(rangeData))
        {}
        Range(Range&& move)
            : rangeData(std::move(move.rangeData))
        {}
        Range& operator=(Range&& move)
        {
            rangeData = std::exchange(move.rangeData, nullptr);
            return *this;
        }
        CursorIterator<R> begin()   {return CursorIterator<R>{*rangeData, cursorhasData()};}
        CursorIterator<R> end()     {return CursorIterator<R>{*rangeData, false};}

        bool isOk()                     const   {return static_cast<bool>(*rangeData);}
        std::string getHRErrorMessage() const   {return rangeData->getHRErrorMessage();}

        operator bool()                 const   {return isOk();}
        friend std::ostream& operator<<(std::ostream& str, Range<R> const& data)    {return str << data.getHRErrorMessage();}

    private:
        friend class TestFindResult<R>;
        R& getResult() const {return *rangeData;}
};

template<typename T>
class CursorData: public CmdReplyBase
{
    friend class ThorsAnvil::Serialize::Traits<CursorData<T>>;
    friend struct Range<FindResult<T>>;
    friend struct Range<ListCollectionResult>;
    friend class TestFindResult<FindResult<T>>;
    friend class TestFindResult<ListCollectionResult>;

    Nref<ThorsMongo>            owner;
    std::string                 dbName;
    std::string                 colName;
    GetMoreConfig               getMoreConfig;
    KillCursorConfig            killCursorConfig;
    std::size_t                 index;
    CursorFirst<T>              cursor;
    TimeStamp                   operationTime;
    ClusterTime                 $clusterTime;

    public:
        CursorData(ThorsMongo& owner, std::string_view dbName, std::string_view colName, GetMoreConfig const& getMoreConfig, KillCursorConfig const& killCursorConfig)
            : owner(owner)
            , dbName(dbName)
            , colName(colName)
            , getMoreConfig(getMoreConfig)
            , killCursorConfig(killCursorConfig)
            , index(0)
        {}
        ~CursorData();
        // Destructor:  See ThorsMongo.h for definition.
        //                  Need to placed after ThorsMongo class declaration.
        friend void swap(CursorData<T>& lhs, GetMoreResult<T>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<CmdReplyBase&>(lhs), static_cast<CmdReplyBase&>(rhs));
            swap(lhs.cursor                     , rhs.cursor);
            swap(lhs.operationTime              , rhs.operationTime);
            swap(lhs.$clusterTime               , rhs.$clusterTime);
        }

    private:
        friend class CursorIterator<FindResult<T>>;
        friend class CursorIterator<ListCollectionResult>;
        // These functions are for the CursorIterator.
        bool increment();
        // increment: See ThorsMongo.h for definition.
        //               Need to placed after ThorsMongo class declaration.
        T& current()
        {
            return cursor.firstBatch[index];
        }
        std::size_t size() const
        {
            return cursor.firstBatch.size();
        }
};
}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Cursor,                  partialResultsReturned, id, ns);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorFirst,             firstBatch);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorNext,              nextBatch);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::CursorData,              cursor);

#endif
