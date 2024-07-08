#include <Geode/Geode.hpp>

using namespace geode::prelude;
/*
-1 = auto, 0 = NA, 1 = Easy, 2 = Normal
3 = Hard, 4 = Harder, 5 = Insane, 6 = Hard Demon
7 = Easy Demon, 8 = Medium Demon, 9 = Insane Demon, 10 = Extreme Demon
*/
int getDifficulty(GJGameLevel* level){
    if (level->m_autoLevel)
        return -1;

    if (level->m_ratingsSum != 0)
        if (level->m_demon == 1){
            int fixedNum = level->m_demonDifficulty;

            if (fixedNum != 0)
                fixedNum -= 2;

            return 6 + fixedNum;
        }
        else{
            return level->m_ratingsSum / level->m_ratings;
        }
    else 
        return 0;
}

#include <Geode/modify/LevelCell.hpp>
class $modify(MyLevelCell, LevelCell) {
    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelCell::loadFromLevel", -9999);
    }

    void loadFromLevel(GJGameLevel* p0){
        LevelCell::loadFromLevel(p0);

        if (p0->m_levelType == GJLevelType::Editor) return;

        if (getDifficulty(m_level) >= 6){
            CCSprite* s = nullptr;
            if (m_mainLayer->getChildByID("difficulty-container")){
                s = static_cast<CCSprite*>(m_mainLayer->getChildByID("difficulty-container")->getChildByID("difficulty-sprite"));
                if (m_mainLayer->getChildByID("difficulty-container")->getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))
                    m_mainLayer->getChildByID("difficulty-container")->getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite")->setVisible(false);
                if (m_mainLayer->getChildByID("difficulty-container")->getChildByID("gddp-difficulty"))
                    m_mainLayer->getChildByID("difficulty-container")->getChildByID("gddp-difficulty")->setVisible(false);
            }
            else if (m_mainLayer->getChildByID("grd-demon-icon-layer")){
                auto childArr = m_mainLayer->getChildByID("grd-demon-icon-layer")->getChildren();

                s = static_cast<CCSprite*>(childArr->objectAtIndex(childArr->count() - 1));
                if (s->getID() == "grd-infinity"){
                    s->setVisible(false);
                    s = static_cast<CCSprite*>(childArr->objectAtIndex(childArr->count() - 2));
                }
            }

            s->setVisible(false);            

            auto s2 = CCSprite::createWithSpriteFrameName("difficulty_06_btn_001.png");
            s2->setPosition(s->getPosition());

            if (m_mainLayer->getChildByID("difficulty-container")){
                m_mainLayer->getChildByID("difficulty-container")->addChild(s2);
                auto arr = s->getChildren();
                CCSprite* feature = nullptr;

                if (arr)
                    if (arr->count() > 0)
                        feature = static_cast<CCSprite*>(arr->objectAtIndex(0));

                if (feature){
                    auto s2bg = CCSprite::createWithSpriteFrame(feature->displayFrame());
                    s2bg->setZOrder(-1);
                    s2bg->setPosition(s2->getContentSize() / 2);
                    s2->addChild(s2bg);
                }
            }
            else if (m_mainLayer->getChildByID("grd-demon-icon-layer")){
                m_mainLayer->getChildByID("grd-demon-icon-layer")->addChild(s2);
            }
            
        }
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(MyLevelInfoLayer, LevelInfoLayer) {

    struct Fields{
        CCSprite* demon = nullptr;
    };

    static void onModify(auto& self) {
        auto _ = self.setHookPriority("LevelInfoLayer::init", -9999);
        _ = self.setHookPriority("LevelInfoLayer::levelDownloadFinished", -9999);
    }

    bool init(GJGameLevel* level, bool challenge){
        if (!LevelInfoLayer::init(level, challenge)) return false;

        refreshDiff();
        return true;
    }

    void levelDownloadFinished(GJGameLevel* p0){
        LevelInfoLayer::levelDownloadFinished(p0);

        refreshDiff();
    }

    void refreshDiff(){
        if (getDifficulty(m_level) >= 6){
            auto s = static_cast<GJDifficultySprite*>(getChildByID("difficulty-sprite"));
            s->setVisible(false);

            if (getChildByID("grd-difficulty")){
                getChildByID("grd-difficulty")->removeMeAndCleanup();
            }
            if (getChildByID("grd-infinity")){
                getChildByID("grd-infinity")->setVisible(false);
            }

            if (getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))
                getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite")->setVisible(false);

            if (getChildByID("gddp-difficulty"))
                getChildByID("gddp-difficulty")->setVisible(false);

            if (getChildByTag(69420))
                getChildByTag(69420)->setVisible(false);

            CCObject* child;
            CCARRAY_FOREACH(getChildren(), child){
                auto c = dynamic_cast<CCParticleSystemQuad*>(child);
                if (c){
                    c->setPositionY(c->getPositionY() - 7);
                }
            }

            auto arr = s->getChildren();
            CCSprite* feature = nullptr;

            if (arr)
                if (arr->count() > 0)
                    feature = static_cast<CCSprite*>(arr->objectAtIndex(0));

            if (m_fields->demon)
                m_fields->demon->removeMeAndCleanup();

            m_fields->demon = CCSprite::createWithSpriteFrameName("difficulty_06_btn_001.png");
            m_fields->demon->setPosition(s->getPosition() - ccp(0, 10));
            this->addChild(m_fields->demon);

            if (feature){
                auto s2bg = CCSprite::createWithSpriteFrame(feature->displayFrame());
                s2bg->setZOrder(-1);
                s2bg->setPosition(m_fields->demon->getContentSize() / 2);
                m_fields->demon->addChild(s2bg);
            }
        }
    }
};

#include <Geode/modify/LevelSearchLayer.hpp>
class $modify(MyLevelSearchLayer, LevelSearchLayer) {
    bool init(int p0){
        if (!LevelSearchLayer::init(p0)) return false;

        if (Mod::get()->getSettingValue<bool>("remove-filters")){
            demonFilterSelectClosed(0);

            schedule(schedule_selector(MyLevelSearchLayer::myUpdate));
        }  

        return true;
    }

    void myUpdate(float delta){
        if (getChildByID("difficulty-filter-menu")->getChildByID("demon-type-filter-button"))
            if (getChildByID("difficulty-filter-menu")->getChildByID("demon-type-filter-button")->isVisible())
                getChildByID("difficulty-filter-menu")->getChildByID("demon-type-filter-button")->setVisible(false);

        if (getChildByID("difficulty-filter-menu")->getChildByID("hiimjustin000.demons_in_between/quick-search-button"))
            if (getChildByID("difficulty-filter-menu")->getChildByID("hiimjustin000.demons_in_between/quick-search-button")->isVisible())
                getChildByID("difficulty-filter-menu")->getChildByID("hiimjustin000.demons_in_between/quick-search-button")->setVisible(false);
    }
};
