#pragma once

#include "f-engine/asset_manager/IAssetManager.hpp"
#include "f-engine/modules/display/events/ClickEvent.hpp"
#include "f-engine/modules/display/events/EventMouseButton.hpp"
#include "levels/Levels.hpp"
#include "map_builder/IMapBuilder.hpp"
#include "f-engine/modules/display/IDisplayModule.hpp"
#include "f-engine/modules/sound/ISoundModule.hpp"
#include "f-engine/world/World.hpp"
#include "map_builder/IMapBuilder.hpp"
#include <memory>
#include <utility>

namespace freakytd {
    class Orchestrator {
        public:
            Orchestrator(std::string);
            ~Orchestrator();

            void loop();
        private:
            fengine::world::World _world;
            std::unique_ptr<IMapBuilder> _mapBuilder;
            std::shared_ptr<fengine::asset_manager::IAssetManager> _assetManager;
            std::unique_ptr<fengine::modules::display::IDisplayModule> _displayModule;
            std::unique_ptr<fengine::modules::sound::ISoundModule> _soundModule;

            void handleClickEvent(std::pair<float, float> &clickEvent, fengine::modules::display::EventMouseButton mouseButton);

            Levels _levels;
    };
}
