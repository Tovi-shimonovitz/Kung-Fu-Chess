#pragma once
#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventBus {
public:
    template<typename EventT>
    void subscribe(std::function<void(const EventT&)> handler) {
        auto wrapper = [handler](const std::any& boxed) {
            handler(std::any_cast<const EventT&>(boxed));
        };
        handlers[std::type_index(typeid(EventT))].push_back(std::move(wrapper));
    }

    template<typename EventT>
    void publish(const EventT& event) {
        auto it = handlers.find(std::type_index(typeid(EventT)));
        if (it == handlers.end()) return;
        std::any boxed = event;
        for (auto& handler : it->second) handler(boxed);
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const std::any&)>>> handlers;
};
