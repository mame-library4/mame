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

    virtual void ShadowRender(ID3D11DeviceContext* deviceContext)           = 0; 
    virtual void DeferredRender(ID3D11DeviceContext* deviceContext)         = 0;
    virtual void ForwardRender(ID3D11DeviceContext* deviceContext)          = 0;
    virtual void UserInterfaceRender(ID3D11DeviceContext* deviceContext)    = 0;
    
    virtual void DrawDebug()                        = 0; // ImGui�p

public:
    bool IsReady()const { return ready; }
    void SetReady() { ready = true; }

private:
    bool ready = false; // ���������Ǘ��t���O
};

