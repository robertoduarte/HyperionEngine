#include "std/type_traits.h"
#include "SatAlloc.hpp"

constexpr size_t eventQueueSize = 64;
constexpr size_t maxEventDataSize = 32;

template <typename E>
concept Event = std::is_trivial<E>::value && (sizeof(E) < maxEventDataSize);

class EventManager
{
public:
    template <Event E>
    using EventListener = void (*)(E *);

    using EmptyEventListener = void (*)();

private:
    struct EventTypeListener
    {
        bool isEmpty;
        size_t listenerCount;
        void **listeners;

        void Init(bool isEmpty)
        {
            this->isEmpty = isEmpty;
            listenerCount = 0;
            listeners = nullptr;
        }

        void AddListener(void *callback)
        {
            size_t pos = listenerCount++;

            listeners = (void **)lwram::realloc(listeners, listenerCount * sizeof(void *));
            listeners[pos] = callback;
        }
    };

    static inline EventTypeListener *eventTypeListeners = nullptr;
    static inline int eventTypeCounter = 0;

    template <Event E>
    static int AddEventType()
    {
        size_t eventId = eventTypeCounter++;

        
        eventTypeListeners = (EventTypeListener *)lwram::realloc(eventTypeListeners, eventTypeCounter * sizeof(EventTypeListener));
        eventTypeListeners[eventId].Init(std::is_empty_v<E>);
        return eventId;
    }

    template <Event E>
    static inline const int EventId = AddEventType<E>();

    struct QueuedEvent
    {
        int eventTypeId;
        char eventData[maxEventDataSize];
    };

    static inline QueuedEvent eventQueue[eventQueueSize] = {0};
    static inline size_t lastPosition = 0;

    static void TriggerEvent(size_t eventTypeId, void *eventData)
    {
        using Listener = void (*)(void *);
        for (size_t i = 0; i < eventTypeListeners[eventTypeId].listenerCount; i++)
        {
            ((Listener)eventTypeListeners[eventTypeId].listeners[i])(eventData);
        }
    }

    static void TriggerEvent(size_t eventTypeId)
    {
        for (size_t i = 0; i < eventTypeListeners[eventTypeId].listenerCount; i++)
        {
            ((EmptyEventListener)eventTypeListeners[eventTypeId].listeners[i])();
        }
    }

public:
    template <Event E>
    static void AddListener(EventListener<E> listener)
    {
        eventTypeListeners[EventId<E>].AddListener((void *)listener);
    }

    template <Event E>
    static void AddListener(EmptyEventListener listener)
    {
        eventTypeListeners[EventId<E>].AddListener((void *)listener);
    }

    template <Event E>
        requires(!std::is_empty_v<E>)
    static void TriggerEvent(const E &ev)
    {
        TriggerEvent(EventId<E>, (void *)&ev);
    }

    template <Event E>
        requires std::is_empty_v<E>
    static void TriggerEvent()
    {
        TriggerEvent((size_t)EventId<E>);
    }

    template <Event E>
        requires(!std::is_empty_v<E>)
    static bool QueueEvent(const E &ev)
    {
        if (lastPosition >= eventQueueSize)
            return false;

        eventQueue[lastPosition].eventTypeId = EventId<E>;
        *((E *)eventQueue[lastPosition].eventData) = ev;

        lastPosition++;

        return true;
    }

    template <Event E>
        requires std::is_empty_v<E>
    static bool QueueEvent()
    {
        if (lastPosition >= eventQueueSize)
            return false;

        eventQueue[lastPosition].eventTypeId = EventId<E>;
        lastPosition++;

        return true;
    }

    static void ProcessQueuedEvents()
    {
        for (size_t i = 0; i < lastPosition; i++)
        {
            size_t eventTypeId = eventQueue[i].eventTypeId;

            if (eventTypeListeners[eventTypeId].isEmpty)
                TriggerEvent(eventTypeId);
            else
                TriggerEvent(eventTypeId, eventQueue[i].eventData);
        }
        lastPosition = 0;
    }
};