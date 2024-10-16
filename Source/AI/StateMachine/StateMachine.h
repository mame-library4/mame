#pragma once

#include <vector>
#include "State.h"

template <class T>
class StateMachine
{
public:
    StateMachine() {};
    ~StateMachine();

    void Update(float elapsedTime); // 更新処理

    void DrawDebug();               // ImGui用

    void SetState(int newState);        // ステートセット
    void ChangeState(int newState);     // ステート変更
    void RegisterState(T* state);       // ステート登録

public:// 取得・設定
    int GetStateIndex();                // 現在のステート番号取得
    T* GetCurrentState() { return currentState; } // 現在のステート取得

private:
    T* currentState;            // 現在のステート
    std::vector<T*> statePool;  // 各ステートを保持する配列
};

template<class T>
inline StateMachine<T>::~StateMachine()
{
    // 登録したステートを削除する
    for (T* state : statePool)
    {
        delete state;
    }
    statePool.clear();
}

template<class T>
inline void StateMachine<T>::Update(float elapsedTime)
{
    currentState->Update(elapsedTime);
}

template<class T>
inline void StateMachine<T>::DrawDebug()
{
    static int state = 0;
    if (ImGui::TreeNode("stateMachine"))
    {
        currentState->DrawDebug();

        ImGui::TreePop();
    }
}

template<class T>
inline void StateMachine<T>::SetState(int newState)
{
    currentState = statePool.at(newState);
    currentState->Initialize();
}

template<class T>
inline void StateMachine<T>::ChangeState(int newState)
{
    currentState->Finalize();
    SetState(newState);
}

template<class T>
inline void StateMachine<T>::RegisterState(T* state)
{
    statePool.emplace_back(state);
}

template<class T>
inline int StateMachine<T>::GetStateIndex()
{
    int i = 0;
    for (T* state : statePool)
    {
        if (state == currentState)
        {
            // i番号目のステートをリターン
            return i;
        }
        ++i;
    }

    // ステートが見つからなかったとき
    return -1;
}
