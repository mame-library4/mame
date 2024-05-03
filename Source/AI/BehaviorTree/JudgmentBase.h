#pragma once

class Enemy;

class JudgmentBase
{
public:
    JudgmentBase(Enemy* owner) : owner_(owner) {}
    virtual const bool Judgment() = 0;

protected:
    Enemy* owner_;
};