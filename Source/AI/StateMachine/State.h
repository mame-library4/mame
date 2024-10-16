#pragma once

template<class T>
class State
{
public:
    State(T* owner, const char* name) : owner_(owner), name_(name) {}
    virtual ~State() {}

    virtual void Initialize() = 0;
    virtual void Update(const float& elapsedTime) = 0;
    virtual void Finalize() = 0;
    virtual void DrawDebug() = 0;

public:// 取得・設定
    const char* GetName() { return name_; }

protected:
    T* owner_;
    const char* name_;
};