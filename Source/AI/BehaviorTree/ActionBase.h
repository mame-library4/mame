#pragma once

class Enemy;

class ActionBase
{
public:
    enum class State
    {
        Run,        // é¿çsíÜ
        Failed,     // é¿çsé∏îs
        Complete,   // é¿çsê¨å˜
    };

public:
    ActionBase(Enemy* owner) : owner_(owner) {}

    virtual const ActionBase::State Run(const float& elapsedTime) = 0;

protected:
    Enemy*  owner_;
    int     step_ = 0;
};