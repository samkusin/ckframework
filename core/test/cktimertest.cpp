
#include "gtest/gtest.h"

#include "../cktimer.h"


TEST(TimerBasic, CreateDestroy) {
    cinek_timer timer = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer);
    cinek_time time0 = cinek_timer_get_time(timer);
    EXPECT_EQ(0, time0);
    cinek_timer_destroy(timer);
}

TEST(TimerBasic, DualTimersCreateDestroy) {
    cinek_timer timer0 = cinek_timer_create(1);
    cinek_timer timer1 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    ASSERT_TRUE(NULL != timer1);
    
    cinek_time time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(0, time0);
    cinek_time time1 = cinek_timer_get_time(timer1);
    EXPECT_EQ(0, time1);

    cinek_timer_destroy(timer0);
    cinek_timer_destroy(timer1);
}


TEST(TimerGetTime, TimerWithSingleUpdate) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    cinek_timer_update(timer0, 1); 
    cinek_time time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(1, time0);   
    cinek_timer_destroy(timer0); 
}

TEST(TimerGetTime, TimerWithMultipleUpdates) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);

    cinek_time time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(0, time0);
    
    cinek_timer_update(timer0, 1);
    time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(1, time0);
    cinek_timer_update(timer0, 99);
    time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(99, time0);
    
    cinek_timer_destroy(timer0);
}

TEST(TimerGetTime, DualTimersWithSingleUpdate) {
    cinek_timer timer0 = cinek_timer_create(1);
    cinek_timer timer1 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    ASSERT_TRUE(NULL != timer1);
    
    cinek_timer_update(timer0, 1);
    cinek_timer_update(timer1, 10);
    
    cinek_time time0 = cinek_timer_get_time(timer0);
    cinek_time time1 = cinek_timer_get_time(timer1);
    EXPECT_EQ(1, time0);
    EXPECT_EQ(10, time1);
    
    cinek_timer_destroy(timer0);
    cinek_timer_destroy(timer1);
}

TEST(TimerGetTime, TimerDelta) {
    const cinek_time TIME_DELTA = 1234;
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    cinek_timer_update(timer0, 1000); 
    cinek_time time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(1000, time0);   
    cinek_timer_update(timer0, time0 + TIME_DELTA);
    time0 = cinek_timer_get_time_delta(timer0);
    EXPECT_EQ(TIME_DELTA, time0);  
    cinek_timer_destroy(timer0);    
}

TEST(TimerGetTime, TimerWithRollover) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    cinek_timer_update(timer0, CINEK_TIME_MAX); 
    cinek_time time0 = cinek_timer_get_time(timer0);
    EXPECT_EQ(CINEK_TIME_MAX, time0);   
    cinek_timer_update(timer0, CINEK_TIME_MAX+1);
    time0 = cinek_timer_get_time_delta(timer0);
    EXPECT_EQ(1, time0);  
    cinek_timer_destroy(timer0);
}


void EventCallback(void *ctx)
{
    int* context = (int* )ctx;
    *context = *context + 1;
}

TEST(TimerEvent, TimerWithSingleEventFiredNoDelay) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 0, EventCallback, &callbackContext); 
    EXPECT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 1);
    EXPECT_EQ(1, callbackContext);

    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerEventWithRollover) {
    const cinek_time TIME_DELTA = 128;
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_update(timer0, CINEK_TIME_MAX - TIME_DELTA);
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, TIME_DELTA*2, 
        EventCallback, &callbackContext);
    EXPECT_TRUE(NULL != evt0);

    cinek_timer_update(timer0, TIME_DELTA / 2);
    EXPECT_EQ(0, callbackContext);

    cinek_timer_update(timer0, TIME_DELTA + 1);
    EXPECT_EQ(1, callbackContext);

    cinek_timer_destroy(timer0);       
}

