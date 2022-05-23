#include "layer/MultiplayerLayer.hpp"

USE_GEODE_NAMESPACE();

// 23973 - default port

void MultiplayerLayer::connectButtonCallback(CCObject* object) {
    Global *global = Global::get();

    if (global->peer != nullptr) enet_peer_disconnect_now(global->peer, {});

    ENetAddress address;
    enet_address_set_host(&address, std::string(ipInput->getString()).c_str());
    address.port = atoi(portInput->getString());

    global->peer = enet_host_connect(global->host, &address, 1, 0);
    if (global->peer == nullptr) {
        fmt::print(stderr,
                   "No available peers for initiating an ENet connection.\n");
        return;
    }

    global->isConnected = true;
}

MultiplayerLayer* MultiplayerLayer::create() {
    auto ret = new MultiplayerLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool MultiplayerLayer::init() {
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    auto label = CCLabelBMFont::create("Multiplayer", "bigFont.fnt");
    label->setPosition(ccp(director->getWinSize().width / 2,
                           director->getWinSize().height - 25));
    addChild(label);

    ipInput = CCTextInputNode::create(100, 100, "IP Address", "bigFont.fnt");
    ipInput->setPosition(ccp((director->getWinSize().width / 2 )-150,
                           director->getWinSize().height / 2));
    ipInput->setAllowedChars("0123456789.");
    ipInput->setString("127.0.0.1");
    addChild(ipInput);

    portInput = CCTextInputNode::create(100, 100, "Port", "bigFont.fnt");
    portInput->setPosition(ccp((director->getWinSize().width / 2)+150,
                           director->getWinSize().height / 2));
    portInput->setAllowedChars("0123456789");
    portInput->setString("23973");
    addChild(portInput);

    auto confirmButtonSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    confirmButtonSprite->setScaleX(0.5);
    confirmButtonSprite->setScaleY(0.5);

    auto confirmButton = CCMenuItemSpriteExtra::create(
        confirmButtonSprite,
        this,
        menu_selector(MultiplayerLayer::connectButtonCallback)
    );

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(MultiplayerLayer::backButtonCallback)
    );

    auto backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    auto backgroundSize = backgroundSprite->getContentSize();

    backgroundSprite->setScaleX(winSize.width / backgroundSize.width);
    backgroundSprite->setScaleY(winSize.height / backgroundSize.height);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({0, 0, 0});
    
    backgroundSprite->setZOrder(-1);
    addChild(backgroundSprite);

    auto menu = CCMenu::create();
    menu->addChild(backButton);
    menu->setPosition({25, winSize.height - 25});
    addChild(menu);

    menu = CCMenu::create();
    menu->addChild(confirmButton);
    menu->setPosition({winSize.width / 2, winSize.height / 2 - 55});
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