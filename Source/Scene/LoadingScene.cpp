#include "LoadingScene.h"
#include "SceneManager.h"

#include "../Graphics/Graphics.h"

#include "../Other/misc.h"

// ----- �R���X�g���N�^ -----
LoadingScene::LoadingScene(BaseScene* nextScene) 
    : nextScene_(nextScene)
{
}

// ----- ���\�[�X���� -----
void LoadingScene::CreateResource()
{
}

// ----- ������ -----
void LoadingScene::Initialize()
{
    // �X���b�h�J�n
    // std::thread(LoadingThread, this);
    // ��ڂ̈�����LoadingThread�̈����ɂȂ�
    thread_ = new std::thread(LoadingThread, this);
}

// ----- �I���� -----
void LoadingScene::Finalize()
{
    // �X���b�h�I����
    thread_->join();
    if (thread_ != nullptr)
    {
        delete thread_;
        thread_ = nullptr;
    }
}

// �X�V����
void LoadingScene::Update(const float& elapsedTime)
{
    // ���̃V�[���������ł�����
    if (nextScene_->IsReady())
    {
        // �V�[���؂�ւ�
        SceneManager::Instance().ChangeScene(nextScene_);
        return;
    }
}

void LoadingScene::ShadowRender(ID3D11DeviceContext* deviceContext)
{
}

void LoadingScene::DeferredRender(ID3D11DeviceContext* deviceContext)
{
}

void LoadingScene::ForwardRender(ID3D11DeviceContext* deviceContext)
{
}

void LoadingScene::UserInterfaceRender(ID3D11DeviceContext* deviceContext)
{
}

// ----- ImGui�p -----
void LoadingScene::DrawDebug()
{
}

// ----- ���[�f�B���O�X���b�h -----
void LoadingScene::LoadingThread(LoadingScene* scene)
{
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    std::ignore = CoInitialize(nullptr); // std::ignore�ŕԂ�l�x������

    // ���̃V�[���̏��������s��
    scene->nextScene_->CreateResource();
    scene->nextScene_->Initialize();
    

    // �X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    // ���̃V�[���̏��������ݒ�
    scene->nextScene_->SetReady();
}