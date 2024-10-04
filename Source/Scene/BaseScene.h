#pragma once
#include <string>
#include <vector>
#include <d3d11.h>

class BaseScene
{
public:
    BaseScene() {}
    virtual ~BaseScene() {}

    virtual void CreateResource()                   = 0; // ���\�[�X����
    virtual void Initialize()                       = 0; // ������
    virtual void Finalize()                         = 0; // �I����
    virtual void Update(const float& elapsedTime)   = 0; // �X�V����

    virtual void ShadowRender()                     = 0; 
    virtual void DeferredRender()                   = 0;
    virtual void ForwardRender()                    = 0;

    // �e�X�g�p
    virtual void Render() = 0;
    
    virtual void DrawDebug()                        = 0; // ImGui�p

public:
    bool IsReady()const { return ready; }
    void SetReady() { ready = true; }

private:
    bool ready = false; // ���������Ǘ��t���O
};

