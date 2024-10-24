#pragma once

class Enemy;

class ActionBase
{
public:
    enum class State
    {
        Run,        // 実行中
        Failed,     // 実行失敗
        Complete,   // 実行成功
    };

public:
    ActionBase(Enemy* owner) : owner_(owner) {}

    virtual const ActionBase::State Run(const float& elapsedTime) = 0;

protected:
    Enemy*  owner_;
    int     step_ = 0;
};