#include "EnemyManager.h"
#include "../Other/Common.h"

// ----- 初期化 -----
void EnemyManager::Initialize()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Initialize();
    }
}

// ----- 終了化 -----
void EnemyManager::Finalize()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Finalize();
    }
    Clear();
}

// ----- 更新 -----
void EnemyManager::Update(const float& elapsedTime)
{
    // 生成
    for (Enemy* enemy : generates_)
    {
        enemies_.emplace_back(enemy);
        enemy->Initialize();
    }
    generates_.clear();

    // 更新
    for (Enemy*& enemy : enemies_)
    {
        enemy->Update(elapsedTime);
    }

    // 破棄
    for (Enemy* enemy : removes_)
    {
        // vectorから要素を削除する場合はイテレーターで削除
        auto it = std::find(enemies_.begin(), enemies_.end(), enemy);

        // std::vectorで管理されている要素を削除するにはerase()関数を使用する
        // (破棄リストのポインタからイテレーターを検索し、erase関数に渡す)
        if (it != enemies_.end())
        {
            enemies_.erase(it);
        }

        // 敵の破棄処理
        SafeDeletePtr(enemy);        
    }
    // 破棄リストをクリア
    removes_.clear();
}

// ----- 描画 -----
void EnemyManager::Render()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Render();
    }
}

// ----- ImGui用 -----
void EnemyManager::DrawDebug()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->DrawDebug();
    }
}

// ----- Debug用 -----
void EnemyManager::DebugRender(DebugRenderer* debugRenderer)
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->DebugRender(debugRenderer);
    }
}

// ----- 登録 -----
void EnemyManager::Register(Enemy* enemy)
{
    generates_.insert(enemy);
}

// ----- 削除 -----
void EnemyManager::Remove(Enemy* enemy)
{
    removes_.insert(enemy);
}

// ----- 全削除 -----
void EnemyManager::Clear()
{
    for (Enemy*& enemy : enemies_)
    {
        SafeDeletePtr(enemy);
    }
    enemies_.clear();
    enemies_.shrink_to_fit();
}
