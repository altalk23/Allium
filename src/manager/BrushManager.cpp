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

bool BrushManager::panEditorInBrush() {
    return m_panEditorInBrush || m_tempPanEditorInBrush;
}

cocos2d::ccColor3B BrushManager::getColor() {
    auto action = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(this->getColorID());
    if (!action) return ccc3(0xff, 0xff, 0xff);
    return action->m_fromColor;
}

int BrushManager::getColorID() {
    return Mod::get()->getSettingValue<int64_t>("brush-color-id");
    // return Mod::get()->getSavedValue<int>("brush-color-id", 1011);
}
float BrushManager::getLineWidth() {
    return Mod::get()->getSettingValue<double>("brush-line-width");
    // return Mod::get()->getSavedValue<float>("brush-line-width", 5.0f);
}
float BrushManager::getCurveRoughness() {
    return Mod::get()->getSettingValue<double>("brush-curve-detail");
    // return Mod::get()->getSavedValue<float>("brush-curve-detail", 0.4f);
}
float BrushManager::getFreeThreshold() {
    return Mod::get()->getSettingValue<double>("brush-free-threshold");
    // return Mod::get()->getSavedValue<float>("brush-free-threshold", 0.4f);
}

void BrushManager::setColorID(int id) {
    Mod::get()->setSavedValue("brush-color-id", id);
}
void BrushManager::setLineWidth(float width) {
    Mod::get()->setSavedValue("brush-line-width", width);
}
void BrushManager::setCurveRoughness(float roughness) {
    Mod::get()->setSavedValue("brush-curve-detail", roughness);
}
void BrushManager::setFreeThreshold(float threshold) {
    Mod::get()->setSavedValue("brush-free-threshold", threshold);
}