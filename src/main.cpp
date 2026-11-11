
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(ARTEXMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        log::info("ARTEX MenuLayer loaded!");

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ARTEX"),
            this,
            menu_selector(ARTEXMenuLayer::onARTEX)
        );

        button->setPosition({
            winSize.width - 55.0f,
            45.0f
        });

        menu->addChild(button);
        this->addChild(menu, 100);

        log::info("ARTEX button created!");

        return true;
    }

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
};
