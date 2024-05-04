#include "GltfModel.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERANL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include "../../External/tinygltf/tiny_gltf.h"

#include <stack>
#include <filesystem>

#include "../Other/misc.h"
#include "../Graphics/shader.h"
#include "../Resource/texture.h"
#include "../Graphics/Graphics.h"

// �R���X�g���N�^
GltfModel::GltfModel(ID3D11Device* device, const std::string& filename) : filename_(filename)
{
    tinygltf::Model gltfModel;

    tinygltf::TinyGLTF tinyGltf;
    std::string error, warning;
    bool succeeded{ false };
    if (filename.find(".glb") != std::string::npos)
    {
        succeeded = tinyGltf.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str());
    }
    else if (filename.find(".gltf") != std::string::npos)
    {
        succeeded = tinyGltf.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());
    _ASSERT_EXPR_A(error.empty(), warning.c_str());
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");
    for (std::vector<tinygltf::Scene>::const_reference gltfScene : gltfModel.scenes)
    {
        Scene& scene{ scenes_.emplace_back() };
        scene.name_ = gltfModel.scenes.at(0).name;
        scene.nodes_ = gltfModel.scenes.at(0).nodes;
    }

    FetchNodes(gltfModel);
    FetchMeshes(device, gltfModel);
    FetchMatetials(device, gltfModel);
    FetchTexture(device, gltfModel);
    FetchAnimation(gltfModel);

    // TODO: This is a force-brute programming, may cause bugs.
    const std::map<std::string, BufferView>& vertexBufferViews{
        meshes_.at(0).primitives_.at(0).vertexBufferViews_ };
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        { "POSITION", 0, vertexBufferViews.at("POSITION").format_, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, vertexBufferViews.at("NORMAL").format_, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, vertexBufferViews.at("TANGENT").format_, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, vertexBufferViews.at("TEXCOORD_0").format_, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS", 0, vertexBufferViews.at("JOINTS_0").format_, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, vertexBufferViews.at("WEIGHTS_0").format_, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS", 1, vertexBufferViews.at("JOINTS_1").format_, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },        
        { "WEIGHTS", 1, vertexBufferViews.at("WEIGHTS_1").format_, 7, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(device, "./Resources/Shader/gltfModelVs.cso", vertexShader_.ReleaseAndGetAddressOf(),
        inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    //CreatePsFromCso(device, "./Resources/Shader/DeferredRenderingPS.cso", pixelShader.ReleaseAndGetAddressOf());
    CreatePsFromCso(device, "./Resources/Shader/gltfModelPs.cso", pixelShader_.ReleaseAndGetAddressOf());

    // �萔�o�b�t�@����
    {
        HRESULT hr;
        D3D11_BUFFER_DESC bufferDesc{};

        // primitiveCbuffer
        bufferDesc.ByteWidth = sizeof(primitiveConstants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, primitiveCbuffer_.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        
        // primitiveJointCbuffer(�{�[���s��)
        bufferDesc.ByteWidth = sizeof(PrimitiveJointConstants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, primitiveJointCbuffer_.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

// ----- �A�j���[�V�����X�V -----
void GltfModel::UpdateAnimation(const float& elapsedTime)
{
    // �A�j���[�V�����Đ����ĂȂ������珈�����Ȃ�
    if (!IsPlayAnimation()) return;

    // �ŏI�t���[������ ( �Đ��I���t���O�������Ă���΍Đ��I�� )
    if (animationEndFlag_)
    {
        animationEndFlag_       = false;    // �I���t���O�����Z�b�g
        currentAnimationIndex_  = -1;       // �A�j���[�V�����ԍ������Z�b�g
        return;
    }

    // �u�����h�A�j���[�V�����Đ� ( �u�����h�A�j���[�V�����̏ꍇ�͂����ŏI�� )
    if (UpdateBlendAnimation(elapsedTime)) return;

    // �A�j���[�V�����Đ����Ԍo��
    currentAnimationSeconds_ += elapsedTime * animationSpeed_;

    Animate(currentAnimationIndex_, currentAnimationSeconds_, nodes_, animationLoopFlag_);
}

// ----- �u�����h�A�j���[�V�����X�V ( �X�V���Ă����� true ) -----
bool GltfModel::UpdateBlendAnimation(const float& elapsedTime)
{
    // �A�j���[�V�����ԍ��������ĂȂ��ꍇ�� return false
    if (blendAnimationIndex1_ < 0) return false;

    if (animationEndFlag_)
    {
        animationEndFlag_ = false;  // �I���t���O�����Z�b�g
        blendAnimationIndex1_ = -1; // �A�j���[�V�����ԍ����Z�b�g
        blendAnimationIndex2_ = -1; // �A�j���[�V�����ԍ����Z�b�g
        return false;
    }

    // weight�l���O�`�P�̊ԂɎ��߂�
    weight_ = std::clamp(weight_, 0.0f, 1.0f);

    // �A�j���[�V�����Đ����ԍX�V
    blendAnimationSeconds_ += elapsedTime * animationSpeed_;

    // �Q�̃A�j���[�V�����̎p��������Ă���
    Animate(blendAnimationIndex1_, blendAnimationSeconds_, nodes_, animationLoopFlag_);
    std::vector<Node> node1 = nodes_;
    Animate(blendAnimationIndex2_, blendAnimationSeconds_, nodes_, animationLoopFlag_);
    std::vector<Node> node2 = nodes_;

    // �Q�̃A�j���[�V�����̃u�����h������
    const std::vector<Node>* keyframes[2] = { &node1, &node2 };
    std::vector<Node> result = node1;
    BlendAnimations(keyframes, weight_, result);

    CumulateTransforms(result);
    nodes_ = result;

    return true;
}

void GltfModel::BlendAnimations(const std::vector<Node>* nodes[2], float factor, std::vector<Node>& node)
{
    size_t nodeCount = nodes[0]->size();
    node.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2] =
        {
            DirectX::XMLoadFloat3(&nodes[0]->at(nodeIndex).scale_),
            DirectX::XMLoadFloat3(&nodes[1]->at(nodeIndex).scale_)
        };
        DirectX::XMStoreFloat3(&node.at(nodeIndex).scale_, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2] =
        {
            DirectX::XMLoadFloat4(&nodes[0]->at(nodeIndex).rotation_),
            DirectX::XMLoadFloat4(&nodes[1]->at(nodeIndex).rotation_)
        };
        DirectX::XMStoreFloat4(&node.at(nodeIndex).rotation_, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2] =
        {
            DirectX::XMLoadFloat3(&nodes[0]->at(nodeIndex).translation_),
            DirectX::XMLoadFloat3(&nodes[1]->at(nodeIndex).translation_),
        };
        DirectX::XMStoreFloat3(&node.at(nodeIndex).translation_, DirectX::XMVectorLerp(T[0], T[1], factor));
    }
}

// �O�ł̌Ăяo���p
void GltfModel::Render(float scaleFactor)
{
    Graphics& graphics = Graphics::Instance();
    DirectX::XMMATRIX W = transform_.CalcWorldMatrix(scaleFactor);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, W);
    
    Render(graphics.GetDeviceContext(), world, nodes_);
}

// �{��
void GltfModel::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const std::vector<Node>& animatedNodes)
{
    deviceContext->PSSetShaderResources(0, 1, materialResourceView_.GetAddressOf());
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
    deviceContext->IASetInputLayout(inputLayout_.Get());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::function<void(int)> traverse{ [&](int nodeIndex)->void {
        const Node& node{nodes_.at(nodeIndex)};
        if (node.mesh_ > -1)
        {
            const Mesh& mesh{ meshes_.at(node.mesh_) };
            for (std::vector<Mesh::Primitive>::const_reference primitive : mesh.primitives_)
            {
                ID3D11Buffer* vertexBuffers[]
                {
                    primitive.vertexBufferViews_.at("POSITION").buffer_.Get(),
                    primitive.vertexBufferViews_.at("NORMAL").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TANGENT").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_1").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_1").buffer_.Get(),
                };
                UINT strides[]
                {
                    static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_1").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_1").strideInBytes_),
                };
                UINT offsets[_countof(vertexBuffers)]{ 0 };
                deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
                deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(),
                    primitive.indexBufferView_.format_, 0);

                primitiveConstants primitiveData{};
                primitiveData.material_ = primitive.material_;
                primitiveData.hasTangent_ = primitive.vertexBufferViews_.at("TANGENT").buffer_ != NULL;
                primitiveData.skin_ = node.skin_;
                
                DirectX::XMStoreFloat4x4(&primitiveData.world_,
                    DirectX::XMLoadFloat4x4(&node.globalTransform_) * DirectX::XMLoadFloat4x4(&world));
                deviceContext->UpdateSubresource(primitiveCbuffer_.Get(), 0, 0, &primitiveData, 0, 0);
                deviceContext->VSSetConstantBuffers(0, 1, primitiveCbuffer_.GetAddressOf());
                deviceContext->PSSetConstantBuffers(0, 1, primitiveCbuffer_.GetAddressOf());

                // texture
                {
                    const Material& material{ materials_.at(primitive.material_) };
                    const int textureIndices[]
                    {
                        material.data_.pbrMetallicRoughness_.baseColorTexture_.index_,
                        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
                        material.data_.normalTexture_.index_,
                        material.data_.emissiveTexture_.index_,
                        material.data_.occlusionTexture_.index_,
                    };
                    ID3D11ShaderResourceView* nullShaderResourceView{};
                    std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices));
                    for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
                    {
                        shaderResourceViews.at(textureIndex) = textureIndices[textureIndex] > -1 ?
                            textureResourceViews_.at(textures_.at(textureIndices[textureIndex]).source_).Get() :
                            nullShaderResourceView;
                    }
                    deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()),
                        shaderResourceViews.data());
                }

                // animation
                {
                    if (node.skin_ > -1)
                    {
                        const Skin& skin{ skins_.at(node.skin_) };
                        PrimitiveJointConstants primitiveJointData{};
                        auto size__ = skin.joints_.size();
                        for (size_t jointIndex = 0; jointIndex < skin.joints_.size(); ++jointIndex)
                        {
                            DirectX::XMStoreFloat4x4(&primitiveJointData.matrices_[jointIndex],
                                DirectX::XMLoadFloat4x4(&skin.inverseBindMatrices_.at(jointIndex)) *
                                DirectX::XMLoadFloat4x4(&animatedNodes.at(skin.joints_.at(jointIndex)).globalTransform_) *
                                DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_))
                            );
                        }
                        deviceContext->UpdateSubresource(primitiveJointCbuffer_.Get(), 0, 0, &primitiveJointData, 0, 0);
                        deviceContext->VSSetConstantBuffers(2, 1, primitiveJointCbuffer_.GetAddressOf());
                    }
                }

                deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexBufferView_.count()), 0, 0);
            }
        }
        for (std::vector<int>::value_type childIndex : node.children_)
        {
            traverse(childIndex);
        }
    } };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        traverse(nodeIndex);
    }
}

