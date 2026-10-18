#include "GFxArray.h"
#include "GFxDisplayObject.h"
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {
unsigned checks{}, additions{}, releases{}, sizeQueries{}, elementQueries{};
void Check(bool condition, const char* message)
{
    ++checks;
    if (!condition) { std::cerr << message << '\n'; std::exit(1); }
}
struct Object { int references{1}; };
struct Array : Object { std::vector<RE::GFxValue> values; bool failRead{}; };
struct Value : RE::GFxValue {
    Value(ObjectInterface& context, Object& object, ValueType type = ValueType::kObject)
    {
        _objectInterface = &context;
        _type = type;
        _type.set(ValueType::kManagedBit);
        _value.obj = &object;
    }
};
struct ManagedString : RE::GFxValue {
    const char* narrow{};
    const wchar_t* wide{};
    ManagedString(ObjectInterface& context, const char* text) : narrow(text)
    {
        _objectInterface = &context;
        _type = ValueType::kString;
        _type.set(ValueType::kManagedBit);
        _value.managedString = &narrow;
    }
    ManagedString(ObjectInterface& context, const wchar_t* text) : wide(text)
    {
        _objectInterface = &context;
        _type = ValueType::kStringW;
        _type.set(ValueType::kManagedBit);
        _value.managedWideString = &wide;
    }
};
}

// Replace only the native engine boundary in this test target; the value
// constructors, assignments, destructors and array wrapper are production code.
namespace RE {
void GFxValue::ObjectInterface::ObjectAddRef(GFxValue* value, void* object)
{
    ++additions;
    if (value->IsString() || value->IsStringW()) return;
    ++static_cast<Object*>(object)->references;
}
void GFxValue::ObjectInterface::ObjectRelease(GFxValue* value, void* object)
{
    ++releases;
    if (value->IsString() || value->IsStringW()) return;
    Check(static_cast<Object*>(object)->references > 0, "Reference released twice");
    --static_cast<Object*>(object)->references;
}
std::uint32_t GFxValue::ObjectInterface::GetArraySize(void* object) const
{
    ++sizeQueries;
    return static_cast<std::uint32_t>(static_cast<Array*>(object)->values.size());
}
bool GFxValue::ObjectInterface::GetElement(void* object, std::uint32_t index, GFxValue* output) const
{
    ++elementQueries;
    const auto& array = *static_cast<Array*>(object);
    if (array.failRead || index >= array.values.size()) return false;
    *output = array.values[index];
    return true;
}
}

int main()
{
    RE::GFxValue::ObjectInterface context(nullptr);
    Object first, second;
    {
        Value target(context, first), source(context, second);
        static_cast<RE::GFxValue&>(target) = std::move(static_cast<RE::GFxValue&>(source));
        Check(first.references == 0, "Move assignment leaked the previously managed value");
        Check(second.references == 1 && source.IsUndefined(), "Move failed to transfer ownership");
        static_cast<RE::GFxValue&>(target) = std::move(static_cast<RE::GFxValue&>(target));
        Check(second.references == 1, "Self move released the owned value");
    }
    Check(second.references == 0 && releases == 2, "Moved ownership was not released exactly once");
    Object original;
    {
        Value source(context, original);
        RE::GFxValue copy(source);
        Check(original.references == 2 && additions == 1, "Managed copy did not acquire a reference");
        RE::GFxValue moved(std::move(copy));
        Check(copy.IsUndefined() && original.references == 2, "Move constructor changed reference count");
        moved = 7.0;
        Check(original.references == 1 && moved.GetNumber() == 7.0, "Replacing a managed value did not release it");
        source = source;
        Check(original.references == 1, "Self copy changed reference count");
    }
    Check(original.references == 0, "Managed source destructor did not release");
    RE::GFxValue reset(42.0);
    reset.SetUndefined();
    Check(reset == RE::GFxValue{}, "Undefined equality depends on stale payload bits");
    reset = 17.0;
    reset.SetNull();
    Check(reset == RE::GFxValue(nullptr), "Null equality depends on stale payload bits");
    Object replaced;
    {
        Value target(context, replaced);
        RE::GFxValue number(-2.5);
        static_cast<RE::GFxValue&>(target) = std::move(number);
        Check(replaced.references == 0 && target.IsNumber() && target.GetNumber() == -2.5 && number.IsUndefined(),
              "Move from primitive failed to release/replace a managed destination");
    }
    Check(RE::GFxValue(static_cast<const char*>(nullptr)) == RE::GFxValue(static_cast<const char*>(nullptr)),
          "Null narrow-string equality failed");
    Check(!(RE::GFxValue(static_cast<const char*>(nullptr)) == RE::GFxValue("text")), "Null string equals nonnull string");
    Check(RE::GFxValue(static_cast<const wchar_t*>(nullptr)) == RE::GFxValue(static_cast<const wchar_t*>(nullptr)),
          "Null wide-string equality failed");
    Check(RE::GFxValue(L"same") == RE::GFxValue(L"same"), "Equal wide strings compare unequal");
    ManagedString narrow(context, "same"), wide(context, L"same");
    ManagedString nullNarrow(context, static_cast<const char*>(nullptr));
    ManagedString nullWide(context, static_cast<const wchar_t*>(nullptr));
    Check(nullNarrow == RE::GFxValue(static_cast<const char*>(nullptr)) &&
          nullWide == RE::GFxValue(static_cast<const wchar_t*>(nullptr)), "Managed null strings compare incorrectly");
    Check(narrow == RE::GFxValue("same") && RE::GFxValue("same") == narrow, "Managed and borrowed narrow strings compare unequal");
    Check(wide == RE::GFxValue(L"same") && RE::GFxValue(L"same") == wide, "Managed and borrowed wide strings compare unequal");
    Check(!(narrow == RE::GFxValue("different")), "Different strings compare equal");
    Check(context.IsSameContext(&context) && !context.IsSameContext(nullptr), "Null context comparison failed");
    Object detached;
    {
        Value owner(context, detached, RE::GFxValue::ValueType::kDisplayObject);
        IUI::GFxDisplayObject display(owner);
        const auto position = display.LocalToGlobal();
        Check(display.GetMovieView() == nullptr && position.x == 0 && position.y == 0,
              "Detached movie context did not return a safe position");
    }
    Check(detached.references == 0, "Detached display reference leaked");
    Array array;
    array.values = {1.0, 2.0, 3.0};
    {
        Value owner(context, array, RE::GFxValue::ValueType::kArray);
        IUI::GFxArray wrapped(owner);
        sizeQueries = elementQueries = 0;
        Check(wrapped.FindElement(3.0) == 2, "Array lookup missed its last element");
        Check(sizeQueries == 1 && elementQueries == 3, "Array lookup repeated the native size query");
        array.failRead = true;
        Check(wrapped.FindElement(RE::GFxValue{}) == -1, "Failed element read falsely matched undefined");
        array.failRead = false;
        Check(wrapped.FindElement(4.0) == -1, "Missing element was found");
        const auto before = elementQueries;
        Check(wrapped.GetElement(99).IsUndefined() && elementQueries == before, "Out-of-range array access reached engine");
    }
    Check(array.references == 0, "Array reference was leaked");
    std::cout << "InfinityUI value/reference/array tests passed: " << checks << " checks\n";
}
