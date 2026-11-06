#include "map_builder/SFMLMapBuilder.hpp"
#include "f-engine/ecs/Registry.hpp"
#include "f-engine/ecs/components/DrawableComponent.hpp"
#include "f-engine/modules/display/shapes/SFMLRectangle.hpp"
#include "f-engine/modules/display/window/IWindow.hpp"
#include <map>
#include <utility>
#include "../common/components/TileComponent.hpp"

freakytd::SFMLMapBuilder::SFMLMapBuilder(fengine::ecs::Registry &registry): _registry(registry) {

}

freakytd::SFMLMapBuilder::~SFMLMapBuilder() {}

constexpr std::size_t width = 50;

void freakytd::SFMLMapBuilder::generateTiles(std::vector<Path> &paths) {
    std::map<std::pair<float, float>, bool> _pathMap;

    for (auto &path: paths) {
        for (auto &tile: path.getTiles()) {
            _pathMap.insert({std::pair(tile.getX() * width, tile.getY() * width), false});
        }
    }

    std::tuple<int, int, int> color = std::tuple(0, 255, 0);

    for (int y = 0; y < WINDOW_Y; y += width) {
        for (int x = 0; x < WINDOW_X; x += width) {
            auto entity = _registry.entityManager.spawnEntity();

            if (_pathMap.contains(std::pair(static_cast<float>(x), static_cast<float>(y)))) {
                _registry.componentManager.addComponent(entity, (TileComponent) {TileType::PATH});
                color = std::tuple(255, 0, 0);
            } else {
                _registry.componentManager.addComponent(entity, (TileComponent) {TileType::BASIC});
                color = std::tuple(0, 255, 0);
            }

            _registry.componentManager.addComponent(entity,
                fengine::ecs::DrawableComponent{
                    .texture_filepath = std::nullopt,
                    .shape = std::make_shared<fengine::modules::display::SFMLRectangle>(width, width),
                    .color = color,
                });
            _registry.componentManager.addComponent(entity,
                fengine::ecs::PositionComponent{
                    .pos_x = static_cast<float>(x),
                    .pos_y = static_cast<float>(y),
                    .can_be_out_window = false,
                });
        }
    }
}