void GltfModel::DrawDebug()
{
#ifdef USE_IMGUI
    GetTransform()->DrawDebug();
    ImGui::DragFloat("weight", &weight_, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat("animationSpeed", &animationSpeed_, 0.1f);
    ImGui::Checkbox("animationLoopFlag_", &animationLoopFlag_);
    ImGui::Checkbox("animationEndFlag_", &animationEndFlag_);
    ImGui::DragFloat("currentAnimationSeconds_", &currentAnimationSeconds_);
    ImGui::DragFloat("blendAnimationSeconds", &blendAnimationSeconds_);
    if (ImGui::Button("timeReset"))
    {
        currentAnimationSeconds_ = 0.0f;
        animationEndFlag_ = false;
    }
#endif // USE_IMGUI
}

void GltfModel::Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes, bool loopback)
{
    std::function<size_t(const std::vector<float>&, float, float&, bool)> indexof
    {
        [](const std::vector<float>& timelines, float time, float& interpolationFactor, bool loopback)->size_t
        {
            const size_t keyframeCount{timelines.size()}; 

            if (time > timelines.at(keyframeCount - 1))
            {
                if (loopback)
                {
                    time = fmodf(time, timelines.at(keyframeCount - 1));
                }
                else
                {
                    interpolationFactor = 1.0f;
                    return keyframeCount - 2;
                }
            }
            else if (time < timelines.at(0))
            {
                interpolationFactor = 0.0f;
                return 0;
            }
            size_t keyframeIndex{ 0 };
            for (size_t timeIndex = 1; timeIndex < keyframeCount; ++timeIndex)
            {
                if (time < timelines.at(timeIndex))
                {
                    keyframeIndex = std::max<size_t>(0LL, timeIndex - 1);
                    break;
                }
            }
            interpolationFactor = (time - timelines.at(keyframeIndex + 0)) /
                (timelines.at(keyframeIndex + 1) - timelines.at(keyframeIndex + 0));
            return keyframeIndex;
        }
    };

    if (animations_.size() > 0)
    {
        const Animation& animation{ animations_.at(animationIndex) };
        for (std::vector<Animation::Channel>::const_reference channel : animation.channels_)
        {
            const Animation::Sampler& sampler{ animation.samplers_.at(channel.sampler_) };
            const std::vector<float>& timeline{ animation.timelines_.at(sampler.input_) };
            if (timeline.size() == 0)
            {
                continue;
            }
            float interpolationFactor{};

            size_t keyframeIndex{ indexof(timeline, time, interpolationFactor, loopback) };

            // �A�j���[�V�����I������^�C�~���O�̔��������Ă���
            const size_t keyframeCount = timeline.size();
            if (time > timeline.at(keyframeCount - 1))
            {
                if (animationLoopFlag_)
                {
                    //currentAnimationSeconds_ = 0.0f;
                    //blendAnimationSeconds_ = 0.0f;
                }
                else
                {
                    animationEndFlag_ = true;
                }
            }
            
            if (channel.targetPath_ == "scale")
            {
                const std::vector<DirectX::XMFLOAT3>& scales{ animation.scales_.at(sampler.output_) };
                DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).scale_,
                    DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&scales.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat3(&scales.at(keyframeIndex + 1)), interpolationFactor));
            }
            else if (channel.targetPath_ == "rotation")
            {
                const std::vector<DirectX::XMFLOAT4>& rotations{ animation.rotations_.at(sampler.output_) };
                DirectX::XMStoreFloat4(&animatedNodes.at(channel.targetNode_).rotation_,
                    DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&rotations.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat4(&rotations.at(keyframeIndex + 1)), interpolationFactor)));
            }
            else if (channel.targetPath_ == "translation")
            {
                const std::vector<DirectX::XMFLOAT3>& translations{ animation.translations_.at(sampler.output_) };
                DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).translation_,
                    DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&translations.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat3(&translations.at(keyframeIndex + 1)), interpolationFactor));
            }
        }
        CumulateTransforms(animatedNodes);
    }
    else
    {
        animatedNodes = nodes_;
    }
}

