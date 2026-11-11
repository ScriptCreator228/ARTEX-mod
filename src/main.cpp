
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(ARTEXMenuLayer, MenuLayer) {

    void onARTEX(CCObject*) {
        FLAlertLayer::create(
            "ARTEX - Auto Build",
            "Difficulty\n"
            "Blocks & Spikes\n"
            "Portals\n"
            "Gamemodes\n"
            "Details\n"
            "Level Size",
            "CLOSE"
        )->show();
    }

    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ARTEX"),
            this,
            menu_selector(ARTEXMenuLayer::onARTEX)
        );

        button->setPosition({
            CCDirector::sharedDirector()->getWinSize().width - 55,
            45
        });

        menu->addChild(button);
        this->addChild(menu, 100);

        return true;
    }
};
