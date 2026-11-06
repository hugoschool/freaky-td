#include "Orchestrator.hpp"
#include "f-engine/asset_manager/SFMLAssetManager.hpp"
#include "f-engine/modules/display/events/EventMouseButton.hpp"
#include "f-engine/modules/display/shapes/SFMLCircle.hpp"
#include "f-engine/modules/display/shapes/SFMLRectangle.hpp"
#include "map_builder/SFMLMapBuilder.hpp"
#include "f-engine/ecs/Registry.hpp"
#include "f-engine/ecs/components/DrawableComponent.hpp"
#include "f-engine/ecs/components/PositionComponent.hpp"
#include "f-engine/ecs/components/VelocityComponent.hpp"
#include "f-engine/ecs/components/BoundaryComponent.hpp"
#include "f-engine/ecs/systems/DisplaySystem.hpp"
#include "f-engine/ecs/systems/MoveSystem.hpp"
#include "f-engine/modules/display/SFMLDisplayModule.hpp"
#include "f-engine/modules/sound/SFMLSoundModule.hpp"
#include "map_builder/SFMLMapBuilder.hpp"
#include "../common/components/TileComponent.hpp"
#include "../common/components/TowerComponent.hpp"
#include "../common/components/UnitComponent.hpp"
#include "../common/systems/TowerSystem.hpp"
#include "../common/systems/KillingSystem.hpp"
#include "../common/systems/EnnemyPathfindingSystem.hpp"
#include <chrono>
#include <memory>

freakytd::Orchestrator::Orchestrator(std::string filepath): _world(fengine::world::World(0, 10000)), _mapBuilder(std::make_unique<SFMLMapBuilder>(_world._registry)),
    _assetManager(std::make_shared<fengine::asset_manager::SFMLAssetManager>()),
    _displayModule(std::make_unique<fengine::modules::display::SFMLDisplayModule>(std::static_pointer_cast<fengine::asset_manager::SFMLAssetManager>(_assetManager))),
    _soundModule(std::make_unique<fengine::modules::sound::SFMLSoundModule>(std::static_pointer_cast<fengine::asset_manager::SFMLAssetManager>(_assetManager))),
    _levels() {

    _world._registry.componentManager.registerComponent<fengine::ecs::PositionComponent>();
    _world._registry.componentManager.registerComponent<fengine::ecs::VelocityComponent>();
    _world._registry.componentManager.registerComponent<fengine::ecs::BoundaryComponent>();
    _world._registry.componentManager.registerComponent<fengine::ecs::DrawableComponent>();
    _world._registry.componentManager.registerComponent<TileComponent>();
    _world._registry.componentManager.registerComponent<TowerComponent>();
    _world._registry.componentManager.registerComponent<UnitComponent>();

    _world._registry.systemManager.addSystem<fengine::ecs::MovementSystem>(_world._camera);
    _world._registry.systemManager.addSystem<fengine::ecs::DisplaySystem>(*_displayModule, _world._camera);
    _world._registry.systemManager.addSystem<freakytd::TowerSystem>();
    _world._registry.systemManager.addSystem<freakytd::KillingSystem>();

    try {
        _levels.parseFile(filepath);
    } catch (std::exception const &e) {
        throw e;
    }

    _mapBuilder->generateTiles(_levels.getCurrentLevel().value().get().getPaths());
    _world._registry.systemManager.addSystem<freakytd::EnnemyPathfindingSystem>(_levels.getCurrentLevel().value().get().getPaths()[0]);
}

freakytd::Orchestrator::~Orchestrator() {}

void freakytd::Orchestrator::loop() {
    bool exit = false;
    while (!exit) {
        _displayModule->clear();

        _world._registry.systemManager.update(_world._registry);

        // TODO: this can just be a queue, optional makes no sense here
        std::optional<std::queue<std::unique_ptr<fengine::modules::display::IEvent>>> events =
            _displayModule->pollEvent();
        if (events.has_value()) {
            while (events.value().empty() == false) {
                std::unique_ptr<fengine::modules::display::IEvent> event = std::move(events.value().front());
                events.value().pop();
                if (event->getType() == fengine::modules::display::EventType::Quit) {
                    exit = true;
                    break;
                }
                if (event->getType() == fengine::modules::display::EventType::Click) {
                    handleClickEvent(event->getClickCoordinates());
                }
            }
            if (exit == true) {
                break;
            }
        }

    }
}

void freakytd::Orchestrator::handleClickEvent(std::pair<float, float> &clickCoords) {
    auto &tileTypes = _world._registry.componentManager.getComponents<TileComponent>();
    auto &positions = _world._registry.componentManager.getComponents<fengine::ecs::PositionComponent>();

    for (std::size_t i = 0; i < _world._registry.entityManager.size(); i++) {
        auto &type = tileTypes[i];
        auto &pos = positions[i];

        if (!pos.has_value())
            continue;
        if (!type.has_value())
            break;

        bool inTile = (pos.value().pos_x < clickCoords.first + 5
                        && pos.value().pos_x + 50 > clickCoords.first
                        && pos.value().pos_y< clickCoords.second + 5
                        && pos.value().pos_y+ 50 > clickCoords.second);

        if (type.value().type == TileType::BASIC && inTile) {
            auto e = _world._registry.entityManager.spawnEntity();
            type.value().type = TileType::TOWER;
            _world._registry.componentManager.addComponent(e, (fengine::ecs::PositionComponent) {pos.value().pos_x, pos.value().pos_y, false});
            _world._registry.componentManager.addComponent(e, (fengine::ecs::DrawableComponent) {std::nullopt, std::make_shared<fengine::modules::display::SFMLRectangle>(50, 50), std::tuple(0, 0, 255)});
            _world._registry.componentManager.addComponent(e, (TowerComponent) {std::pair(8, 15), 150, DamageType::PHYSICAL, TowerType::ARCHER, 750, std::chrono::steady_clock::now()});
        } else if (type.value().type == TileType::PATH && inTile) {
            auto e = _world._registry.entityManager.spawnEntity();
            _world._registry.componentManager.addComponent(e, (fengine::ecs::PositionComponent) {clickCoords.first, clickCoords.second, false});
            _world._registry.componentManager.addComponent(e, (fengine::ecs::DrawableComponent) {std::nullopt, std::make_shared<fengine::modules::display::SFMLCircle>(30), std::tuple(0, 255, 255)});
            _world._registry.componentManager.addComponent(e, (UnitComponent) {UnitTeam::ENNEMIES, 100, std::pair(5, 8), std::pair(0, 0), 3, std::chrono::steady_clock::now()});
            _world._registry.componentManager.addComponent(e, (fengine::ecs::VelocityComponent) {0, 0, false});
        }

    }
}