// ----- �A�j���[�V�����Đ� -----
void GltfModel::PlayAnimation(const int& index, const bool& loop, const float& speed)
{
    if (index == currentAnimationIndex_) return;

    currentAnimationIndex_      = index;    // �A�j���[�V�����ԍ���ݒ�
    currentAnimationSeconds_    = 0.0f;     // �^�C�}�[�����Z�b�g����
    
    blendAnimationIndex1_ = -1; // �g�p���Ȃ��̂�-1
    blendAnimationIndex2_ = -1; // �g�p���Ȃ��̂�-1

    animationLoopFlag_          = loop;     // �A�j���[�V�������[�v�t���O��ݒ肷��
    animationEndFlag_           = false;    // �Đ��I���t���O�����Z�b�g

    animationSpeed_             = speed;    // �A�j���[�V�����Đ����x��ݒ�
}

// ----- �u�����h�A�j���[�V�����Đ� -----
void GltfModel::PlayBlendAnimation(const int& index1, const int& index2, const bool& loop, const float& speed)
{
    // �ݒ�p�̃A�j���[�V�����ԍ������݂̃A�j���[�V�����ԍ��Ɠ����ꍇ��return
    if (blendAnimationIndex1_ == index1 && blendAnimationIndex2_ == index2) return;
    
    // �ʏ�A�j���[�V�����ԍ������Z�b�g
    currentAnimationIndex_ = -1;

    blendAnimationIndex1_ = index1;    // �Đ�����A�j���[�V�����ԍ���ݒ�
    blendAnimationIndex2_ = index2;    // �Đ�����A�j���[�V�����ԍ���ݒ�
    blendAnimationSeconds_ = 0.0f;     // �A�j���[�V�����Đ����ԃ��Z�b�g

    animationLoopFlag_  = loop;     // ���[�v�����邩
    animationEndFlag_   = false;    // �Đ��I���t���O�����Z�b�g

    animationSpeed_     = speed;    // �A�j���[�V�����Đ����x
}

