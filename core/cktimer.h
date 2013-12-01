/**
 * \file    cktimer.h
 *
 *
 * \note    Created by Samir Sinha on 5/15/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_CKTIMER_H
#define CINEK_CKTIMER_H

#include "cktypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque timer handle. */
typedef struct cinek_timer_t {} *cinek_timer;

/** An opaque timer event handle. */
typedef struct cinek_timer_event_t {} *cinek_timer_event;

/** A callback function pointer for timer events. */
typedef void (*cinek_timer_event_callback)(void *ctx);

/** 
 * \def CINEK_TIME_MAX
 * Maximum time value. 
 */
#define CINEK_TIME_MAX UINT32_MAX

/** 
 * Creates a timer instance.  All timers start with a 'zero' time value.
 *
 * @param maxEvents Maximum number of concurrent events.
 *
 * @return A handle to a timer.
 */
cinek_timer cinek_timer_create(uint32_t maxEvents);

/**
 * Updates a timer instance.
 * @param timer     Handle to the timer.
 * @param timeDelta Delta time since last update
 */
void cinek_timer_update(cinek_timer timer,
                        cinek_time timeDelta);

/**
 * Destroys a timer instance.
 * @param timer A handle to the timer.
 */
void cinek_timer_destroy(cinek_timer timer);

/**
 * Returns the last set time value for the specified timer.
 * @param  timer Timer handle.
 * @return       Time value.
 */
cinek_time cinek_timer_get_time(cinek_timer timer);

/**
 * Returns the delta between updates.
 * @param  timer Timer handle.
 * @return       Delta time value.
 */
cinek_time cinek_timer_get_time_delta(cinek_timer timer);

/**
 * Schedules an event callback fired on the specified timer.  Upon expiration
 * the event is implicitly freed.  To explicitly cancel an active event, call
 * cinek_timer_cancel_event.
 * 
 * @param  timer     The timer handle.
 * @param  delayTime The event expiration time from now. 
 * @param  ctx       Context passed to callback.
 * @param  cb        Callback fired when event expires.
 * @return           A timer event handle.
 */
cinek_timer_event cinek_timer_schedule_event(cinek_timer timer, 
                        cinek_time delayTime, 
                        cinek_timer_event_callback cb,
                        void* ctx);

/**
 * Cancels an active timer event.
 * 
 * @param timerEvent The event to cancel.
 */
void cinek_timer_cancel_event(cinek_timer_event timerEvent);

#ifdef __cplusplus
}
#endif

#endif
