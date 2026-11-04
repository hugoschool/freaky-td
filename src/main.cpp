#include "Orchestrator.hpp"
#include <exception>
#include <f-engine/ecs/Registry.hpp>
#include <f-engine/modules/display/SFMLDisplayModule.hpp>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Missing level argument" << std::endl;
        return 1;
    }

    try {
        freakytd::Orchestrator orch(argv[1]);
        orch.loop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // auto &levelsVec = levels.getLevels();

    // std::cout << levelsVec.size() << std::endl;

    // auto level = levels.getCurrentLevel();

    // if (!level.has_value()) {
    //     return 1;
    // }

    // auto &paths = level.value().get().getPaths();

    // std::cout << paths.size() << std::endl;

    // if (paths.size() > 0) {
    //     std::cout << level.value().get().getPaths()[0].getStart().getX() << std::endl;
    //     std::cout << level.value().get().getPaths()[0].getStart().getY() << std::endl;
    // }

    // std::cout << level.value().get().getPlayerHP() << std::endl;

    // TODO: to be removed
    // exit(0);
}