// ----- �A�j���[�V�������Đ����Ȃ� true -----
const bool GltfModel::IsPlayAnimation()
{
    //// �A�j���[�V�����Đ�����Ă��Ȃ�
    if (currentAnimationIndex_ < 0 && blendAnimationIndex1_ < 0) return false;

    // �A�j���[�V�����ԍ������݂��Ȃ�
    const int animatinIndexEnd = static_cast<int>(animations_.size());
    if (currentAnimationIndex_ >= animatinIndexEnd && blendAnimationIndex1_ >= animatinIndexEnd) return false;
    

    return true;
}

// ----- �w�肵���W���C���g�̈ʒu���擾 -----
DirectX::XMFLOAT3 GltfModel::GetJointPosition(const size_t& nodeIndex, const float& scaleFactor)
{
    DirectX::XMFLOAT3 position = { 0, 0, 0 };

    const Node& node = nodes_.at(nodeIndex);
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * GetTransform()->CalcWorldMatrix(scaleFactor);
    DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));

    return position;
}

// ----- �w�肵���W���C���g�̈ʒu���擾 -----
DirectX::XMFLOAT3 GltfModel::GetJointPosition(const std::string& nodeName, const float& scaleFactor)
{
    DirectX::XMFLOAT3 position = { 0, 0, 0 };

    // �m�[�h�𖼑O��������
    for (Node& node : nodes_)
    {
        // ���O����v���Ȃ������� continue
        if (node.name_ != nodeName) continue;        

        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * GetTransform()->CalcWorldMatrix(scaleFactor);
        DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));

        return position;
    }

    // ������Ȃ������B
    return DirectX::XMFLOAT3(0, 0, 0);
}

