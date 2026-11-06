#include "../common/systems/KillingSystem.hpp"
#include "../common/components/UnitComponent.hpp"

freakytd::KillingSystem::KillingSystem() {}

freakytd::KillingSystem::~KillingSystem() {}

void freakytd::KillingSystem::update(fengine::ecs::Registry &manager) {
    auto &units = manager.componentManager.getComponents<UnitComponent>();

    for (std::size_t i = 0; i < manager.entityManager.size(); i++) {
        if (!manager.entityManager.entityFromIndex(i).has_value())
            continue;

        auto &unit = units[i];

        if (!unit.has_value())
            continue;

        if (unit.value().health <= 0) {
            manager.entityManager.killEntity(manager.entityManager.entityFromIndex(i).value());
        }
    }
}
