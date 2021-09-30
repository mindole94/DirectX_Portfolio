#pragma once

class ModelBone
{
public:
	friend class Model;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }

	vector<ModelBone *>& Childs() { return childs; }

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	vector<ModelBone *> childs;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh
{
public:
	friend class Model;

public:
	ModelMesh();
	~ModelMesh();

	void Binding(Model* model);

public:
	void Pass(UINT val);
	void SetShader(Shader* shader);

	void Update();
	//void Render();
	void Render(UINT instanceCount);

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone() { return bone; }

	//void Transforms(Matrix* transforms);
	//void SetTransform(Transform* transform);

	void TransformsSRV(ID3D11ShaderResourceView* srv);

private:
	struct BoneDesc
	{
		//Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT Index;
		float Padding[3];
	} boneDesc;

private:
	wstring name;

	Shader* shader;

	//Transform* transform = NULL;
	PerFrame* perFrame = NULL;

	int boneIndex;
	class ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::ModelVertex* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;

	ID3D11ShaderResourceView* transformsSRV = NULL;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;

	vector<class ModelMeshPart *> meshParts;
};

