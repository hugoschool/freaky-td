#pragma once

namespace freakytd {
    enum class TileType {
        BASIC,
        PATH,
    };

    struct TileComponent {
        TileType type;
    };
}
