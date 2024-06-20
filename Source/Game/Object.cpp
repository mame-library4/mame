#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- �R���X�g���N�^ -----
Object::Object(const std::string filename)
    : gltfModel_(filename)
{
    // AnimationNode
    animatedNodes_[0] = gltfModel_.nodes_;
    animatedNodes_[1] = gltfModel_.nodes_;
    blendAnimatedNodes_ = gltfModel_.nodes_;
}

// ----- �f�X�g���N�^ -----
Object::~Object()
{
}

void Object::Update(const float& elapsedTime)
{
    UpdateAnimation(elapsedTime);
}

// ----- �`�� -----
void Object::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMMATRIX World = GetTransform()->CalcWorldMatrix(scaleFactor);
    DirectX::XMStoreFloat4x4(&world, World);

    if (isBlendAnimation_)
    {
        gltfModel_.Render(world, blendAnimatedNodes_, psShader);
    }
    else
    {
        gltfModel_.Render(world, animatedNodes_[0], psShader);
    }
}

// ----- ImGui�p -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}

void Object::PlayAnimation(const int& animationIndex, const bool& loop)
{
    currentAnimationIndex_ = animationIndex;

    animationLoopFlag_ = loop;
    animationEndFlag_ = false;
}

void Object::PlayBlendAnimation(const int& blendAnimationIndex, const bool& loop)
{
    gltfModel_.Animate(currentAnimationIndex_, currentAnimationSeconds_, animatedNodes_[0]);
    gltfModel_.Animate(blendAnimationIndex, 0.0f, animatedNodes_[1]);
    currentAnimationIndex_ = blendAnimationIndex;
    currentAnimationSeconds_ = 0.0f;
    animationFactor_ = 0.0f;
    isBlendAnimation_ = true;

    animationLoopFlag_ = loop;
    animationEndFlag_ = false;
}

void Object::UpdateAnimation(const float& elapsedTime)
{
    // �A�j���[�V�����ԍ����Ȃ��̂ł����ŏI��
    if (currentAnimationIndex_ < 0) return;

    if (isBlendAnimation_)
    {
        animationFactor_ = currentAnimationSeconds_ / transitionTime_;
        gltfModel_.BlendAnimations(animatedNodes_[0], animatedNodes_[1], animationFactor_, blendAnimatedNodes_);
        currentAnimationSeconds_ += elapsedTime;
        
        // �u�����h���������
        if (animationFactor_ > 1.0f)
        {
            isBlendAnimation_ = false;
            currentAnimationSeconds_ = 0.0f;
        }
    }
    else
    {
        // �A�j���[�V�������Đ��I����Ă���̂ł����ŏI��
        if (animationEndFlag_) return;

        currentAnimationSeconds_ += elapsedTime;
        
        // �A�j���[�V�������Ō�̃t���[���܂ŒB����
        if (gltfModel_.animations_.at(currentAnimationIndex_).duration_ < currentAnimationSeconds_)
        {
            if (animationLoopFlag_)
            {
                currentAnimationSeconds_ = 0.0f;
            }
            else
            {
                currentAnimationSeconds_ = gltfModel_.animations_.at(currentAnimationIndex_).duration_;
                animationEndFlag_ = true;
            }
        }

        gltfModel_.Animate(currentAnimationIndex_, currentAnimationSeconds_, animatedNodes_[0]);
    }
}