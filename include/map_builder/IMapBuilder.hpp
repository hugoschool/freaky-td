#pragma once

#include <vector>
#include "levels/Path.hpp"

namespace freakytd {
    class IMapBuilder {
        public:
            virtual ~IMapBuilder() = default;

            virtual void generateTiles() = 0;
            virtual void modifyTilesToPaths(std::vector<Path> &) = 0;
    };
}
