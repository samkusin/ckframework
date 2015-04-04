/**
 * @file    Core/MessageDispatcher.hpp
 * @author  Samir Sinha
 * @date    11/15/2014
 * @brief   Handles queueing and dispatch of events to a Dispatcher
 * @copyright Copyright 2014 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#ifndef CK_Core_MessageDispatcher_hpp
#define CK_Core_MessageDispatcher_hpp

#include "message.hpp"
#include "cinek/objectpool.hpp"
#include "cinek/vector.hpp"
#include "cinek/map.hpp"

#include <functional>

namespace cinek {

class MessageQueue;

class MessageDispatcher
{
public:
    /**
     * Constructor for a MessageDispatcher,
     *
     * @param  count     The expected maximum number of messages in the queue
     * @param  allocator The memory allocator.
     */
    MessageDispatcher(size_t delegateCount, size_t classCount,
        size_t sequenceCount,
        Allocator allocator=Allocator());
    ~MessageDispatcher();

    /**
     * The EventDelegate signature
     */
    using Delegate =
        std::function<void(const SDO*, Message::SequenceId, void*)>;

    /**
     * Queues a message for dispatch
     *
     * @param  queue    The Message Queue that will contain the incoming message.
     * @param  msgClass The Message Class
     * @param  msgData  The Message Data cloned to the output queue
     * @param  cb       (Optional) Callback issued upon process of a message with
     *                  the same Sequence ID.
     * @return A handle to the supplied delegate, used for cancellation purposes.
     */
    MessageDelegateHandle queue(MessageQueue& queue,
        const Message::ClassId& msgClass,
        const SDO& msgData,
        const Delegate& cb=Delegate());
    /**
     * Send event immediately.  This event is not queued but sent to each
     * subscriber.  Using this method bypasses the point of a queue, and should
     * not be used unless the caller requires *immediate* action.  Basically use
     * queueEvent unless there's a reason to use sendEvent (if an action must
     * occur in the same frame as the trigger.)
     *
     * @param msgClass The Message Class
     * @param msg      The msg object.  The EventQueue gains ownership of the
     *                 event.
     * @param seqId    (Optional) An optional sequence Id passed to the class
     *                 delegate.  Normally there's no sequence ID attached to
     *                 class-based events, but this optional parameter allows
     *                 systems to pass an extra value to all callbacks.
     * @param
     */
    void send(const Message::ClassId& msgClass, const SDO& msgData,
              Message::SequenceId seqId=0,
              void* context=nullptr);
    /**
     * Subscribe to a specific event type
     * @param  msgClass The Message Class
     * @param  del      The delegate executed when a message of class 'id' is
     *                  processed
     * @return A handle to the subscribed delegate.  Subscribers must
     *         unsubscribe at some point if the subscriber id destroyed before
     *         the MessageDispatcher
     */
    MessageDelegateHandle on(const Message::ClassId& msgClass,
        const Delegate& del);
    /**
     * Unsubscribes a delegate.  This must occur before the delegate owner is
     * destroyed.
     *
     * @param delegateHandle The delegate object handle.
     */
    void remove(MessageDelegateHandle delegateHandle);
    /**
     * Process all events in the queue.  At the end of this method, the queue
     * is flushed.
     *
     * @param queue   The message queue to process
     * @param deltaMs The current process time
     * @param context Application supplied context
     */
    void dispatch(MessageQueue& queue, uint32_t timeMs, void* context=nullptr);

private:
    Message::SequenceId _nextSeqId;
    MessageDelegateHandle _nextDelegateHandle;

    // Store for Delegates by their delegate handle
    struct DelegateEntry
    {
        MessageDispatcher::Delegate del;
        DelegateEntry* next = nullptr;
        DelegateEntry* prev = nullptr;
        MessageDelegateHandle handle = 0;          // ref to its owner
        Message::SequenceId seqId = 0;
        int32_t refcnt = 0;
    };
    ObjectPool<DelegateEntry> _delegatePool;
    unordered_map<MessageDelegateHandle, DelegateEntry*> _delegates;

    void obtainDelegateEntry(DelegateEntry* entry);
    void releaseDelegateEntry(DelegateEntry* entry);

    // These maps contain references to DelegateEntry objects from the
    // _delegates and _delegatePool containers.
    // The _classDelegates vector contains a head node of a delegate list
    // where each sibling node resides within the _delegates array
    vector<std::pair<Message::ClassId, DelegateEntry*>> _classDelegates;
    unordered_map<Message::SequenceId, DelegateEntry*> _sequenceDelegates;

    template<typename Key, typename Container>
    auto lowerBoundIterator(const Key& key, Container& container) ->
        typename Container::iterator
    {
        return std::lower_bound(container.begin(), container.end(),
                                key,
                               [](const typename Container::value_type &l,
                                  const Key& r) -> bool
                               {
                                   return l.first < r;
                               });
    }
};

} /* namespace cinek */

#endif
