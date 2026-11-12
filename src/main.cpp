
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <string>

using namespace geode::prelude;

class ARTEXPanel : public CCLayer {
protected:
    CCMenu* m_menu = nullptr;
    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_status = nullptr;
    int m_page = -1;
    std::string m_selected[7];

    static const char* const* optionsForPage(int page, int& count) {
        static const char* const difficulty[] = {
            "Easy", "Normal", "Hard", "Harder", "Insane",
            "Demon", "Extreme Demon", "Mega Demon",
            "Giga Demon", "Tera Demon", "Ultra Demon"
        };

        static const char* const blocks[] = {
            "Classic", "Modern", "Glow", "Mixed"
        };

        static const char* const portals[] = {
            "None", "Speed", "Gravity", "Dual",
            "Mini", "UFO", "Wave", "Robot",
            "Spider", "Ball", "Ship", "Mixed"
        };

        static const char* const gamemodes[] = {
            "Cube", "Ship", "Ball", "UFO",
            "Wave", "Robot", "Spider", "Swing", "Mixed"
        };

        static const char* const details[] = {
            "Low", "Medium", "High", "Ultra"
        };

        static const char* const sizes[] = {
            "Tiny", "Small", "Medium", "Large", "XL"
        };

        static const char* const counts[] = {
            "100", "500", "1000", "2500",
            "5000", "10000"
        };

        switch (page) {
            case 0:
                count = 11;
                return difficulty;
            case 1:
                count = 4;
                return blocks;
            case 2:
                count = 12;
                return portals;
            case 3:
                count = 9;
                return gamemodes;
            case 4:
                count = 4;
                return details;
            case 5:
                count = 5;
                return sizes;
            case 6:
                count = 6;
                return counts;
            default:
                count = 0;
                return nullptr;
        }
    }

    static const char* pageName(int page) {
        switch (page) {
            case 0: return "Difficulty";
            case 1: return "Blocks & Spikes";
            case 2: return "Portals";
            case 3: return "Gamemodes";
            case 4: return "Details";
            case 5: return "Level Size";
            case 6: return "Object Count";
            default: return "Auto Build";
        }
    }

    void beginMenu() {
        if (m_menu) {
            m_menu->removeFromParentAndCleanup(true);
            m_menu = nullptr;
        }

        m_menu = CCMenu::create();
        m_menu->setPosition({0, 0});
        this->addChild(m_menu, 10);
    }

    void addButton(
        const std::string& label,
        float x,
        float y,
        int tag,
        SEL_MenuHandler callback,
        float scale = 0.62f
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

    void addCloseButton(float cx, float cy) {
        addButton(
            "X", cx + 207, cy + 158, 101,
            menu_selector(ARTEXPanel::onClose),
            0.55f
        );
    }

    void showMain() {
        m_page = -1;
        beginMenu();

        auto size = CCDirector::sharedDirector()->getWinSize();
        float cx = size.width / 2;
        float cy = size.height / 2;

        m_title->setString("ARTEX - AUTO BUILD");
        m_status->setString("Choose settings and press BUILD");

        const char* names[] = {
            "Difficulty",
            "Blocks & Spikes",
            "Portals",
            "Gamemodes",
            "Details",
            "Level Size",
            "Object Count"
        };

        for (int i = 0; i < 7; i++) {
            std::string label = names[i];
            label += ": ";
            label += m_selected[i];
            label += " >";

            addButton(
                label,
                cx,
                cy + 101 - i * 31,
                i,
                menu_selector(ARTEXPanel::onMainAction),
                0.58f
            );
        }

        addButton(
            "BUILD",
            cx,
            cy - 137,
            100,
            menu_selector(ARTEXPanel::onMainAction),
            0.75f
        );

        addCloseButton(cx, cy);
    }

    void showPage(int page) {
        m_page = page;
        beginMenu();

        auto size = CCDirector::sharedDirector()->getWinSize();
        float cx = size.width / 2;
        float cy = size.height / 2;

        m_title->setString(pageName(page));
        m_status->setString("Select an option");

        int count = 0;
        auto options = optionsForPage(page, count);

        for (int i = 0; i < count; i++) {
            int column = i % 2;
            int row = i / 2;

            float x = cx + (column == 0 ? -108.0f : 108.0f);
            float y = cy + 91.0f - row * 36.0f;

            addButton(
                options[i],
                x,
                y,
                i,
                menu_selector(ARTEXPanel::onPick),
                0.53f
            );
        }

        addButton(
            "BACK",
            cx,
            cy - 145,
            200,
            menu_selector(ARTEXPanel::onBack),
            0.65f
        );

        addCloseButton(cx, cy);
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
        m_selected[6] = "1000";

        auto size = CCDirector::sharedDirector()->getWinSize();
        float cx = size.width / 2;
        float cy = size.height / 2;

        auto background = CCLayerColor::create(
            ccc4(25, 30, 48, 245),
            470,
            390
        );

        background->ignoreAnchorPointForPosition(false);
        background->setAnchorPoint({0.5f, 0.5f});
        background->setPosition({cx, cy});

        this->addChild(background);

        m_title = CCLabelBMFont::create(
            "ARTEX - AUTO BUILD",
            "bigFont.fnt"
        );
        m_title->setScale(0.58f);
        m_title->setPosition({cx, cy + 158});
        this->addChild(m_title, 5);

        m_status = CCLabelBMFont::create(
            "Choose settings and press BUILD",
            "bigFont.fnt"
        );
        m_status->setScale(0.38f);
        m_status->setPosition({cx, cy - 174});
        this->addChild(m_status, 5);

        showMain();
        return true;
    }

    void onMainAction(CCObject* sender) {
        int tag = static_cast<CCNode*>(sender)->getTag();

        if (tag == 100) {
            m_status->setString(
                "Generator is not connected yet"
            );
            return;
        }

        if (tag >= 0 && tag < 7) {
            showPage(tag);
        }
    }

    void onPick(CCObject* sender) {
        if (m_page < 0 || m_page > 6) return;

        int index = static_cast<CCNode*>(sender)->getTag();

        int count = 0;
        auto options = optionsForPage(m_page, count);

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
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ARTEX"),
            this,
            menu_selector(ARTEXEditorUI::onARTEX)
        );

        button->setPosition({
            size.width - 55.0f,
            size.height - 45.0f
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