TEST(TimerEvent, TimerWithOutOfEvents) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 0, EventCallback, &callbackContext); 
    ASSERT_TRUE(NULL != evt0);
    evt0 = cinek_timer_schedule_event(timer0, 1, EventCallback, &callbackContext);
    EXPECT_TRUE(NULL == evt0);
    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithSingleEventFiredDelay) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    ASSERT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 5);
    EXPECT_EQ(0, callbackContext);
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(1, callbackContext);

    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithEventsFiredDelay) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    ASSERT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 5);
    EXPECT_EQ(0, callbackContext);
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(1, callbackContext);

    //  make sure event has expired - this should not fire again.
    callbackContext = 0;
    cinek_timer_update(timer0, 15);
    EXPECT_EQ(0, callbackContext);

    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithSequentialEventsFiredDelay) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    ASSERT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 5);
    EXPECT_EQ(0, callbackContext);
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(1, callbackContext);

    //  make sure event has expired - this should not fire again.
    callbackContext = 0;
    cinek_timer_update(timer0, 15);
    EXPECT_EQ(0, callbackContext);

    //  fire a new event off 10 units after the current timer's time value.
    evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext);
    ASSERT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 20); 
    EXPECT_EQ(0, callbackContext);
    cinek_timer_update(timer0, 30); 
    EXPECT_EQ(1, callbackContext);

    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithSingleEventFiredDelayExpired) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    ASSERT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 5);
    EXPECT_EQ(0, callbackContext);
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(1, callbackContext);

    //  make sure event has expired - this should not fire again.
    callbackContext = 0;
    cinek_timer_update(timer0, 15);
    EXPECT_EQ(0, callbackContext);

    evt0 = 0;
    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithDualEventsFiredDelay) {
    const size_t NUM_EVENTS = 2;
    cinek_timer timer0 = cinek_timer_create(NUM_EVENTS);
    ASSERT_TRUE(NULL != timer0);
    cinek_timer_event evts[NUM_EVENTS];

    int callbackContext = 0;

    evts[0] = cinek_timer_schedule_event(timer0, 0, EventCallback, &callbackContext);
    ASSERT_TRUE(NULL != evts[0]);
    evts[1] = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext);
    ASSERT_TRUE(NULL != evts[1]);

    cinek_timer_update(timer0, 5);
    EXPECT_EQ(1, callbackContext);
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(2, callbackContext);
    //  verify we're no longer firing any timers (all should've expired!)
    cinek_timer_update(timer0, 50);
    EXPECT_EQ(2, callbackContext);

    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithMultipleEventsFiredDelay) {
    const size_t NUM_EVENTS = 5;
    cinek_timer timer0 = cinek_timer_create(NUM_EVENTS);
    ASSERT_TRUE(NULL != timer0);
    cinek_timer_event evts[NUM_EVENTS];

    cinek_time delayTime = 10;
    int callbackContext = 0;

    for (size_t i = 0; i < NUM_EVENTS; ++i, delayTime += 10)
    {
        evts[i] = cinek_timer_schedule_event(timer0, delayTime, EventCallback, &callbackContext);
        ASSERT_TRUE(evts[i] != NULL);
    }
    cinek_timer_update(timer0, 5);
    EXPECT_EQ(0, callbackContext);
    delayTime = 10;
    for (size_t i = 0; i < NUM_EVENTS; ++i, delayTime += 10)
    {
        cinek_timer_update(timer0, delayTime);
        EXPECT_EQ(i+1, callbackContext);
    }
    
    cinek_timer_destroy(timer0);    
}

TEST(TimerEvent, TimerWithSingleEventCancelled) {
    cinek_timer timer0 = cinek_timer_create(1);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    EXPECT_TRUE(NULL != evt0);
    cinek_timer_update(timer0, 1);
    EXPECT_EQ(0, callbackContext);

    //  cancel the event
    cinek_timer_cancel_event(evt0);
    //  make sure this event didn't fire.
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(0, callbackContext);

    cinek_timer_destroy(timer0);     
}

TEST(TimerEvent, TimerWithMutipleEventsOneCancelled) {
    cinek_timer timer0 = cinek_timer_create(2);
    ASSERT_TRUE(NULL != timer0);
    int callbackContext = 0;
    cinek_timer_event evt0 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    EXPECT_TRUE(NULL != evt0);
    cinek_timer_event evt1 = cinek_timer_schedule_event(timer0, 10, EventCallback, &callbackContext); 
    EXPECT_TRUE(NULL != evt1);
    //  cancel the event
    cinek_timer_cancel_event(evt0);
    //  make sure one event didn't fire (the one we cancelled.)  The other one did.
    cinek_timer_update(timer0, 10);
    EXPECT_EQ(1, callbackContext);

    cinek_timer_destroy(timer0);     
}

