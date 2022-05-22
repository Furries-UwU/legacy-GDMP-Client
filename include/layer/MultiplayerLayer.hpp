#pragma once
#include <charconv>

#include "include.hpp"
#include "global.hpp"

class MultiplayerLayer: public cocos2d::CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void backButtonCallback(CCObject*);
    void connectButtonCallback(CCObject*);

    CCTextInputNode* ipInput;
    CCTextInputNode* portInput;
public:
    static MultiplayerLayer* create();
    void switchToCustomLayerButton(CCObject*);
};