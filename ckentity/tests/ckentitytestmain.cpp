#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ckentity/datarowset.hpp"
#include "testcomponent.hpp"

#include <vector>

static const uint32_t kRowsetCapacity = 32;

static CKEntityIteration sIteration = 0;
static CKEntityIndex sIndex = 0;

using namespace cinek;

static CKEntity createEntity(CKEntityContext context) {
    ++sIteration;
    if (sIteration == 0)
        sIteration = 1;
    ++sIndex;
    return cinek_make_entity(sIteration, context, sIndex);
}

static bool populateDataRowset
(
    cinek::component::DataRowset& rowset,
    uint32_t amt,
    std::vector<Entity>& outputEntities
)
{
    outputEntities.reserve(amt);
    outputEntities.clear();
    uint32_t i;
    for (i = 0; i < kRowsetCapacity; ++i)
    {
        Entity eid = createEntity(i % 16);
        outputEntities.push_back(eid);
        component::DataRowset::index_type idx = rowset.allocate(eid);
        if (idx == component::DataRowset::npos)
            return false;
    }
    return true;
}

template<typename Component>
bool populateDataRowsetComponents
(
    component::DataRowset& rowset,
    const Component& data
)
{
    component::DataRowset::index_type i;
    for (i = 0; i < rowset.size(); ++i)
    {
        auto comp = rowset.at<Component>(i);
        if (!comp)
            return false;
        *comp = data;
    }
    return true;
}

template<typename Component>
bool validateDataRowsetComponents
(
    component::DataRowset& rowset,
    const Component& data
)
{
    component::DataRowset::index_type i;
    for (i = 0; i < rowset.size(); ++i)
    {
        auto comp = rowset.at<Component>(i);
        if (comp)
        {
            if (memcmp(comp, &data, sizeof(*comp)) != 0)
                return false;
        }
    }
    return true;
}


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
    Entity entity = createEntity(0);
    component::DataRowset::index_type rowIndex = rowset.allocate(entity);
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

        REQUIRE(rowset.size() == 0);
        REQUIRE(rowset.firstIndex() == component::DataRowset::npos);
    }
}

TEST_CASE("DataRowset with multiple rows")
{
    component::DataRowset rowset(TestComponent::kComponentType, kRowsetCapacity);
    const float kPI = 3.14159f;
    const TestComponent kTestCompData = { 1234, kPI, true, "Test" };

    std::vector<Entity> entities;

    REQUIRE(populateDataRowset(rowset, kRowsetCapacity, entities) == true);
    REQUIRE(populateDataRowsetComponents<TestComponent>(rowset, kTestCompData) == true);
    REQUIRE(validateDataRowsetComponents<TestComponent>(rowset, kTestCompData) == true);
    REQUIRE(rowset.size() == kRowsetCapacity);

    SECTION("iterating through rowset")
    {
        uint32_t sz = rowset.size();
        uint32_t cnt = 0;
        for (auto idx = rowset.firstIndex();
             idx != component::DataRowset::npos && cnt < kRowsetCapacity;
             idx = rowset.nextIndex(idx), ++cnt)
        {
        }

        REQUIRE(sz == cnt);
    }

    SECTION("entities created match entities in rowset")
    {
        component::DataRowset::index_type i;
        for (i = 0; i < rowset.size() && i < entities.size(); ++i)
        {
            if (rowset.entityAt(i) != entities[i])
                break;
        }
        REQUIRE(i == entities.size());
    }

    SECTION("entities have expected row indices")
    {
        component::DataRowset::index_type rowIndex = 0;
        for (auto& entity : entities)
        {
            if (rowset.indexFromEntity(entity) != rowIndex)
                break;
            ++rowIndex;
        }
        REQUIRE(rowIndex == rowset.size());
    }
}

TEST_CASE("DataRowset allocation and freeing of rows")
{
    component::DataRowset rowset(TestComponent::kComponentType, kRowsetCapacity);
    const float kPI = 3.14159f;
    const TestComponent kTestCompData = { 1234, kPI, true, "Test" };

    std::vector<Entity> entities;

    populateDataRowset(rowset, kRowsetCapacity, entities);
    populateDataRowsetComponents<TestComponent>(rowset, kTestCompData);
    REQUIRE(rowset.size() == kRowsetCapacity);

    SECTION("allocating over capacity should fail")
    {
        Entity eid = createEntity(0);
        component::DataRowset::index_type idx = rowset.allocate(eid);
        REQUIRE(idx == component::DataRowset::npos);
    }

    SECTION("allocating after free succeeds")
    {
        Entity eid = rowset.entityAt(rowset.size()/2);
        REQUIRE(eid != 0);

        rowset.free(eid);
        component::DataRowset::index_type idx = rowset.allocate(eid);
        REQUIRE(idx != component::DataRowset::npos);
    }

    SECTION("Erase all rows")
    {
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
            rowset.free(*it);
        }
        REQUIRE(rowset.size() == 0);
    }
}


