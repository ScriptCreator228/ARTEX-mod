
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(ARTEXEditorUI, EditorUI) {

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ARTEX"),
            this,
            menu_selector(ARTEXEditorUI::onARTEX)
        );

        button->setPosition({
            winSize.width - 55.0f,
            winSize.height - 45.0f
        });

        menu->addChild(button);
        this->addChild(menu, 100);

        return true;
    }

    void onARTEX(CCObject*) {
        FLAlertLayer::create(
            "ARTEX - Auto Build",
            "Difficulty >\n"
            "Blocks & Spikes >\n"
            "Portals >\n"
            "Gamemodes >\n"
            "Details >\n"
            "Level Size >",
            "CLOSE"
        )->show();
    }
};
