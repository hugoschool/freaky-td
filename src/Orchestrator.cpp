#include "Orchestrator.hpp"
#include "f-engine/asset_manager/SFMLAssetManager.hpp"
#include "f-engine/modules/display/shapes/SFMLCircle.hpp"
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

    _world._registry.systemManager.addSystem<fengine::ecs::MovementSystem>(_world._camera);
    _world._registry.systemManager.addSystem<fengine::ecs::DisplaySystem>(*_displayModule, _world._camera);

    try {
        _levels.parseFile(filepath);
    } catch (std::exception const &e) {
        throw e;
    }

    _mapBuilder->generateTiles(_levels.getCurrentLevel().value().get().getPaths());
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
        if (type.value().type == TileType::PATH)
            continue;

        if ((pos.value().pos_x < clickCoords.first + 5
            && pos.value().pos_x + 50 > clickCoords.first
            && pos.value().pos_y< clickCoords.second + 5
            && pos.value().pos_y+ 50 > clickCoords.second)) {
            auto e = _world._registry.entityManager.spawnEntity();
            _world._registry.componentManager.addComponent(e, (fengine::ecs::PositionComponent) {clickCoords.first, clickCoords.second, false});
            _world._registry.componentManager.addComponent(e, (fengine::ecs::DrawableComponent) {std::nullopt, std::make_shared<fengine::modules::display::SFMLCircle>(30), std::tuple(0, 0, 255)});
            }
    }
}
