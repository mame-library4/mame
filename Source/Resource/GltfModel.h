#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "../../External/tinygltf/tiny_gltf.h"
#include <unordered_map>
#include "../Other/Transform.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "ConstantBuffer.h"

namespace DirectX
{  
    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

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

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, nodes_);
        }
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

        bool isRootNode_ = false; // rootNodeの場合trueが入る

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, skin_, mesh_, children_, rotation_, scale_, translation_, globalTransform_, isRootNode_);
        }
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
        
        std::vector<UINT8> verticesBinary_;

        template<class T>
        void serialize(T& archive)
        {
            archive(format_, strideInBytes_, sizeInBytes_, verticesBinary_);
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

            template<class T>
            void serialize(T& archive)
            {
                archive(material_, vertexBufferViews_, indexBufferView_);
            }
        };
        std::vector<Primitive> primitives_;

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, primitives_);
        }
    };

    struct TextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;

        template<class T>
        void serialize(T& archive)
        {
            archive(index_, texcoord_);
        }
    };
   
    struct NormalTextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;
        float scale_    = 1;

        template<class T>
        void serialize(T& archive)
        {
            archive(index_, texcoord_, scale_);
        }
    };

    struct OcclusionTextureInfo
    {
        int index_      = -1;
        int texcoord_   = 0;
        float strength_ = 1;

        template<class T>
        void serialize(T& archive)
        {
            archive(index_, texcoord_, strength_);
        }
    };

    struct PbrMetaricRoughness
    {
        float       baseColorFactor_[4] = { 1, 1, 1, 1 };
        TextureInfo baseColorTexture_;
        float       metallicFactor_ = 1;
        float       roughnessFactor_ = 1;
        TextureInfo metallicRoughnessTexture_;

        template<class T>
        void serialize(T& archive)
        {
            archive(baseColorFactor_, baseColorTexture_, metallicFactor_, roughnessFactor_, metallicRoughnessTexture_);
        }
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

            template<class T>
            void serialize(T& archive)
            {
                archive(emissiveFactor_, alphaMode_, alphaCutoff_, doubleSided_, pbrMetallicRoughness_,
                    normalTexture_, occlusionTexture_, emissiveTexture_);
            }
        };
        Cbuffer data_;

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, data_);
        }
    };

    struct TextureData
    {
        std::string name_;
        int source_ = -1;

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, source_);
        }
    };

    struct Image
    {
        std::string uri_;
        std::wstring filename_;
        std::string name_;
        int         width_      = -1;
        int         height_     = -1;
        int         component_  = -1;
        int         bits_       = -1;
        int         pixelType_  = -1 ;
        int         bufferView_ = 0;
        std::string mimeType_;
        bool        asIs_       = false;


        template<class T>
        void serialize(T& archive)
        {
            archive(name_, width_, height_, component_, bits_, pixelType_, bufferView_,
                mimeType_, uri_, asIs_, filename_);
        }
    };

    struct Skin
    {
        std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices_;
        std::vector<int> joints_;

        template<class T>
        void serialize(T& archive)
        {
            archive(inverseBindMatrices_, joints_);
        }
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

            template<class T>
            void serialize(T& archive)
            {
                archive(sampler_, targetNode_, targetPath_);
            }
        };
        std::vector<Channel> channels_;

        struct Sampler
        {
            int input_ = -1;
            int output_ = -1;
            std::string interpolation_;  // 補間

            template<class T>
            void serialize(T& archive)
            {
                archive(input_, output_, interpolation_);
            }
        };
        std::vector<Sampler> samplers_;

        std::unordered_map<int, std::vector<float>> timelines_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> scales_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT4>> rotations_;
        std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> translations_;

        template<class T>
        void serialize(T& archive)
        {
            archive(name_, duration_, channels_, samplers_, timelines_, scales_, rotations_, translations_);
        }
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
    void Render(const float& scaleFactor, ID3D11PixelShader* psShader = nullptr);
    void Render(const DirectX::XMFLOAT4X4 world, ID3D11PixelShader* psShader = nullptr);
    void CastShadow(const float& scaleFactor);

    void DrawDebug();

    // ---------- Animation ----------
    void PlayAnimation(const int& index, const bool& loop, const float& speed);
    void PlayBlendAnimation(const int& index, const bool& loop, const float& speed, const float& blendAnimationFrame);
    void UpdateAnimation(const float& elapsedTime);
    [[nodiscard]] const bool IsPlayAnimation();
    [[nodiscard]] const int GetAnimationIndex() const { return animationIndex_; }
    [[nodiscard]] const float GetAnimationSeconds() { return isBlendAnimation_ ? 0.0f : animationSeconds_; }
    [[nodiscard]] const float GetAnimationSpeed() const { return animationSpeed_; }
    [[nodiscard]] const float GetTransitionTime() const { return transitionTime_; }
    [[nodiscard]] const bool GetIsBlendAnimation() const { return isBlendAnimation_; }
    [[nodiscard]] const bool GetIsAnimationLooped() const { return isAnimationLooped_; }
    [[nodiscard]] const bool GetUseRootMotionMovement() const { return useRootMotionMovement_; }
    void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }
    void SetTransitionTime(const float& time) { transitionTime_ = time; }
    void SetUseRootMotionMovement(const bool& flag) { useRootMotionMovement_ = flag; }

    // ---------- Transform ----------
    Transform* GetTransform() { return &transform_; }

    // ----- JointPosiion -----
    DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {});
    DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {});
    DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const DirectX::XMFLOAT4X4& world);
    DirectX::XMMATRIX GetJointGlobalTransform(const size_t& nodeIndex);
    DirectX::XMMATRIX GetJointGlobalTransform(const std::string& nodeName);
    DirectX::XMMATRIX GetJointWorldTransform(const std::string& nodeName, const float& scaleFacter);

    // ----- Node -----
    [[nodiscard]] const int GetNodeIndex(const std::string& nodeName);
    std::vector<Node>* GetNodes() { return &nodes_; }

    // ----- RootMotion -----
    void RootMotion(const float& scaleFacter);

    void SetRootJointIndex(const int& index) { rootJointIndex_ = index; }
    void SetUseRootMotion(const bool& flag);

    void SetUpperLowerBodyAnimationIndex(const int& index) { upperLowerBodyAnimationIndex_ = index; }
    void SetIsUpperLowerBodyAnimation(const bool& flag) { isUpperLowerBodyAnimation_ = flag; }