// �m�[�h�f�[�^�擾
void GltfModel::FetchNodes(const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Node>::const_reference gltfNode : gltfModel.nodes)
    {
        Node& node{ nodes_.emplace_back() };
        node.name_ = gltfNode.name;
        node.skin_ = gltfNode.skin;
        node.mesh_ = gltfNode.mesh;
        node.children_ = gltfNode.children;
        if (!gltfNode.matrix.empty())
        {
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; ++row)
            {
                for (size_t column = 0; column < 4; ++column)
                {
                    matrix(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
                }
            }

            DirectX::XMVECTOR S, T, R;
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR(succeed, L"Failed to decompose matrix");

            DirectX::XMStoreFloat3(&node.scale_, S);
            DirectX::XMStoreFloat4(&node.rotation_, R);
            DirectX::XMStoreFloat3(&node.translation_, T);
        }
        else
        {
            if (gltfNode.scale.size() > 0)
            {
                node.scale_.x = static_cast<float>(gltfNode.scale.at(0));
                node.scale_.y = static_cast<float>(gltfNode.scale.at(1));
                node.scale_.z = static_cast<float>(gltfNode.scale.at(2));
            }
            if (gltfNode.translation.size() > 0)
            {
                node.translation_.x = static_cast<float>(gltfNode.translation.at(0));
                node.translation_.y = static_cast<float>(gltfNode.translation.at(1));
                node.translation_.z = static_cast<float>(gltfNode.translation.at(2));
            }
            if (gltfNode.rotation.size() > 0)
            {
                node.rotation_.x = static_cast<float>(gltfNode.rotation.at(0));
                node.rotation_.y = static_cast<float>(gltfNode.rotation.at(1));
                node.rotation_.z = static_cast<float>(gltfNode.rotation.at(2));
                node.rotation_.w = static_cast<float>(gltfNode.rotation.at(3));
            }
        }
    }
    CumulateTransforms(nodes_);
}

