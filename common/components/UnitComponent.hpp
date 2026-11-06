#pragma once

#include <chrono>
#include <utility>

namespace freakytd {
    enum class UnitTeam {
        ENNEMIES,
        ALLIES,
    };

    struct UnitComponent {
        UnitTeam team;
        float health;
        std::pair<float, float> damage;
        std::pair<float, float> armors;
        float speed;

        std::chrono::time_point<std::chrono::steady_clock> timeBeforeAttack;
    };
}
