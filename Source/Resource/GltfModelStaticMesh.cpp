#include "GltfModelStaticMesh.h"
#include <stack>
#include <functional>
#include <filesystem>
#include "Graphics.h"
#include "Misc.h"
#include "Texture.h"

bool nullLoadImageData(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*)
{
	return true;
}

// ----- コンストラクタ -----
GltfModelStaticMesh::GltfModelStaticMesh(const std::string& filename)
{
    tinygltf::TinyGLTF tinyGltf;
	tinyGltf.SetImageLoader(nullLoadImageData, nullptr);

    tinygltf::Model gltfModel;
    std::string error, warning;
    bool succeeded = false;
    if (filename.find(".glb") != std::string::npos)
    {
        succeeded = tinyGltf.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str());
    }
    else if (filename.find(".glb") != std::string::npos)
    {
        succeeded = tinyGltf.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());
    _ASSERT_EXPR_A(error.empty(), error.c_str());
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");

    for (std::vector<tinygltf::Scene>::const_reference gltfScene : gltfModel.scenes)
    {
        Scene& scene = scenes_.emplace_back();
        scene.name_ = gltfModel.scenes.at(0).name;
        scene.nodes_ = gltfModel.scenes.at(0).nodes;
    }

    FetchNodes(gltfModel);
    FetchMeshes(gltfModel);
    FetchMaterials(gltfModel);
    FetchTextures(gltfModel);

    const std::map<std::string, BufferView>& vertexBufferViews = primitives_.at(0).vertexBufferView_;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, vertexBufferViews.at("POSITION").format_,   0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, vertexBufferViews.at("NORMAL").format_,     1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, vertexBufferViews.at("TANGENT").format_,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, vertexBufferViews.at("TEXCOORD_0").format_, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Graphics::Instance().CreateVsFromCso("./Resources/Shader/GltfModelStaticBatchingVS.cso", vertexShader_.ReleaseAndGetAddressOf(), inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/gltfModelPs.cso", pixelShader_.ReleaseAndGetAddressOf());

    primitiveConstantBuffer_ = std::make_unique<ConstantBuffer<PrimitiveConstants>>();
}

// ----- 描画 -----
void GltfModelStaticMesh::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	DirectX::XMMATRIX W = transform_.CalcWorldMatrix(scaleFactor);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, W);

	deviceContext->PSSetShaderResources(0, 1, materialResourceView_.GetAddressOf());
	deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
	psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
	deviceContext->IASetInputLayout(inputLayout_.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (decltype(primitives_)::const_reference primitive : primitives_)
	{
		ID3D11Buffer* vertexBuffers[] = {
			primitive.vertexBufferView_.at("POSITION").buffer_.Get(),
			primitive.vertexBufferView_.at("NORMAL").buffer_.Get(),
			primitive.vertexBufferView_.at("TANGENT").buffer_.Get(),
			primitive.vertexBufferView_.at("TEXCOORD_0").buffer_.Get(),
		};
		UINT strides[] = {
			static_cast<UINT>(primitive.vertexBufferView_.at("POSITION").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferView_.at("NORMAL").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferView_.at("TANGENT").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferView_.at("TEXCOORD_0").strideInBytes_),
		};
		UINT offsets[_countof(vertexBuffers)] = {};
		deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
		deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(), primitive.indexBufferView_.format_, 0);


		primitiveConstantBuffer_->GetData()->material_ = primitive.material_;
		primitiveConstantBuffer_->GetData()->hasTangent_ = primitive.vertexBufferView_.at("TANGENT").buffer_ != NULL;
		DirectX::XMStoreFloat4x4(&primitiveConstantBuffer_->GetData()->world_, DirectX::XMLoadFloat4x4(&world));		
		primitiveConstantBuffer_->Activate(0);

		const Material& material = materials_.at(primitive.material_);
		const int texture_indices[] =
		{
			material.data_.pbrMetallicRoughness_.baseColorTexture_.index_,
			material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
			material.data_.normalTexture_.index_,
			material.data_.emissiveTexture_.index_,
			material.data_.occlusionTexture_.index_,
		};
		ID3D11ShaderResourceView* null_shader_resource_view = {};
		std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
		for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
		{
			shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ? textureResourceViews_.at(textures_.at(texture_indices[texture_index]).source_).Get() : null_shader_resource_view;
		}
		deviceContext->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());

		deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexBufferView_.count()), 0, 0);
	}
}

