/*
 * Copyright (C) 2015-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the CoreComponents license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <list>
#include <set>
#include <map>
#include <memory>

namespace cc {

using std::list;
using std::set;
using std::map;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::static_pointer_cast;

template<class Value>
class Callback {
public:
    virtual void invoke(Value value) = 0;
};

template<class Recipient, class Value>
class Slot: public Callback<Value>
{
public:
    typedef void (Recipient::* Method)(Value);

    Slot():
        recipient_(0),
        method_(0)
    {}

    Slot(Recipient *recipient, Method method):
        recipient_(recipient),
        method_(method)
    {}

    inline void invoke(Value value) { (recipient_->*method_)(value); }

private:
    Recipient *recipient_;
    Method method_;
};

class Recipient;

class ConnectionEndPoint
{
protected:
    friend class Recipient;

    virtual void disconnect(Recipient *recipient) = 0;
    inline void reverseConnect(ConnectionEndPoint *trigger, Recipient *recipient);
    inline void reverseDisconnect(ConnectionEndPoint *trigger, Recipient *recipient);
};

template<class Value>
class Event: public ConnectionEndPoint
{
public:
    Event() {}

    ~Event() {
        disconnectAll();
    }

    template<template<class> class Ref, class Recipient>
    void connect(Ref<Recipient> &recipient, void (Recipient::* method)(Value))
    {
        connect(recipient.get(), method);
    }

    template<class Recipient>
    void connect(Recipient *recipient, void (Recipient::* method)(Value))
    {
        if (!callbacks_)
            callbacks_.reset(new CallbackListByRecipient);

        shared_ptr<CallbackList> list;
        auto it = callbacks_->find(recipient);
        if (it == callbacks_->end()) {
            list = make_shared<CallbackList>();
            callbacks_->insert(typename CallbackListByRecipient::value_type(recipient, list));
        }
        else
            list = it->second;

        list->push_back(
            static_pointer_cast< Callback<Value> >(
                make_shared< Slot<Recipient, Value> >(recipient, method)
            )
        );

        reverseConnect(this, recipient);
    }

    void disconnect(Recipient *recipient)
    {
        if (!callbacks_) return;

        if (callbacks_->erase(recipient));
        reverseDisconnect(this, recipient);
    }

    void disconnectAll()
    {
        if (!callbacks_) return;

        while (callbacks_->size() > 0)
            disconnect(callbacks_->begin()->first);
    }

    void notify(Value value)
    {
        if (!callbacks_) return;

        for (auto pair: *callbacks_) {
            for (auto callback: *pair.second)
                callback->invoke(value);
        }
    }

private:
    typedef list< shared_ptr< Callback<Value> > > CallbackList;
    typedef map< Recipient*, shared_ptr<CallbackList> > CallbackListByRecipient;

    unique_ptr<CallbackListByRecipient> callbacks_;
};

class Recipient
{
protected:
    Recipient():
        triggers_(new Triggers)
    {}

    ~Recipient() {
        while (triggers_->size() > 0)
            (*triggers_->begin())->disconnect(this);
    }

private:
    friend class ConnectionEndPoint;

    typedef set<ConnectionEndPoint*> Triggers;
    unique_ptr<Triggers> triggers_;
};

inline void ConnectionEndPoint::reverseConnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->triggers_->insert(signal);
}
inline void ConnectionEndPoint::reverseDisconnect(ConnectionEndPoint *signal, Recipient *recipient) {
    recipient->triggers_->erase(signal);
}

} // namespace cc
