
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace geode::prelude;

class ARTEXPanel : public CCLayer {
    CCMenu* m_menu = nullptr;
    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_status = nullptr;

    int m_page = -1;
    int m_difficulty = 0;
    int m_size = 2;
    int m_count = 250;
    bool m_random = true;

    const char* difficulties[11] = {
        "Easy", "Normal", "Hard", "Harder", "Insane",
        "Demon", "Extreme Demon", "Mega Demon",
        "Giga Demon", "Tera Demon", "Ultra Demon"
    };

    const char* sizes[6] = {
        "Tiny", "Small", "Medium", "Large", "XL", "XXL"
    };

    void resetMenu() {
        if (m_menu) {
            m_menu->removeFromParentAndCleanup(true);
            m_menu = nullptr;
        }

        auto win = CCDirector::sharedDirector()->getWinSize();

        m_menu = CCMenu::create();
        m_menu->setPosition({win.width / 2, win.height / 2});
        this->addChild(m_menu, 10);
    }

    void addButton(
        const char* text,
        float x,
        float y,
        int tag,
        SEL_MenuHandler callback,
        float scale = 0.45f
    ) {
        auto sprite = ButtonSprite::create(text);
        auto button = CCMenuItemSpriteExtra::create(
            sprite, this, callback
        );

        if (!button) return;

        button->setTag(tag);
        button->setScale(scale);
        button->setPosition({x, y});
        m_menu->addChild(button);
    }

    void showMain() {
        m_page = -1;
        resetMenu();

        auto win = CCDirector::sharedDirector()->getWinSize();

        m_title->setString("ARTEX GENERATOR");

        std::string status =
            std::string(difficulties[m_difficulty]) +
            " | " + sizes[m_size] +
            " | " + std::to_string(m_count);

        m_status->setString(status.c_str());

        addButton("Difficulty >", -75, 65, 0,
            menu_selector(ARTEXPanel::onAction));

        addButton("Level Size >", 75, 65, 1,
            menu_selector(ARTEXPanel::onAction));

        addButton("Object Count >", -75, 20, 2,
            menu_selector(ARTEXPanel::onAction));

        addButton(
            m_random ? "Random: ON" : "Random: OFF",
            75, 20, 3,
            menu_selector(ARTEXPanel::onAction)
        );

        addButton("BUILD", 0, -45, 4,
            menu_selector(ARTEXPanel::onAction), 0.65f);

        addButton("CLOSE", 0, -95, 5,
            menu_selector(ARTEXPanel::onAction), 0.45f);
    }

    void showOptions() {
        resetMenu();

        auto win = CCDirector::sharedDirector()->getWinSize();

        if (m_page == 0) {
            m_title->setString("DIFFICULTY");

            for (int i = 0; i < 11; i++) {
                int col = i % 2;
                int row = i / 2;

                float x = col == 0 ? -75.f : 75.f;
                float y = 95.f - row * 30.f;

                addButton(
                    difficulties[i], x, y, i,
                    menu_selector(ARTEXPanel::onPick),
                    0.37f
                );
            }
        }

        if (m_page == 1) {
            m_title->setString("LEVEL SIZE");

            for (int i = 0; i < 6; i++) {
                int col = i % 2;
                int row = i / 2;

                addButton(
                    sizes[i],
                    col == 0 ? -75.f : 75.f,
                    65.f - row * 45.f,
                    i,
                    menu_selector(ARTEXPanel::onPick),
                    0.45f
                );
            }
        }

        if (m_page == 2) {
            m_title->setString("OBJECT COUNT");

            const char* counts[] = {
                "100", "250", "500", "1000", "1500"
            };

            for (int i = 0; i < 5; i++) {
                addButton(
                    counts[i],
                    0,
                    75.f - i * 35.f,
                    i,
                    menu_selector(ARTEXPanel::onPick),
                    0.45f
                );
            }
        }

        addButton("BACK", 0, -115, 100,
            menu_selector(ARTEXPanel::onBack), 0.5f);
    }