// ----- ノードデータ取得 -----
void GltfModelStaticMesh::FetchNodes(const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Node>::const_reference gltfNode : gltfModel.nodes)
    {
		Node& node = nodes_.emplace_back();
		node.name_ = gltfNode.name;
		node.skin_ = gltfNode.skin;
		node.mesh_ = gltfNode.mesh;
		node.children_ = gltfNode.children;
		if (!gltfNode.matrix.empty())
		{
			DirectX::XMFLOAT4X4 matrix;
			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
				}
			}

			DirectX::XMVECTOR S, T, R;
			bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
			_ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

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

// ----- メッシュデータ取得 -----
void GltfModelStaticMesh::FetchMeshes(const tinygltf::Model& gltfModel)
{
	HRESULT result = S_OK;

	struct CombinedBuffer
	{
		size_t indexCount_;
		size_t vertexCount_;

		D3D_PRIMITIVE_TOPOLOGY topology_ = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		std::vector<unsigned int> indices_;
		struct StructureOfArrays
		{
			std::vector<DirectX::XMFLOAT3> positions_;
			std::vector<DirectX::XMFLOAT3> normals_;
			std::vector<DirectX::XMFLOAT4> tangents_;
			std::vector<DirectX::XMFLOAT2> texcoords_;
		};
		StructureOfArrays vertices_;
	};
	std::unordered_map<int/*material*/, CombinedBuffer> combinedBuffers;

	// Collect primitives with same material
	for (decltype(nodes_)::reference node : nodes_)
	{
		const DirectX::XMMATRIX transform = DirectX::XMLoadFloat4x4(&node.globalTransform_);

		if (node.mesh_ > -1)
		{
			const tinygltf::Mesh& gltfMesh = gltfModel.meshes.at(node.mesh_);

			for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
			{
				CombinedBuffer& combinedBuffer = combinedBuffers[gltfPrimitive.material];
				if (gltfPrimitive.indices > -1)
				{
					const tinygltf::Accessor gltfAccessor = gltfModel.accessors.at(gltfPrimitive.indices);
					const tinygltf::BufferView& gltfBufferView = gltfModel.bufferViews.at(gltfAccessor.bufferView);

					if (gltfAccessor.count == 0)
					{
						continue;
					}

					const size_t vertex_offset = combinedBuffer.vertices_.positions_.size();
					if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						const unsigned char* buffer = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							combinedBuffer.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertex_offset));
						}
					}
					else if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						const unsigned short* buffer = reinterpret_cast<const unsigned short*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							combinedBuffer.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertex_offset));
						}
					}
					else if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const unsigned int* buffer = reinterpret_cast<const unsigned int*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							combinedBuffer.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertex_offset));
						}
					}
					else
					{
						_ASSERT_EXPR(false, L"This index format is not supported.");
					}
				}

				// Combine primitives using the same material into a single vertex buffer. In addition, apply a coordinate transformation to position, normal and tangent of primitives.
				for (decltype(gltfPrimitive.attributes)::const_reference gltfAttribute : gltfPrimitive.attributes)
				{
					const tinygltf::Accessor gltfAccessor = gltfModel.accessors.at(gltfAttribute.second);
					const tinygltf::BufferView& gltfBufferView = gltfModel.bufferViews.at(gltfAccessor.bufferView);

					if (gltfAccessor.count == 0)
					{
						continue;
					}

					if (gltfAttribute.first == "POSITION")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC3, L"'POSITION' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const DirectX::XMFLOAT3* buffer = reinterpret_cast<const DirectX::XMFLOAT3*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							DirectX::XMFLOAT3 position = buffer[accessor_index];
							DirectX::XMStoreFloat3(&position, XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), transform));
							combinedBuffer.vertices_.positions_.emplace_back(position);
						}
					}
					else if (gltfAttribute.first == "NORMAL")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC3, L"'NORMAL' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const DirectX::XMFLOAT3* buffer = reinterpret_cast<const DirectX::XMFLOAT3*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							DirectX::XMFLOAT3 normal = buffer[accessor_index];
							DirectX::XMStoreFloat3(&normal, XMVector3TransformNormal(DirectX::XMLoadFloat3(&normal), transform));
							combinedBuffer.vertices_.normals_.emplace_back(normal);
						}
					}
					else if (gltfAttribute.first == "TANGENT")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC4, L"'TANGENT' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC4.");
						const DirectX::XMFLOAT4* buffer = reinterpret_cast<const DirectX::XMFLOAT4*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							DirectX::XMFLOAT4 tangent = buffer[accessor_index];
							float sigma = tangent.w;
							tangent.w = 0;
							DirectX::XMStoreFloat4(&tangent, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&tangent), transform));
							tangent.w = sigma;
							combinedBuffer.vertices_.tangents_.emplace_back(tangent);
						}
					}
					else if (gltfAttribute.first == "TEXCOORD_0")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
						const DirectX::XMFLOAT2* buffer = reinterpret_cast<const DirectX::XMFLOAT2*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < gltfAccessor.count; ++accessor_index)
						{
							combinedBuffer.vertices_.texcoords_.emplace_back(buffer[accessor_index]);
						}
					}
				}
			}
		}
	}

	ID3D11Device* device = Graphics::Instance().GetDevice();

	// Create GPU buffers
	for (decltype(combinedBuffers)::const_reference combinedBuffer : combinedBuffers)
	{
#if 1
		if (combinedBuffer.second.vertices_.positions_.size() == 0)
		{
			continue;
		}
#endif
		Primitive& primitive = primitives_.emplace_back();
		primitive.material_ = combinedBuffer.first;

		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		if (combinedBuffer.second.indices_.size() > 0)
		{
			primitive.indexBufferView_.format_ = DXGI_FORMAT_R32_UINT;
			primitive.indexBufferView_.strideInBytes_ = sizeof(UINT);
			primitive.indexBufferView_.sizeInBytes_ = combinedBuffer.second.indices_.size() * primitive.indexBufferView_.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(primitive.indexBufferView_.sizeInBytes_);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = combinedBuffer.second.indices_.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			result = device->CreateBuffer(&buffer_desc, &subresource_data, primitive.indexBufferView_.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
		}

		BufferView vertexBufferView;
		if (combinedBuffer.second.vertices_.positions_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices_.positions_.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices_.positions_.data();
			result = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
			primitive.vertexBufferView_.emplace(std::make_pair("POSITION", vertexBufferView));
		}
		if (combinedBuffer.second.vertices_.normals_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices_.normals_.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices_.normals_.data();
			result = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
			primitive.vertexBufferView_.emplace(std::make_pair("NORMAL", vertexBufferView));
		}
		if (combinedBuffer.second.vertices_.tangents_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 4;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices_.tangents_.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices_.tangents_.data();
			result = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
			primitive.vertexBufferView_.emplace(std::make_pair("TANGENT", vertexBufferView));
		}
		if (combinedBuffer.second.vertices_.texcoords_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 2;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices_.texcoords_.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices_.texcoords_.data();
			result = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
			primitive.vertexBufferView_.emplace(std::make_pair("TEXCOORD_0", vertexBufferView));
		}


		// Add dummy attributes if any are missing.
		const std::unordered_map<std::string, BufferView> attributes =
		{
			{ "POSITION", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "NORMAL", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
		};
		for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
		{
			if (primitive.vertexBufferView_.find(attribute.first) == primitive.vertexBufferView_.end())
			{
				primitive.vertexBufferView_.insert(std::make_pair(attribute.first, attribute.second));
			}
		}
	}
}

// ----- テクスチャデータ取得 -----
void GltfModelStaticMesh::FetchTextures(const tinygltf::Model& gltfModel)
{
	HRESULT result = S_OK;
	ID3D11Device* device = Graphics::Instance().GetDevice();

	for (const tinygltf::Texture& gltf_texture : gltfModel.textures)
	{
		TextureData& texture = textures_.emplace_back();
		texture.name_ = gltf_texture.name;
		texture.source_ = gltf_texture.source;
	}
	for (const tinygltf::Image& gltfImage : gltfModel.images)
	{
		Image& image = images_.emplace_back();
		image.name_ = gltfImage.name;
		image.width_ = gltfImage.width;
		image.height_ = gltfImage.height;
		image.component_ = gltfImage.component;
		image.bits_ = gltfImage.bits;
		image.pixelType_ = gltfImage.pixel_type;
		image.bufferView_ = gltfImage.bufferView;
		image.mimeType_ = gltfImage.mimeType;
		image.uri_ = gltfImage.uri;
		image.asIs_ = gltfImage.as_is;

		if (gltfImage.bufferView > -1)
		{
			const tinygltf::BufferView& bufferView = gltfModel.bufferViews.at(gltfImage.bufferView);
			const tinygltf::Buffer& buffer = gltfModel.buffers.at(bufferView.buffer);
			const byte* data = buffer.data.data() + bufferView.byteOffset;

			ID3D11ShaderResourceView* textureResourceView{};
			result = Texture::Instance().LoadTexture(data, bufferView.byteLength, &textureResourceView);
			if (result == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(textureResourceView);
			}
		}
		else
		{
			const std::filesystem::path path(filename_);
			ID3D11ShaderResourceView* shaderResourceView{};
			D3D11_TEXTURE2D_DESC texture2dDesc;
			std::wstring filename = path.parent_path().concat(L"/").wstring() + std::wstring(gltfImage.uri.begin(), gltfImage.uri.end());
			result = Texture::Instance().LoadTexture(filename.c_str(), &shaderResourceView, &texture2dDesc);
			if (result == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(shaderResourceView);
			}
		}
	}
}

// ----- マテリアルデータ取得 -----
void GltfModelStaticMesh::FetchMaterials(const tinygltf::Model& gltfModel)
{
	for (std::vector<tinygltf::Material>::const_reference gltfMaterial : gltfModel.materials)
	{
		std::vector<Material>::reference material = materials_.emplace_back();

		material.name_ = gltfMaterial.name;

		material.data_.emissiveFactor_[0] = static_cast<float>(gltfMaterial.emissiveFactor.at(0));
		material.data_.emissiveFactor_[1] = static_cast<float>(gltfMaterial.emissiveFactor.at(1));
		material.data_.emissiveFactor_[2] = static_cast<float>(gltfMaterial.emissiveFactor.at(2));

		material.data_.alphaMode_ = gltfMaterial.alphaMode == "OPAQUE" ? 0 : gltfMaterial.alphaMode == "MASK" ? 1 : gltfMaterial.alphaMode == "BLEND" ? 2 : 0;
		material.data_.alphaCutoff_ = static_cast<float>(gltfMaterial.alphaCutoff);
		material.data_.doubleSided_ = gltfMaterial.doubleSided ? 1 : 0;

		material.data_.pbrMetallicRoughness_.baseColorFactor_[0] = static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(0));
		material.data_.pbrMetallicRoughness_.baseColorFactor_[1] = static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(1));
		material.data_.pbrMetallicRoughness_.baseColorFactor_[2] = static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(2));
		material.data_.pbrMetallicRoughness_.baseColorFactor_[3] = static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(3));
		material.data_.pbrMetallicRoughness_.baseColorTexture_.index_ = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
		material.data_.pbrMetallicRoughness_.baseColorTexture_.texcoord_ = gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
		material.data_.pbrMetallicRoughness_.metallicFactor_ = static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
		material.data_.pbrMetallicRoughness_.roughnessFactor_ = static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);
		material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_ = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.texcoord_ = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

		material.data_.normalTexture_.index_ = gltfMaterial.normalTexture.index;
		material.data_.normalTexture_.texcoord_ = gltfMaterial.normalTexture.texCoord;
		material.data_.normalTexture_.scale_ = static_cast<float>(gltfMaterial.normalTexture.scale);

		material.data_.occlusionTexture_.index_ = gltfMaterial.occlusionTexture.index;
		material.data_.occlusionTexture_.texcoord_ = gltfMaterial.occlusionTexture.texCoord;
		material.data_.occlusionTexture_.strength_ = static_cast<float>(gltfMaterial.occlusionTexture.strength);

		material.data_.emissiveTexture_.index_ = gltfMaterial.emissiveTexture.index;
		material.data_.emissiveTexture_.texcoord_ = gltfMaterial.emissiveTexture.texCoord;
	}

	// Create material data as shader resource view on GPU
	std::vector<Material::CBuffer> materialData;
	for (std::vector<Material>::const_reference material : materials_)
	{
		materialData.emplace_back(material.data_);
	}

	HRESULT result = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Material::CBuffer) * materialData.size());
	buffer_desc.StructureByteStride = sizeof(Material::CBuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = materialData.data();
	result = Graphics::Instance().GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(materialData.size());
	result = Graphics::Instance().GetDevice()->CreateShaderResourceView(material_buffer.Get(), &shaderResourceViewDesc, materialResourceView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

void GltfModelStaticMesh::CumulateTransforms(std::vector<Node>& nodes)
{
	std::stack<DirectX::XMFLOAT4X4> parentGlobalTransforms;
	std::function<void(int)> traverse{ [&](int nodeIndex)->void
	{
		Node& node = nodes.at(nodeIndex);
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
		DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * XMLoadFloat4x4(&parentGlobalTransforms.top()));
		for (int childIndex : node.children_)
		{
			parentGlobalTransforms.push(node.globalTransform_);
			traverse(childIndex);
			parentGlobalTransforms.pop();
		}
	} };
	for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
	{
		parentGlobalTransforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
		traverse(nodeIndex);
		parentGlobalTransforms.pop();
	}
}

GltfModelStaticMesh::BufferView GltfModelStaticMesh::MakeBufferView(const tinygltf::Accessor& accessor)
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
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	default:
		_ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
		break;
	}
	bufferView.strideInBytes_ = static_cast<UINT>(accessor.count * bufferView.strideInBytes_);
	return bufferView;
}
