
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace geode::prelude;

class ARTEXPanel : public CCLayer {
    CCMenu* m_menu = nullptr;
    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_status = nullptr;

    int m_page = -1;
    std::string m_selected[7];

    static const char* const* getOptions(int page, int& count) {
        static const char* difficulty[] = {
            "Easy", "Normal", "Hard", "Harder", "Insane",
            "Demon", "Extreme Demon", "Mega Demon",
            "Giga Demon", "Tera Demon", "Ultra Demon"
        };

        static const char* blocks[] = {
            "Classic", "Modern", "Glow", "Mixed"
        };

        static const char* portals[] = {
            "None", "Speed", "Gravity", "Dual",
            "Mini", "UFO", "Wave", "Robot",
            "Spider", "Ball", "Ship", "Mixed"
        };

        static const char* modes[] = {
            "Cube", "Ship", "Ball", "UFO",
            "Wave", "Robot", "Spider", "Swing", "Mixed"
        };

        static const char* details[] = {
            "Low", "Medium", "High", "Ultra"
        };

        static const char* sizes[] = {
            "Tiny", "Small", "Medium", "Large", "XL"
        };

        static const char* counts[] = {
            "100", "250", "500", "1000"
        };

        switch (page) {
            case 0: count = 11; return difficulty;
            case 1: count = 4; return blocks;
            case 2: count = 12; return portals;
            case 3: count = 9; return modes;
            case 4: count = 4; return details;
            case 5: count = 5; return sizes;
            case 6: count = 4; return counts;
            default: count = 0; return nullptr;
        }
    }

    static const char* pageName(int page) {
        static const char* names[] = {
            "Difficulty", "Blocks & Spikes", "Portals",
            "Gamemodes", "Details", "Level Size", "Object Count"
        };

        return page >= 0 && page < 7 ? names[page] : "AUTO BUILD";
    }

    void beginMenu() {
        if (m_menu) {
            m_menu->removeFromParentAndCleanup(true);
        }

        auto size = CCDirector::sharedDirector()->getWinSize();

        m_menu = CCMenu::create();
        m_menu->setPosition({
            size.width / 2,
            size.height / 2
        });

        this->addChild(m_menu, 10);
    }

    void addButton(
        const std::string& label,
        float x,
        float y,
        int tag,
        SEL_MenuHandler callback,
        float scale = 0.48f
    ) {
        auto sprite = ButtonSprite::create(label.c_str());

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            callback
        );

        if (!button) return;

        button->setTag(tag);
        button->setScale(scale);
        button->setPosition({x, y});

        m_menu->addChild(button);
    }

    void addClose(float width, float height) {
        addButton(
            "X",
            width / 2 - 22,
            height / 2 - 20,
            101,
            menu_selector(ARTEXPanel::onClose),
            0.42f
        );
    }

    void showMain() {
        m_page = -1;
        beginMenu();

        auto size = CCDirector::sharedDirector()->getWinSize();
        float width = std::min(360.f, size.width - 20.f);
        float height = std::min(290.f, size.height - 20.f);

        m_title->setString("ARTEX - AUTO BUILD");
        m_status->setString("Choose settings");

        const char* names[] = {
            "Difficulty", "Blocks & Spikes", "Portals",
            "Gamemodes", "Details", "Level Size", "Object Count"
        };

        for (int i = 0; i < 7; i++) {
            std::string label = names[i];
            label += ": ";
            label += m_selected[i];
            label += " >";

            addButton(
                label,
                0,
                height / 2 - 55 - i * 23,
                i,
                menu_selector(ARTEXPanel::onMainAction),
                0.43f
            );
        }

        addButton(
            "BUILD",
            0,
            -height / 2 + 43,
            100,
            menu_selector(ARTEXPanel::onMainAction),
            0.65f
        );

        addClose(width, height);
    }

    void showPage(int page) {
        m_page = page;
        beginMenu();

        auto size = CCDirector::sharedDirector()->getWinSize();
        float height = std::min(290.f, size.height - 20.f);

        m_title->setString(pageName(page));
        m_status->setString("Select an option");

        int count = 0;
        auto options = getOptions(page, count);

        for (int i = 0; i < count; i++) {
            int column = i % 2;
            int row = i / 2;

            float x = column == 0 ? -85.f : 85.f;
            float y = height / 2 - 65.f - row * 26.f;

            addButton(
                options[i],
                x,
                y,
                i,
                menu_selector(ARTEXPanel::onPick),
                0.43f
            );
        }

        addButton(
            "BACK",
            0,
            -height / 2 + 43,
            200,
            menu_selector(ARTEXPanel::onBack),
            0.55f
        );

        addClose(360.f, height);
    }

    void buildLevel() {
        auto editor = LevelEditorLayer::get();

        if (!editor) {
            m_status->setString("Editor not found!");
            return;
        }

        int count = std::atoi(m_selected[6].c_str());

        if (count <= 0) {
            m_status->setString("Invalid object count");
            return;
        }

        float startX = std::max(
            120.f,
            editor->getLastObjectX() + 60.f
        );

        int created = 0;

        for (int i = 0; i < count; i++) {
            bool spike = (i % 5 == 4);

            int objectID = spike ? 8 : 1;

            float x = startX + i * 30.f;
            float y = spike ? 120.f : 90.f;

            auto object = editor->createObject(
                objectID,
                {x, y},
                false
            );

            if (object) {
                created++;
            }
        }

        std::string message = "Created: ";
        message += std::to_string(created);
        message += " objects";

        m_status->setString(message.c_str());
    }

