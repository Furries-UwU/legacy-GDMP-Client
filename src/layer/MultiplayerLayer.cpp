#include "layer/MultiplayerLayer.hpp"

USE_GEODE_NAMESPACE();

MultiplayerLayer* MultiplayerLayer::create() {
    auto ret = new MultiplayerLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        // if `new` or `init` went wrong, delete the object and return a nullptr
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool MultiplayerLayer::init() {
    auto director = CCDirector::sharedDirector();

    auto label = CCLabelBMFont::create("Multiplayer", "bigFont.fnt");
    label->setPosition(ccp(director->getWinSize().width / 2,
                           director->getWinSize().height - 25));
    addChild(label);

    auto input = CCTextInputNode::create(500, 500, "Enter DA IP", "bigFont.fnt");
    input->setPosition(ccp(director->getWinSize().width / 2,
                           director->getWinSize().height / 2));
    input->setAllowedChars("0123456789.:");
    addChild(input);

    auto backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = backgroundSprite->getContentSize();
    
    backgroundSprite->setScaleX(winSize.width / size.width);
    backgroundSprite->setScaleY(winSize.height / size.height);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({0, 0, 0});
    
    backgroundSprite->setZOrder(-1);
    addChild(backgroundSprite);

    auto button = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(MultiplayerLayer::backButtonCallback)
    );

    auto menu = CCMenu::create();
    menu->addChild(button);
    menu->setPosition({25, winSize.height - 25});

    addChild(menu);
    setKeypadEnabled(true);

    return true;
}

void exitMenu() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void MultiplayerLayer::keyBackClicked() {
    exitMenu();
}

void MultiplayerLayer::backButtonCallback(CCObject* object) {
    exitMenu();
}

void MultiplayerLayer::switchToCustomLayerButton(CCObject* object) {
    auto layer = MultiplayerLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);

    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}