#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

template<typename... Args>
class Event {
private:
    struct IDelegate {
        virtual ~IDelegate() = default;
        virtual void Invoke(Args... args) const = 0;
        virtual bool IsValid() const = 0;
    };

    // 1. Nyers pointeres (Member function)
    template<typename T>
    struct RawDelegate : public IDelegate {
        T* instance;
        void (T::*method)(Args...);
        RawDelegate(T* inst, void (T::*meth)(Args...)) : instance(inst), method(meth) {}
        void Invoke(Args... args) const override { (instance->*method)(std::forward<Args>(args)...); }
        bool IsValid() const override { return instance != nullptr; }
    };

    // 2. Okos pointeres (Member function)
    template<typename T>
    struct WeakDelegate : public IDelegate {
        std::weak_ptr<T> instance;
        void (T::*method)(Args...);
        WeakDelegate(std::shared_ptr<T> inst, void (T::*meth)(Args...)) : instance(inst), method(meth) {}
        void Invoke(Args... args) const override {
            if (auto sp = instance.lock()) { (sp.get()->*method)(std::forward<Args>(args)...); }
        }
        bool IsValid() const override { return !instance.expired(); }
    };

    // --- ÚJ: 3. Statikus / Globális függvénypointer ---
    struct StaticDelegate : public IDelegate {
        void (*function)(Args...);
        StaticDelegate(void (*func)(Args...)) : function(func) {}
        void Invoke(Args... args) const override {
            if (function) function(std::forward<Args>(args)...);
        }
        bool IsValid() const override { return function != nullptr; }
    };

    std::vector<std::unique_ptr<IDelegate>> listeners;

public:
    // Feliratkozás tagfüggvénnyel (nyers pointer)
    template<typename T>
    void AddListener(T* instance, void (T::*method)(Args...)) {
        listeners.push_back(std::make_unique<RawDelegate<T>>(instance, method));
    }

    // Feliratkozás tagfüggvénnyel (shared_ptr/weak_ptr)
    template<typename T>
    void AddListener(std::shared_ptr<T> instance, void (T::*method)(Args...)) {
        listeners.push_back(std::make_unique<WeakDelegate<T>>(instance, method));
    }

    // --- ÚJ: Feliratkozás statikus/globális függvénnyel ---
    void AddListener(void (*function)(Args...)) {
        listeners.push_back(std::make_unique<StaticDelegate>(function));
    }

    // Leiratkozás tagfüggvénynél
    template<typename T>
    void RemoveListener(T* instance, void (T::*method)(Args...)) {
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](const std::unique_ptr<IDelegate>& del) {
            if (auto* r = dynamic_cast<RawDelegate<T>*>(del.get())) return r->instance == instance && r->method == method;
            if (auto* w = dynamic_cast<WeakDelegate<T>*>(del.get())) return w->instance.lock().get() == instance && w->method == method;
            return false;
        }), listeners.end());
    }

    // --- ÚJ: Leiratkozás statikus függvénynél ---
    void RemoveListener(void (*function)(Args...)) {
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](const std::unique_ptr<IDelegate>& del) {
            if (auto* s = dynamic_cast<StaticDelegate*>(del.get())) return s->function == function;
            return false;
        }), listeners.end());
    }

    void Invoke(Args... args) const {
        // Miattuk nem lehet const: listeners.erase...
        // De az Event-nek nem kéne változnia Invoke közben, 
        // a lejárt listener-ek takarítása ráér, vagy megoldható máshogy.
        
        for (const auto& del : listeners) {
            if (del->IsValid()) del->Invoke(std::forward<Args>(args)...);
        }
    }
};