#pragma once

#include "f-engine/ecs/Registry.hpp"
#include "levels/Path.hpp"
#include "map_builder/IMapBuilder.hpp"

namespace freakytd {
    class SFMLMapBuilder: public IMapBuilder {
        public:
            SFMLMapBuilder(fengine::ecs::Registry &);
            ~SFMLMapBuilder();

            void generateTiles() override;
            void modifyTilesToPaths(std::vector<Path> &vec) override;
        private:
            fengine::ecs::Registry &_registry;
    };
}
