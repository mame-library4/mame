#pragma once

class Enemy;

class ActionBase
{
public:
    enum class State
    {
        Run,        // ���s��
        Failed,     // ���s���s
        Complete,   // ���s����
    };

public:
    ActionBase(Enemy* owner) : owner_(owner) {}

    virtual const ActionBase::State Run(const float& elapsedTime) = 0;

protected:
    Enemy*  owner_;
    int     step_ = 0;
};