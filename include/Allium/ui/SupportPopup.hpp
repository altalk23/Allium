#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    class SupportPopup : public FLAlertLayer {
    public:
        static SupportPopup* create();
        bool init() override;

        void keyBackClicked() override;
        void onClose(cocos2d::CCObject* sender);
    };
}