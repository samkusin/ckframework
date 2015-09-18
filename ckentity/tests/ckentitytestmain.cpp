#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ckentity/datarowset.hpp"
#include "testcomponent.hpp"

static const uint32_t kRowsetCapacity = 32;

static CKEntityIteration sIteration = 0;
static CKEntityIndex sIndex = 0;

static CKEntity createEntity(CKEntityContext context) {
    ++sIteration;
    if (sIteration == 0)
        sIteration = 1;
    ++sIndex;
    return cinek_make_entity(sIteration, context, sIndex);
}

using namespace cinek;

TEST_CASE("DataRowset baseline state is empty", "[component]")
{
    component::DataRowset rowset(TestComponent::kComponentType, kRowsetCapacity);

    REQUIRE(rowset.size() == 0);
    REQUIRE(rowset.capacity() == kRowsetCapacity);
    REQUIRE(rowset.firstIndex() == component::DataRowset::npos);
    REQUIRE(rowset.nextIndex(component::DataRowset::npos) == component::DataRowset::npos);
    REQUIRE(rowset.prevIndex(component::DataRowset::npos) == component::DataRowset::npos);
}

TEST_CASE("DataRowset returns correct state after allocating row")
{
    component::DataRowset rowset(TestComponent::kComponentType, kRowsetCapacity);
    const float kPI = 3.14159f;
    auto entity = createEntity(0);
    auto rowIndex = rowset.allocate(entity);
    REQUIRE(rowIndex != component::DataRowset::npos);
    REQUIRE(rowset.size() == 1);

    auto data = rowset.at<TestComponent>(rowIndex);
    REQUIRE(data != nullptr);
    data->propInt = 1;
    data->propFloat = kPI;
    data->propBool = true;
    strncpy(data->propString, "Test String", sizeof(data->propString));

    SECTION("accessible via indexing operators")
    {
        uint8_t* dataPtr = rowset.at(0);
        REQUIRE(dataPtr != nullptr);
        auto comp = reinterpret_cast<TestComponent*>(dataPtr);
        REQUIRE(comp->propInt == 1);
        REQUIRE(comp->propFloat == kPI);
        REQUIRE(comp->propBool == true);
    }

    SECTION("retrieving row indices through iteration methods")
    {
        REQUIRE(rowset.firstIndex() == rowIndex);
        REQUIRE(rowset.nextIndex(rowIndex) == component::DataRowset::npos);
        REQUIRE(rowset.prevIndex(rowIndex) == component::DataRowset::npos);
    }

    SECTION("retrieving row entity")
    {
        REQUIRE(rowset.entityAt(rowIndex) == entity);
    }

    SECTION("retrieving row index from entity")
    {
        REQUIRE(rowset.indexFromEntity(entity) == rowIndex);
    }

    SECTION("freeing a row")
    {
        rowset.free(entity);

        //  freeing does not change the size (compression does)
        REQUIRE(rowset.at(rowIndex) == nullptr);
        REQUIRE(rowset.size() == 1);
        REQUIRE(rowset.firstIndex() == component::DataRowset::npos);
    }

    SECTION("compressing an empty rowset")
    {
        rowset.free(entity);
        rowset.compress();
        REQUIRE(rowset.size() == 0);
    }
}

TEST_CASE("DataRowset returns correct state after freeing")
{
    component::DataRowset rowset(TestComponent::kComponentType, kRowsetCapacity);
    const float kPI = 3.14159f;
    auto entity = createEntity(0);
    auto rowIndex = rowset.allocate(entity);
    auto data = rowset.at<TestComponent>(rowIndex);
    data->propInt = 1;
    data->propFloat = kPI;
    data->propBool = true;
    data->propString[0] = 0;

    REQUIRE(rowset.size() == 1);

    rowset.free(entity);

    //  freeing does not change the size (compression does)
    REQUIRE(rowset.at(rowIndex) == nullptr);
    REQUIRE(rowset.size() == 1);
    REQUIRE(rowset.firstIndex() == component::DataRowset::npos);

    SECTION("compressing empty rowset")
    {
        rowset.compress();

        REQUIRE(rowset.size() == 0);
    }
}