// ���b�V���f�[�^�擾
void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    HRESULT hr;
    for (std::vector<tinygltf::Mesh>::const_reference gltfMesh : gltfModel.meshes)
    {
        Mesh& mesh{ meshes_.emplace_back() };
        mesh.name_ = gltfMesh.name;
        for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
        {
            Mesh::Primitive& primitive{ mesh.primitives_.emplace_back() };
            primitive.material_ = gltfPrimitive.material;

            // Create index buffer
            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfPrimitive.indices) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

            primitive.indexBufferView_ = MakeBufferView(gltfAccessor);
            D3D11_BUFFER_DESC bufferDesc{};
            bufferDesc.ByteWidth = static_cast<UINT>(primitive.indexBufferView_.sizeInBytes_);
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            D3D11_SUBRESOURCE_DATA subresourceData{};
            subresourceData.pSysMem = gltfModel.buffers.at(gltfBufferView.buffer).data.data()
                + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
            hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                primitive.indexBufferView_.buffer_.ReleaseAndGetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            // Create vertex buffers
            for (std::map<std::string, int>::const_reference gltfAttribute : gltfPrimitive.attributes)
            {
                // TODO:�����̏���
                // if 1��0�ɂ�����fbx�r���A�[�ǂ���ɂł�...
                // �ǂ�����������ł��傤��
#if 0
                const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfAttribute.second) };
                const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

                BufferView vertexBufferView{ MakeBufferView(gltfAccessor) };

                D3D11_BUFFER_DESC bufferDesc{};
                bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresourceData{};
                subresourceData.pSysMem = gltfModel.buffers.at(gltfBufferView.buffer).data.data()
                    + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                    vertexBufferView.buffer_.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                primitive.vertexBufferViews_.emplace(std::make_pair(gltfAttribute.first, vertexBufferView));
#else
                tinygltf::Accessor gltfAccessor{ gltfModel.accessors.at(gltfAttribute.second) };
                const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

                const void* buffer = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                std::vector<USHORT> joints0;
                std::vector<FLOAT> weight0;
                if (gltfAttribute.first == "JOINTS_0")
                {
                    if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        const BYTE* data = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                        for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count * 4; ++accessorIndex)
                        {
                            joints0.emplace_back(static_cast<USHORT>(data[accessorIndex]));
                        }
                        buffer = joints0.data();
                        gltfAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
                    }
                }
                else if (gltfAttribute.first == "JOINTS_1")
                {
                    continue;
                }
                else if (gltfAttribute.first == "WEIGHTS_0")
                {
                    if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        const BYTE* data = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                        for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count * 4; ++accessorIndex)
                        {
                            weight0.emplace_back(static_cast<FLOAT>(data[accessorIndex]) / 0xFF);
                        }
                        buffer = weight0.data();
                        gltfAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
                    }
                }
                else if (gltfAttribute.first == "WEIGHTS_1")
                {
                    continue;
                }


                BufferView vertexBufferView{ MakeBufferView(gltfAccessor) };

                D3D11_BUFFER_DESC bufferDesc{};
                bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresourceData{};
                subresourceData.pSysMem = buffer;
                hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                    vertexBufferView.buffer_.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                primitive.vertexBufferViews_.emplace(std::make_pair(gltfAttribute.first, vertexBufferView));
