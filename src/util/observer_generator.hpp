#pragma once

#define OBSERVER_DEF(name) \
struct I##name##Observer { \
    virtual void On##name##Changed() = 0; \
    virtual ~I##name##Observer() = default; \
}

#define OBSERVER_LIST(name) \
std::vector<std::weak_ptr<IPointObserver>> observers



#define OBSERVER_FUNC_H \
void Notify() const; \
void AddListener(const std::shared_ptr<IPointObserver> observer); \
void RemoveListener(const std::shared_ptr<IPointObserver> observer); \

#define OBSERVER_FUNC_CPP(name) \
void ##Name##::Notify() const { \
    for (auto& observer : observers) { \
        if (const auto p = observer.lock()) p->OnPointChanged(); \
    } \
} \
void ##Name##::AddListener(const std::shared_ptr<IPointObserver> observer) { \
    if (!observer) return; \
    observers.push_back(observer); \
} \
void ##Name##::RemoveListener(const std::shared_ptr<IPointObserver> observer) { \
    std::erase_if(observers, [&observer](const std::weak_ptr<IPointObserver>& wp) { \
        const auto sp = wp.lock(); \
        return !sp || sp == observer; \
    }); \
}

#define OBSERVER_FUNC_HPP \
void Notify() const { \
    for (auto& observer : observers) { \
        if (const auto p = observer.lock()) p->OnPointChanged(); \
    } \
} \
void AddListener(const std::shared_ptr<IPointObserver> observer) { \
    if (!observer) return; \
    observers.push_back(observer); \
} \
void RemoveListener(const std::shared_ptr<IPointObserver> observer) { \
    std::erase_if(observers, [&observer](const std::weak_ptr<IPointObserver>& wp) { \
        const auto sp = wp.lock(); \
        return !sp || sp == observer; \
    }); \
}