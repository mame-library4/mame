#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "../../External/tinygltf/tiny_gltf.h"
#include <unordered_map>

#include "../Other/Transform.h"

class GltfModel
{
public:
    GltfModel(const std::string& filename);
    virtual ~GltfModel() = default;

public:
    struct Scene
    {
        std::string name_;
        std::vector<int> nodes_;
    };

    struct Node
    {
        std::string name_;
        int skin_ = -1;
        int mesh_ = -1;

        std::vector<int> children_;

        DirectX::XMFLOAT4 rotation_     = { 0, 0, 0, 1 };
        DirectX::XMFLOAT3 scale_        = { 1, 1, 1 };
        DirectX::XMFLOAT3 translation_  = { 0, 0, 0 };

        DirectX::XMFLOAT4X4 globalTransform_
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };

        DirectX::XMFLOAT4X4 parentGlobalTransform_;
    };

    struct BufferView
    {
        DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
        size_t strideInBytes_ = 0;
        size_t sizeInBytes_ = 0;
        size_t count()const
        {
            return sizeInBytes_ / strideInBytes_;
        }
    };

    struct Mesh
    {
        std::string name_;
        struct Primitive
        {
            int material_;
            std::map<std::string, BufferView> vertexBufferViews_;
            BufferView indexBufferView_;
        };
        std::vector<Primitive> primitives_;
    };

    struct TextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;
    };
   
    struct NormalTextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;
        float scale_    = 1;
    };

    struct OcclusionTextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;
        float strength_ = 1;
    };

    struct PbrMetaricRoughness
    {
        float       baseColorFactor_[4] = { 1, 1, 1, 1 };
        TextureInfo baseColorTexture_;
        float       metallicFactor_ = 1;
        float       roughnessFactor_ = 1;
        TextureInfo metallicRoughnessTexture_;
    };

    struct Material
    {
        std::string name_;

        struct Cbuffer
        {
            float   emissiveFactor_[3]  = { 0, 0, 0 };
            int     alphaMode_          = 0; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
            float   alphaCutoff_        = 0.5f;
            bool    doubleSided_        = false;

            PbrMetaricRoughness pbrMetallicRoughness_;

            NormalTextureInfo       normalTexture_;
            OcclusionTextureInfo    occlusionTexture_;
            TextureInfo             emissiveTexture_;
        };
        Cbuffer data_;
    };

    struct TextureData
    {
        std::string name_;
        int source_ = -1;
    };

    struct Image
    {
        std::string name_;
        int         width_      = -1;
        int         height_     = -1;
        int         component   = -1;
        int         bits_       = -1;
        int         pixelType_  = -1 ;
        int         bufferView_ = 0;
        std::string mimeType_;
        std::string uri_;
        bool        asIs_       = false;
    };

    struct Skin
    {
        std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices_;
        std::vector<int> joints_;
    };
        
    struct Animation
    {
        std::string name_;
        float duration_ = 0.0f;

        struct Channel
        {
            int sampler_ = -1;
            int targetNode_ = -1;
            std::string targetPath_;
        };
        std::vector<Channel> channels_;

        struct Sampler
        {
            int input_ = -1;
            int output_ = -1;
            std::string interpolation_;  // ���
        };
        std::vector<Sampler> samplers_;

        std::unordered_map<int, std::vector<float>> timelines_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> scales_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT4>> rotations_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> translations_;
    };
        
    static const size_t PRIMITIVE_MAX_JOINT = 512;
    struct PrimitiveJointConstants
    {
        DirectX::XMFLOAT4X4 matrices_[PRIMITIVE_MAX_JOINT];
    };   

    std::vector<Scene>      scenes_;
    std::vector<Node>       nodes_;
    std::vector<Mesh>       meshes_;
    std::vector<Material>   materials_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;
    std::vector<TextureData>textures_;
    std::vector<Image>      images_;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;
    std::vector<Skin>       skins_;
    std::vector<Animation>  animations_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveJointCbuffer_;