#endif
            }

            // Add dummy attributes if any are amissing.
            const std::unordered_map<std::string, BufferView> attributes{
                { "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
                { "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
                { "JOINTS_0", { DXGI_FORMAT_R16G16B16A16_UINT } },
                { "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
                { "JOINTS_1", { DXGI_FORMAT_R16G16B16A16_UINT } },
                { "WEIGHTS_1", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
            };
            
            for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
            {
                if (primitive.vertexBufferViews_.find(attribute.first) == primitive.vertexBufferViews_.end())
                {
                    primitive.vertexBufferViews_.insert(std::make_pair(attribute.first, attribute.second));
                }
            }
        }
    }
}

// �}�e���A���̎擾
void GltfModel::FetchMatetials(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Material>::const_reference gltfMaterial : gltfModel.materials)
    {
        std::vector<Material>::reference material = materials_.emplace_back();

        material.name_ = gltfMaterial.name;

        material.data_.emissiveFactor_[0] = static_cast<float>(gltfMaterial.emissiveFactor.at(0));
        material.data_.emissiveFactor_[1] = static_cast<float>(gltfMaterial.emissiveFactor.at(1));
        material.data_.emissiveFactor_[2] = static_cast<float>(gltfMaterial.emissiveFactor.at(2));

        material.data_.alphaMode_ = gltfMaterial.alphaMode == "OPAQUE" ?
            0 : gltfMaterial.alphaMode == "MASK" ? 1 : gltfMaterial.alphaMode == "BLEND" ? 2 : 0;
        material.data_.alphaCutoff_ = static_cast<float>(gltfMaterial.alphaCutoff);
        material.data_.doubleSided_ = gltfMaterial.doubleSided ? 1 : 0;

        material.data_.pbrMetallicRoughness_.baseColorFactor_[0] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(0));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[1] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(1));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[2] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(2));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[3] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(3));
        material.data_.pbrMetallicRoughness_.baseColorTexture_.index_ =
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
        material.data_.pbrMetallicRoughness_.baseColorTexture_.texcoord_ =
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
        material.data_.pbrMetallicRoughness_.metallicFactor_ =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
        material.data_.pbrMetallicRoughness_.roughnessFactor_ =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);
        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_ =
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.texcoord_ =
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

        material.data_.normalTexture_.index_ = gltfMaterial.normalTexture.index;
        material.data_.normalTexture_.texcoord_ = gltfMaterial.normalTexture.texCoord;
        material.data_.normalTexture_.scale_ = static_cast<float>(gltfMaterial.normalTexture.scale);

        material.data_.occlusionTexture_.index_ = gltfMaterial.occlusionTexture.index;
        material.data_.occlusionTexture_.texcoord_ = gltfMaterial.occlusionTexture.texCoord;
        material.data_.occlusionTexture_.strength_ =
            static_cast<float>(gltfMaterial.occlusionTexture.strength);

        material.data_.emissiveTexture_.index_ = gltfMaterial.emissiveTexture.index;
        material.data_.emissiveTexture_.texcoord_ = gltfMaterial.emissiveTexture.texCoord;
    }

    // Create material data as shader resource view on GPU
    std::vector<Material::Cbuffer> materialData;
    for (std::vector<Material>::const_reference material : materials_)
    {
        materialData.emplace_back(material.data_);
    }

    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Material::Cbuffer) * materialData.size());
    bufferDesc.StructureByteStride = sizeof(Material::Cbuffer);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = materialData.data();
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, materialBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(materialData.size());
    hr = device->CreateShaderResourceView(materialBuffer.Get(),
        &shaderResourceViewDesc, materialResourceView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// �e�N�X�`���̎擾
void GltfModel::FetchTexture(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    HRESULT hr{ S_OK };

    for (const tinygltf::Texture& gltfTexture : gltfModel.textures)
    {
        Texture& texture{ textures_.emplace_back() };
        texture.name_ = gltfTexture.name;
        texture.source_ = gltfTexture.source;
    }
    for (const tinygltf::Image& gltfImage : gltfModel.images)
    {
        Image& image{ images_.emplace_back() };
        image.name_ = gltfImage.name;
        image.width_ = gltfImage.width;
        image.height_ = gltfImage.height;
        image.component = gltfImage.component;
        image.bits_ = gltfImage.bits;
        image.pixelType_ = gltfImage.pixel_type;
        image.bufferView_ = gltfImage.bufferView;
        image.mimeType_ = gltfImage.mimeType;
        image.uri_ = gltfImage.uri;
        image.asIs_ = gltfImage.as_is;

        if (gltfImage.bufferView > -1)
        {
            const tinygltf::BufferView& bufferView{ gltfModel.bufferViews.at(gltfImage.bufferView) };
            const tinygltf::Buffer& buffer{ gltfModel.buffers.at(bufferView.buffer) };
            const byte* data = buffer.data.data() + bufferView.byteOffset;

            ID3D11ShaderResourceView* textureResourceView{};
            hr = LoadTextureFromMemory(device, data, bufferView.byteLength, &textureResourceView);
            if (hr == S_OK)
            {
                textureResourceViews_.emplace_back().Attach(textureResourceView);
            }
        }
        else
        {
            const std::filesystem::path path(filename_);
            ID3D11ShaderResourceView* shaderResourceView{};
            D3D11_TEXTURE2D_DESC texture2dDesc;
            std::wstring filename
            {
                path.parent_path().concat(L"/").wstring() +
                std::wstring(gltfImage.uri.begin(),gltfImage.uri.end())
            };
            hr = LoadTextureFromFile(device, filename.c_str(), &shaderResourceView, &texture2dDesc);
            if (hr == S_OK)
            {
                textureResourceViews_.emplace_back().Attach(shaderResourceView);
            }
        }
    }
}

// �A�j���[�V�����̎擾
void GltfModel::FetchAnimation(const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Skin>::const_reference transmissionSkin : gltfModel.skins)
    {
        Skin& skin{ skins_.emplace_back() };
        const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(transmissionSkin.inverseBindMatrices) };
        const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
        skin.inverseBindMatrices_.resize(gltfAccessor.count);
        std::memcpy(skin.inverseBindMatrices_.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
            gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT4X4));
        skin.joints_ = transmissionSkin.joints;
    }

    for (std::vector<tinygltf::Animation>::const_reference gltfAnimation : gltfModel.animations)
    {
        Animation& animation{ animations_.emplace_back() };
        animation.name_ = gltfAnimation.name;
        for (std::vector<tinygltf::AnimationSampler>::const_reference gltfSampler : gltfAnimation.samplers)
        {
            Animation::Sampler& sampler{ animation.samplers_.emplace_back() };
            sampler.input_ = gltfSampler.input;
            sampler.output_ = gltfSampler.output;
            sampler.interpolation_ = gltfSampler.interpolation;

            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfSampler.input) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
            std::pair<std::unordered_map<int, std::vector<float>>::iterator, bool>& timelines
            {
                animation.timelines_.emplace(gltfSampler.input, gltfAccessor.count)
            };
            if (timelines.second)
            {
                std::memcpy(timelines.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                    gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(FLOAT));
            }
        }

        for (std::vector<tinygltf::AnimationChannel>::const_reference gltfChannel : gltfAnimation.channels)
        {
            Animation::Channel& channel{ animation.channels_.emplace_back() };
            channel.sampler_ = gltfChannel.sampler;
            channel.targetNode_ = gltfChannel.target_node;
            channel.targetPath_ = gltfChannel.target_path;

            const tinygltf::AnimationSampler& gltfSampler{ gltfAnimation.samplers.at(gltfChannel.sampler) };
            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfSampler.output) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
            if (gltfChannel.target_path == "scale")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool>& scales
                {
                    animation.scales_.emplace(gltfSampler.output, gltfAccessor.count)
                };
                if (scales.second)
                {
                    std::memcpy(scales.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT3));
                }
            }
            else if (gltfChannel.target_path == "rotation")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT4>>::iterator, bool>& rotations
                {
                    animation.rotations_.emplace(gltfSampler.output, gltfAccessor.count)
                };
                if (rotations.second)
                {
                    std::memcpy(rotations.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT4));
                }
            }
            else if (gltfChannel.target_path == "translation")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool>& translations
                {
                    animation.translations_.emplace(gltfSampler.output,gltfAccessor.count)
                };
                if (translations.second)
                {
                    std::memcpy(translations.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT3));
                }
            }
        }
    }
}

