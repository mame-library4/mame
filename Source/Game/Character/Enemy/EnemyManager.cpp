#include "EnemyManager.h"
#include "../Other/Common.h"

// ----- ������ -----
void EnemyManager::Initialize()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Initialize();
    }
}

// ----- �I���� -----
void EnemyManager::Finalize()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Finalize();
    }
    Clear();
}

// ----- �X�V -----
void EnemyManager::Update(const float& elapsedTime)
{
    // ����
    for (Enemy* enemy : generates_)
    {
        enemies_.emplace_back(enemy);
        enemy->Initialize();
    }
    generates_.clear();

    // �X�V
    for (Enemy*& enemy : enemies_)
    {
        enemy->Update(elapsedTime);
    }

    // �j��
    for (Enemy* enemy : removes_)
    {
        // vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜
        auto it = std::find(enemies_.begin(), enemies_.end(), enemy);

        // std::vector�ŊǗ�����Ă���v�f���폜����ɂ�erase()�֐����g�p����
        // (�j�����X�g�̃|�C���^����C�e���[�^�[���������Aerase�֐��ɓn��)
        if (it != enemies_.end())
        {
            enemies_.erase(it);
        }

        // �G�̔j������
        SafeDeletePtr(enemy);        
    }
    // �j�����X�g���N���A
    removes_.clear();
}

// ----- �`�� -----
void EnemyManager::Render()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->Render();
    }
}

// ----- ImGui�p -----
void EnemyManager::DrawDebug()
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->DrawDebug();
    }
}

// ----- Debug�p -----
void EnemyManager::DebugRender(DebugRenderer* debugRenderer)
{
    for (Enemy*& enemy : enemies_)
    {
        enemy->DebugRender(debugRenderer);
    }
}

// ----- �o�^ -----
void EnemyManager::Register(Enemy* enemy)
{
    generates_.insert(enemy);
}

// ----- �폜 -----
void EnemyManager::Remove(Enemy* enemy)
{
    removes_.insert(enemy);
}

// ----- �S�폜 -----
void EnemyManager::Clear()
{
    for (Enemy*& enemy : enemies_)
    {
        SafeDeletePtr(enemy);
    }
    enemies_.clear();
    enemies_.shrink_to_fit();
}