public:
    void UpdateAnimation(const float& elapsedTime);
    bool UpdateBlendAnimation(const float& elapsedTime);
    void BlendAnimations(const std::vector<Node>* nodes[2], float factor, std::vector<Node>& node);

    void Render(const float& scaleFactor, ID3D11PixelShader* psShader = nullptr);
    void Render(const DirectX::XMFLOAT4X4& world, ID3D11PixelShader* psShader = nullptr);
    void DrawDebug();

    void Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes);


    Transform* GetTransform() { return &transform_; }

    // �A�j���[�V�����֘A
    void SetAnimationIndex(int index) { currentAnimationIndex_  = index; }
    int GetAnimationIndex() { return currentAnimationIndex_; }

    // �A�j���[�V�����Đ�
    void PlayAnimation(const int& animationIndex, const bool& loop, const float& speed);
    void PlayBlendAnimation(const int& index1, const int& index2, const bool& loop, const float& speed);
    void PlayBlendAnimation(const int& index, const bool& loop, const float& speed);
    const bool IsPlayAnimation();
    
    [[nodiscard]] const float GetAnimationSpeed() const { return animationSpeed_; }
    void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

    // ----- BlendAnimationSeconds -----
    [[nodiscard]] const float GetBlendAnimationSeconds() const { return blendAnimationSeconds_; }

    // ----- AnimationIndex -----
    [[nodiscard]] const int GetCurrentBlendAnimationIndex() const { return blendAnimationIndex2_; }

    // ----- JointPosiion -----
    DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {});
    DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {});
    DirectX::XMMATRIX GetJointGlobalTransform(const size_t& nodeIndex);
    DirectX::XMMATRIX GetJointGlobalTransform(const std::string& nodeName);
    DirectX::XMMATRIX GetJointWorldTransform(const std::string& nodeName, const float& scaleFacter);

    // ----- Node -----
    [[nodiscard]] const int GetNodeIndex(const std::string& nodeName);
    std::vector<Node>* GetNodes() { return &nodes_; }

    // ----- weight�l -----
    [[nodiscard]] const float GetWeight() const { return weight_; }
    void SetWeight(const float& weight) { weight_ = weight; }

private:
    void FetchNodes(const tinygltf::Model& gltfModel);
    void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchMatetials(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchTexture(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchAnimation(const tinygltf::Model& gltfModel);

    void CumulateTransforms(std::vector<Node>& nodes);
    GltfModel::BufferView MakeBufferView(const tinygltf::Accessor& accessor);

private:
    std::string filename_;

    Transform transform_ = {};

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    struct primitiveConstants
    {
        DirectX::XMFLOAT4X4 world_;
        int material_ = -1;
        int hasTangent_ = 0;
        int skin_ = -1;
        float emissiveIntencity_ = 1.0f;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveCbuffer_;
    float emissiveIntencity_ = 1.0f;

    // ----- Animation -----
    int     currentAnimationIndex_      = -1;       // ���݂̃A�j���[�V�����ԍ�
    float   currentAnimationSeconds_    = 0.0f;     // ���݂̃A�j���[�V�����Đ�����
    int     currentKeyframeIndex_       = 0;        // ���݂̃A�j���[�V�����Đ��t���[��
    bool    animationLoopFlag_          = false;    // �A�j���[�V���������[�v���邩 
    bool    animationEndFlag_           = false;    // �A�j���[�V�������I��������

    int     blendAnimationIndex1_       = -1;       // �P�ڃu�����h�p�A�j���[�V�����ԍ�
    int     blendAnimationIndex2_       = -1;       // �Q�ڃu�����h�p�A�j���[�V�����ԍ�
    float   blendAnimationSeconds_      = 0.0f;     // ���݂̃A�j���[�V�����Đ�����
    float   blendThreshold_             = 0.0f;     // �u�����h�A�j���[�V�����̃x�[�X�ƂȂ�A�j���[�V�����̐؂�ւ�臒l

    float   weight_                     = 0.0f;     // �e���l
    float   animationSpeed_             = 1.0f;     // �A�j���[�V�����Đ����x
};