void GltfModel::CumulateTransforms(std::vector<Node>& nodes)
{
    std::stack<DirectX::XMFLOAT4X4> parentGlobalTransforms;
    std::function<void(int)> traverse{ [&](int nodeIndex)->void
        {
            Node& node{nodes.at(nodeIndex)};
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(
            DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z) };
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));
            for (int childIndex : node.children_)
            {
                parentGlobalTransforms.push(node.globalTransform_);
                traverse(childIndex);
                parentGlobalTransforms.pop();
            }
    } };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        parentGlobalTransforms.push(
            {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
            }
        );
        traverse(nodeIndex);
        parentGlobalTransforms.pop();
    }
}

GltfModel::BufferView GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
    BufferView bufferView;
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            bufferView.format_ = DXGI_FORMAT_R16_UINT;
            bufferView.strideInBytes_ = sizeof(USHORT);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            bufferView.format_ = DXGI_FORMAT_R32_UINT;
            bufferView.strideInBytes_ = sizeof(UINT);
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC2:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC3:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC4:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            bufferView.format_ = DXGI_FORMAT_R16G16B16A16_UINT;
            bufferView.strideInBytes_ = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32A32_UINT;
            bufferView.strideInBytes_ = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 4;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not suppoeted.");
            break;
        }
        break;
    default:
        _ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
        break;
    }
    bufferView.sizeInBytes_ = static_cast<UINT>(accessor.count * bufferView.strideInBytes_);
    return bufferView;
}