    void buildLevel() {
        auto editor = LevelEditorLayer::get();

        if (!editor) {
            m_status->setString("Editor not found!");
            return;
        }

        int amount = m_count;

        // Ограничение защищает от случайной огромной генерации.
        amount = std::min(amount, 1500);

        float startX = std::max(
            120.f,
            editor->getLastObjectX() + 60.f
        );

        int created = 0;

        // Сложность увеличивает частоту препятствий.
        int spikeChance = 2 + m_difficulty;

        for (int i = 0; i < amount; i++) {
            int pattern = m_random ? rand() % 6 : i % 6;

            float x = startX + i * 30.f;
            float y = 90.f;

            int objectID = 1;

            // Базовый пол.
            if (pattern == 0 || pattern == 1) {
                objectID = 1;
                y = 90.f;
            }

            // Шипы.
            else if (pattern == 2 || pattern == 3) {
                objectID = 8;
                y = 120.f;
            }

            // Повышенные платформы.
            else if (pattern == 4) {
                objectID = 1;
                y = 120.f;
            }

            // Дополнительные препятствия на высокой сложности.
            else {
                objectID = (rand() % 100 < spikeChance * 7) ? 8 : 1;
                y = objectID == 8 ? 120.f : 90.f;
            }

            auto object = editor->createObject(
                objectID,
                {x, y},
                false
            );

            if (object) {
                created++;
            }
        }

        std::string message =
            "Created: " + std::to_string(created);

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

        srand(static_cast<unsigned int>(time(nullptr)));

        auto win = CCDirector::sharedDirector()->getWinSize();

        float width = std::min(340.f, win.width - 24.f);
        float height = std::min(270.f, win.height - 24.f);

        auto background = CCLayerColor::create(
            ccc4(24, 29, 45, 245),
            width,
            height
        );

        background->setPosition({
            (win.width - width) / 2,
            (win.height - height) / 2
        });

        this->addChild(background);

        m_title = CCLabelBMFont::create(
            "ARTEX GENERATOR",
            "bigFont.fnt"
        );

        m_title->setScale(0.5f);
        m_title->setPosition({
            win.width / 2,
            win.height / 2 + height / 2 - 22.f
        });

        this->addChild(m_title, 5);

        m_status = CCLabelBMFont::create(
            "Ready",
            "bigFont.fnt"
        );

        m_status->setScale(0.32f);
        m_status->setPosition({
            win.width / 2,
            win.height / 2 - height / 2 + 14.f
        });

        this->addChild(m_status, 5);

        showMain();
        return true;
    }

    void onAction(CCObject* sender) {
        int tag = static_cast<CCNode*>(sender)->getTag();

        if (tag >= 0 && tag <= 2) {
            m_page = tag;
            showOptions();
        }
        else if (tag == 3) {
            m_random = !m_random;
            showMain();
        }
        else if (tag == 4) {
            buildLevel();
        }
        else if (tag == 5) {
            this->removeFromParentAndCleanup(true);
        }
    }

    void onPick(CCObject* sender) {
        int value = static_cast<CCNode*>(sender)->getTag();

        if (m_page == 0 && value >= 0 && value < 11) {
            m_difficulty = value;
        }
        else if (m_page == 1 && value >= 0 && value < 6) {
            m_size = value;

            const int amounts[] = {
                100, 150, 250, 500, 1000, 1500
            };

            m_count = amounts[value];
        }
        else if (m_page == 2) {
            const int amounts[] = {
                100, 250, 500, 1000, 1500
            };

            if (value >= 0 && value < 5) {
                m_count = amounts[value];
            }
        }

        showMain();
    }

    void onBack(CCObject*) {
        showMain();
    }
};

class $modify(ARTEXEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto win = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("A"),
            this,
            menu_selector(ARTEXEditorUI::onARTEX)
        );

        button->setPosition({
            win.width - 35.f,
            win.height - 35.f
        });

        button->setScale(0.75f);

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
