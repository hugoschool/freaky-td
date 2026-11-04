#include "f-engine/asset_manager/SFMLAssetManager.hpp"
#include "f-engine/camera/Camera.hpp"
#include "f-engine/ecs/components/DrawableComponent.hpp"
#include "f-engine/ecs/components/PositionComponent.hpp"
#include "f-engine/modules/display/shapes/SFMLRectangle.hpp"
#include "f-engine/modules/display/window/IWindow.hpp"
#include "levels/Levels.hpp"
#include <cstddef>
#include <f-engine/ecs/Registry.hpp>
#include <f-engine/modules/display/SFMLDisplayModule.hpp>
#include <iostream>
#include <memory>

void generateTiles(fengine::ecs::Registry &registry) {
    constexpr std::size_t width = 50;

    for (int y = 0; y < WINDOW_Y; y += width) {
        for (int x = 0; x < WINDOW_X; x += width) {
            auto entity = registry.entityManager.spawnEntity();

            registry.componentManager.addComponent(entity,
                fengine::ecs::DrawableComponent{
                    .texture_filepath = std::nullopt,
                    .shape = std::make_shared<fengine::modules::display::SFMLRectangle>(width, width),
                    .color = std::tuple(x, 0, 0),
                });
            registry.componentManager.addComponent(entity,
                fengine::ecs::PositionComponent{
                    .pos_x = static_cast<float>(x),
                    .pos_y = static_cast<float>(y),
                    .can_be_out_window = false,
                });
        }
    }
}

void drawTiles(fengine::ecs::Registry &registry,
    std::shared_ptr<fengine::modules::display::IDisplayModule> displayModule) {
    for (auto &entity : registry.entityManager.getEntities()) {
        if (!entity.has_value())
            continue;
        const std::size_t entityId = entity->getId();

        auto &drawables = registry.componentManager.getComponents<fengine::ecs::DrawableComponent>();
        auto &positions = registry.componentManager.getComponents<fengine::ecs::PositionComponent>();

        displayModule->drawEntity(positions[entityId].value(), drawables[entityId].value());
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Missing level argument" << std::endl;
        return 1;
    }

    freakytd::Levels levels;

    try {
        levels.parseFile(argv[1]);
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    auto &levelsVec = levels.getLevels();

    std::cout << levelsVec.size() << std::endl;

    auto level = levels.getCurrentLevel();

    if (!level.has_value()) {
        return 1;
    }

    auto &paths = level.value().get().getPaths();

    std::cout << paths.size() << std::endl;

    if (paths.size() > 0) {
        std::cout << level.value().get().getPaths()[0].getStart().getX() << std::endl;
        std::cout << level.value().get().getPaths()[0].getStart().getY() << std::endl;
    }

    std::cout << level.value().get().getPlayerHP() << std::endl;

    // TODO: to be removed
    exit(0);

    try {
        fengine::ecs::Registry registry = fengine::ecs::Registry();

        registry.componentManager.registerComponent<fengine::ecs::DrawableComponent>();
        registry.componentManager.registerComponent<fengine::ecs::PositionComponent>();

        generateTiles(registry);

        std::shared_ptr<fengine::asset_manager::SFMLAssetManager> _assetManager =
            std::make_shared<fengine::asset_manager::SFMLAssetManager>();

        // TODO: change title of window
        std::shared_ptr<fengine::modules::display::IDisplayModule> _displayModule =
            std::make_shared<fengine::modules::display::SFMLDisplayModule>(_assetManager);

        fengine::camera::Camera _camera(0.0, 100000);

        bool exit = false;
        while (!exit) {
            _displayModule->clear();

            registry.systemManager.update(registry);

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
                }
                if (exit == true) {
                    break;
                }
            }

            drawTiles(registry, _displayModule);
        }
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
