
#include "gtest/gtest.h"

#include "cinek/overview/stage.hpp"
#include "cinek/overview/map.hpp"

#include <memory>

namespace overview = cinekine::overview;

cinekine::overview::Stage* CreateStage(uint16_t xUnits, uint16_t yUnits, int16_t zUp, int16_t zDown)
{
    cinek_ov_map_bounds bounds;
    bounds.xUnits = xUnits;
    bounds.yUnits = yUnits;
    bounds.zUp = zUp;
    bounds.zDown = zDown;

    return new overview::Stage(bounds);
}


//  simple create/destroy case with a decent sized map.
TEST(StageBasic, StageCreateDestroy) {
    std::unique_ptr< overview::Stage> stage(CreateStage(256, 256, 1, -1));
}

//  pool allocation of maps.
TEST(StageBasic, StagePoolCreateDestroy) {
    std::unique_ptr< overview::Stage > stage(CreateStage(256, 256, 1, -1));
    std::shared_ptr< overview::Map > map = stage.get()->getMapPtr();
}

//  more complex map operations are tested in the map fixture.
TEST(StageMap, StageGetMapAndVerifySize) {
    const uint16_t MAP_X_SIZE = 256;
    const uint16_t MAP_Y_SIZE = 192;
    const int16_t MAP_Z_UP = 2;
    const int16_t MAP_Z_DOWN = -1;

    std::unique_ptr< overview::Stage > stage(CreateStage(MAP_X_SIZE, MAP_Y_SIZE, MAP_Z_UP, MAP_Z_DOWN));
    std::shared_ptr< overview::Map > map = stage.get()->getMapPtr();

    //  verify bounds.
    const cinek_ov_map_bounds& bounds = map.get()->getMapBounds();
    EXPECT_EQ(MAP_X_SIZE, bounds.xUnits);
    EXPECT_EQ(MAP_Y_SIZE, bounds.yUnits);
    EXPECT_EQ(MAP_Z_UP, bounds.zUp);
    EXPECT_EQ(MAP_Z_DOWN, bounds.zDown);
}

//  Verify map stays alive following a stage release if map not released.
TEST(StageMap, StageGetMapNoRelease) {
    const uint16_t MAP_X_SIZE = 256;
    const uint16_t MAP_Y_SIZE = 192;
    const int16_t MAP_Z_UP = 2;
    const int16_t MAP_Z_DOWN = -1;

    std::unique_ptr< overview::Stage > stage(CreateStage(MAP_X_SIZE, MAP_Y_SIZE, MAP_Z_UP, MAP_Z_DOWN));
    std::shared_ptr< overview::Map > map = stage.get()->getMapPtr();
    stage.reset();

    //  verify bounds on the dangling map.
    const cinek_ov_map_bounds& bounds = map.get()->getMapBounds();
    EXPECT_EQ(MAP_X_SIZE, bounds.xUnits);
    EXPECT_EQ(MAP_X_SIZE, bounds.xUnits);
    EXPECT_EQ(MAP_Y_SIZE, bounds.yUnits);
    EXPECT_EQ(MAP_Z_UP, bounds.zUp);
    EXPECT_EQ(MAP_Z_DOWN, bounds.zDown);  
}