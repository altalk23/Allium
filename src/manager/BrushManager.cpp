#include <manager/BrushManager.hpp>

using namespace geode::prelude;
using namespace allium;

BrushManager* BrushManager::get() {
    static BrushManager* instance = nullptr;
    if (!instance) {
        instance = new (std::nothrow) BrushManager();
    }
    return instance;
}