private:
    void FetchNodes(const tinygltf::Model& gltfModel);
    void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchMatetials(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchTexture(ID3D11Device* device, const tinygltf::Model& gltfModel);
    void FetchAnimation(const tinygltf::Model& gltfModel);

    void CumulateTransforms(std::vector<Node>& nodes);
    GltfModel::BufferView MakeBufferView(const tinygltf::Accessor& accessor);

    // ---------- Animation ----------
    bool UpdateBlendAnimation(const float& elapsedTime);
    void BlendAnimations(const std::vector<Node>* nodes[2], float factor, std::vector<Node>& node);
    void Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes);
    
    void UpdateUpperLowerBodyAnimation(const float& elapsedTime);

private:
    std::string filename_;

    Transform transform_ = {};

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    struct PrimitiveConstants
    {
        DirectX::XMFLOAT4X4 world_;
        int                 material_               = -1;
        int                 hasTangent_             = 0;
        int                 skin_                   = -1;
        int                 startInstanceLocation_  = 0;
    };
    std::unique_ptr<ConstantBuffer<PrimitiveConstants>> primitiveConstants_;

    static const size_t maxJoints_ = 512;
    struct JointConstants
    {
        DirectX::XMFLOAT4X4 matrices_[maxJoints_];
    };
    std::unique_ptr<ConstantBuffer<JointConstants>> jointConstants_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>   shadowVertexShader_;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> shadowGeometryShader_;

    // ---------- Animation ----------
    int     animationIndex_         = -1;       // アニメーション番号
    float   animationSeconds_       = 0.0f;     // アニメーション再生時間
    float   blendAnimationSeconds_  = 0.0f;
    float   animationSpeed_         = 1.0f;     // アニメーション再生速度
    float   weight_                 = 0.0f;     // 影響値
    float   transitionTime_         = 0.15f;    // アニメーションブレンド速度
    bool    animationLoopFlag_      = false;    // アニメーションループフラグ
    bool    isAnimationLooped_      = false;    // アニメーションが一回以上ループしたか
    bool    animationEndFlag_       = false;    // アニメーションエンドフラグ
    bool    isBlendAnimation_       = false;    // ブレンドするかフラグ
    bool    useRootMotionMovement_  = false;    // ルートモーションの移動値を使用するか
    std::vector<Node> animatedNodes_[2];
    std::vector<Node> blendedAnimationNodes_;    

    // ---------- RootMotion ----------
    std::vector<GltfModel::Node> zeroAnimatedNodes_;
    DirectX::XMFLOAT3   previousPosition_       = {};
    int                 rootJointIndex_         = 1;
    float               rootMotionSpeed_        = 1.0f;
    bool                isFirstTimeRootMotion_  = false; // RootMotion初回判定

    // ---------- 上下半身分離用 ----------
    std::vector<Node>   upperLowerBodyAnimatedNodes_;
    int                 upperLowerBodyAnimationIndex_   = -1;
    float               upperLowerBodyAnimationSeconds_ = 0.0f;
    bool                isUpperLowerBodyAnimation_      = false;
};