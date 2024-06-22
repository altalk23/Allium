#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    enum class BrushType {
        None,
        Line,
        Curve,
        Free
    };

    class BrushDrawer;

    class BrushManager {
    public:
        static BrushManager* get();

        BrushType m_currentBrush = BrushType::None;
        BrushDrawer* m_currentDrawer = nullptr;
    };
}