public:
    static ARTEXPanel* create() {
        auto ret = new ARTEXPanel();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;

        m_selected[0] = "Easy";
        m_selected[1] = "Mixed";
        m_selected[2] = "Mixed";
        m_selected[3] = "Cube";
        m_selected[4] = "Medium";
        m_selected[5] = "Medium";
        m_selected[6] = "100";

        auto size = CCDirector::sharedDirector()->getWinSize();

        float width = std::min(360.f, size.width - 20.f);
        float height = std::min(290.f, size.height - 20.f);

        auto background = CCLayerColor::create(
            ccc4(24, 29, 45, 245),
            width,
            height
        );

        background->setPosition({
            (size.width - width) / 2,
            (size.height - height) / 2
        });

        this->addChild(background);

        m_title = CCLabelBMFont::create(
            "ARTEX - AUTO BUILD",
            "bigFont.fnt"
        );

        m_title->setScale(0.55f);
        m_title->setPosition({
            size.width / 2,
            size.height / 2 + height / 2 - 23
        });

        this->addChild(m_title, 5);

        m_status = CCLabelBMFont::create(
            "Choose settings",
            "bigFont.fnt"
        );

        m_status->setScale(0.36f);
        m_status->setPosition({
            size.width / 2,
            size.height / 2 - height / 2 + 15
        });

        this->addChild(m_status, 5);

        showMain();
        return true;
    }

    void onMainAction(CCObject* sender) {
        int tag = static_cast<CCNode*>(sender)->getTag();

        if (tag == 100) {
            buildLevel();
            return;
        }

        if (tag >= 0 && tag < 7) {
            showPage(tag);
        }
    }

    void onPick(CCObject* sender) {
        int count = 0;
        auto options = getOptions(m_page, count);

        int index = static_cast<CCNode*>(sender)->getTag();

        if (!options || index < 0 || index >= count) return;

        m_selected[m_page] = options[index];
        showMain();
    }

    void onBack(CCObject*) {
        showMain();
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }
};

class $modify(ARTEXEditorUI, EditorUI) {

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ARTEX"),
            this,
            menu_selector(ARTEXEditorUI::onARTEX)
        );

        button->setPosition({
            size.width - 55.f,
            size.height - 45.f
        });

        menu->addChild(button);
        this->addChild(menu, 100);

        return true;
    }

    void onARTEX(CCObject*) {
        if (this->getChildByTag(73192)) return;

        auto panel = ARTEXPanel::create();

        if (panel) {
            panel->setTag(73192);
            this->addChild(panel, 9999);
        }
    }
};
