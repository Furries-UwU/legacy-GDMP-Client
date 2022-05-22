#pragma once
#include "include.hpp"

class MultiplayerLayer: public cocos2d::CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void backButtonCallback(CCObject*);
public:
    static MultiplayerLayer* create();
    void switchToCustomLayerButton(CCObject*);
};