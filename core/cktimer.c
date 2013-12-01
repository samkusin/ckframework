/**
 * \file    cktimer.c
 *
 *
 * \note    Created by Samir Sinha on 5/15/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "cktimer.h"
#include "ckpool.h"
#include "ckalloc.h"
#include "ckdebug.h"

/* Internal data */

struct cinek_timer_event_data
{
    struct cinek_timer_event_data* prev;
    cinek_timer sourceTimer;
    cinek_timer_event_callback cb;
    void* ctx;
    uint32_t timeScheduled;
    uint32_t delayTime;
};

struct cinek_timer_data 
{
    uint32_t thisTime;
    uint32_t lastTime;
    cinek_mempool eventPool;
    struct cinek_timer_event_data* eventTail;
};


cinek_timer cinek_timer_create(uint32_t maxEvents)
{
    struct cinek_timer_data* timerData;

    timerData = cinek_alloc_heap_alloc(sizeof(struct cinek_timer_data));
    if (timerData)
    {
        timerData->thisTime = 0;
        timerData->lastTime = 0;
        timerData->eventPool = cinek_mempool_create(NULL, maxEvents, sizeof(struct cinek_timer_event_data));
        timerData->eventTail = NULL;
        if (!timerData->eventPool)
        {
            cinek_alloc_heap_dealloc(timerData);
            timerData = NULL;
        }
    }

    return (cinek_timer)timerData;
}


void cinek_timer_update(cinek_timer timer,
                        cinek_time timeDelta)
{
    struct cinek_timer_data* timerData;
    struct cinek_timer_event_data* eventData;
    struct cinek_timer_event_data* nextEventData;

    if (!timer)
        return;
    timerData = (struct cinek_timer_data* )timer;

    /* Update timer */
    timerData->lastTime = timerData->thisTime;
    timerData->thisTime = timerData->thisTime + timeDelta;

    /* Handle scheduled events */
    nextEventData = NULL;
    eventData = timerData->eventTail;

    while (eventData)
    {
        cinek_time fireTime;
        uint8_t eventFired;

        /* determine if the event fired. */
        fireTime = eventData->timeScheduled + eventData->delayTime;
        eventFired = 0;

        if (timerData->thisTime >= fireTime)
        {
            /* account for fired time rollover */
            eventFired = fireTime >= eventData->timeScheduled || 
                    (fireTime < eventData->timeScheduled && eventData->timeScheduled > timerData->thisTime);
        }
        else if (timerData->thisTime < timerData->lastTime)
        {
            /* account for timer rollover */
            if (eventData->timeScheduled < eventData->timeScheduled + eventData->delayTime)
            {
                eventFired = (timerData->lastTime >= eventData->timeScheduled);
            }
        }

        if (eventFired)
        {
            /* detach this event from the active event list and free it. if we're
               firing the tail event, fixup the list pointer for the timer and set
               our new event data to the tail. */
            (*eventData->cb)(eventData->ctx);
            if (nextEventData)
            {
                nextEventData->prev = eventData->prev;
            }
            else
            {
                timerData->eventTail = eventData->prev;
            }
            cinek_mempool_dealloc(timerData->eventPool, eventData);
            if (nextEventData)
            {
                eventData = nextEventData->prev;
            }
            else
            {
                eventData = timerData->eventTail;
            }
        }
        else
        {
            nextEventData = eventData;
            eventData = eventData->prev;
        }
    }
}

cinek_time cinek_timer_get_time(cinek_timer timer)
{
    struct cinek_timer_data* timerData;
    timerData = (struct cinek_timer_data* )timer;
    return timerData->thisTime;
}

cinek_time cinek_timer_get_time_delta(cinek_timer timer)
{
    struct cinek_timer_data* timerData;
    timerData = (struct cinek_timer_data* )timer;
    return timerData->thisTime - timerData->lastTime;
}


void cinek_timer_destroy(cinek_timer timer)
{
    struct cinek_timer_data* timerData;
    if (timer == NULL)
        return;
    timerData = (struct cinek_timer_data* )timer;
    cinek_mempool_destroy(timerData->eventPool);
    cinek_alloc_heap_dealloc(timerData);
}


cinek_timer_event cinek_timer_schedule_event(cinek_timer timer, 
                        cinek_time delayTime, 
                        cinek_timer_event_callback cb,
                        void* ctx)
{
    struct cinek_timer_data* timerData;
    struct cinek_timer_event_data* eventData;
    
    if (!timer)
        return NULL;
    timerData = (struct cinek_timer_data* )timer;

    eventData = cinek_mempool_alloc(timerData->eventPool);
    if (eventData)
    {
        eventData->prev = timerData->eventTail;
        eventData->sourceTimer = timer;
        eventData->cb = cb;
        eventData->ctx = ctx;
        eventData->timeScheduled = timerData->thisTime;
        eventData->delayTime = delayTime;
        /* append this event to the timer's event list. */
        timerData->eventTail = eventData;
    }
    return (cinek_timer_event)eventData;
}


void cinek_timer_cancel_event(cinek_timer_event timerEvent)
{
    struct cinek_timer_data* timerData;
    struct cinek_timer_event_data* eventData;

    if (!timerEvent)
        return;
    
    eventData = (struct cinek_timer_event_data* )timerEvent;
    timerData = (struct cinek_timer_data* )eventData->sourceTimer;

    /* find event in the timer's list, unlink it and free. */
    if (eventData == timerData->eventTail)
    {
        timerData->eventTail = timerData->eventTail->prev;
    }
    else
    {
        struct cinek_timer_event_data* lastEventData;
        lastEventData = timerData->eventTail;

        while (lastEventData)
        {
            if (lastEventData->prev == eventData)
            {
                lastEventData->prev = eventData->prev;
                break;
            }
            lastEventData = lastEventData->prev;
        }
    }

    cinek_mempool_dealloc(timerData->eventPool, eventData);
}