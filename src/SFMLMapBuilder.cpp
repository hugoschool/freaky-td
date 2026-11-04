#include "map_builder/SFMLMapBuilder.hpp"
#include "f-engine/ecs/Registry.hpp"
#include "f-engine/ecs/components/DrawableComponent.hpp"
#include "f-engine/modules/display/shapes/SFMLRectangle.hpp"
#include "f-engine/modules/display/window/IWindow.hpp"

freakytd::SFMLMapBuilder::SFMLMapBuilder(fengine::ecs::Registry &registry): _registry(registry) {

}

freakytd::SFMLMapBuilder::~SFMLMapBuilder() {}

constexpr std::size_t width = 50;

void freakytd::SFMLMapBuilder::generateTiles() {

    for (int y = 0; y < WINDOW_Y; y += width) {
        for (int x = 0; x < WINDOW_X; x += width) {
            auto entity = _registry.entityManager.spawnEntity();

            _registry.componentManager.addComponent(entity,
                fengine::ecs::DrawableComponent{
                    .texture_filepath = std::nullopt,
                    .shape = std::make_shared<fengine::modules::display::SFMLRectangle>(width, width),
                    .color = std::tuple(0, 255, 0),
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

void freakytd::SFMLMapBuilder::modifyTilesToPaths(std::vector<freakytd::Path> &paths) {
    auto &positions = _registry.componentManager.getComponents<fengine::ecs::PositionComponent>();
    auto &drawables = _registry.componentManager.getComponents<fengine::ecs::DrawableComponent>();

    for (std::size_t i = 0; i < _registry.entityManager.size(); i++) {
        auto &pos = positions[i];
        if (pos.has_value()) {
            for (auto &path: paths) {
                for (auto &tile: path.getTiles()) {
                    if (tile.getX() * width == pos.value().pos_x && tile.getY() * width == pos.value().pos_y) {
                        auto &draw = drawables[i];
                        if (draw.has_value()) {
                            draw.value().color = std::tuple(255, 0, 0);
                        }
                    }
                }
            }
        }
    }
}
