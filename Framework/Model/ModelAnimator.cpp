#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();

	tweenBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	sTweenBuffer = shader->AsConstantBuffer("CB_TweenFrame");

	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc) * MAX_MODEL_INSTANCE);
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendFrame");

	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		colors[i] = Math::RandomColor4();
	instanceColorBuffer = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2);

	//CreateComputeShader
	{
		computeShader = new Shader(L"30_GetMultiBones.fxo");

		inputWorldBuffer = new StructuredBuffer(worlds, sizeof(Matrix), MAX_MODEL_INSTANCE);
		sInputWorldSrv = computeShader->AsSRV("InputWorlds");

		inputBoneBuffer = new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sInputBoneSrv = computeShader->AsSRV("InputBones");


		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

		outputBuffer = new TextureBuffer(texture);
		sOutputUav = computeShader->AsUAV("Output");

		SafeRelease(texture);


		boneTransforms = new Matrix*[MAX_MODEL_INSTANCE];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
			boneTransforms[i] = new Matrix[MAX_MODEL_TRANSFORMS];

		sTransformSrv = computeShader->AsSRV("TransformsMap");

		sComputeTweenBuffer = computeShader->AsConstantBuffer("CB_TweenFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendFrame");
	}
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);

	SafeDeleteArray(clipTransforms);
	SafeRelease(texture);
	SafeRelease(srv);

	SafeDelete(tweenBuffer);
	SafeDelete(instanceBuffer);
	SafeDelete(instanceColorBuffer);

	SafeDelete(computeShader);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SafeDeleteArray(boneTransforms[i]);
	SafeDeleteArray(boneTransforms);

	SafeDelete(inputWorldBuffer);
	SafeDelete(inputBoneBuffer);
	SafeDelete(outputBuffer);
}

void ModelAnimator::Update()
{
	if (texture == NULL)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		inputBoneBuffer->CopyToInput(bones);
	}


	for (UINT i = 0; i < transforms.size(); i++)
	{
		if (blendDesc[i].Mode == 0)
			UpdateTweenMode(i);
		else if (blendDesc[i].Mode == 1)
			UpdateBlendMode(i);
		else if (blendDesc[i].Mode == 2)
			UpdateNormalMode(i);
	}


	tweenBuffer->Render();
	blendBuffer->Render();


	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeTweenBuffer->SetConstantBuffer(tweenBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		
		sTransformSrv->SetResource(srv);
		sInputWorldSrv->SetResource(inputWorldBuffer->SRV());
		sInputBoneSrv->SetResource(inputBoneBuffer->SRV());
		sOutputUav->SetUnorderedAccessView(outputBuffer->UAV());
		
		computeShader->Dispatch(0, 0, 1, MAX_MODEL_INSTANCE, 1);

		outputBuffer->CopyFromOutput();
	}
	frameTime = fmod(frameTime, (1.0f / frameRate));

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::UpdateTweenMode(UINT index)
{
	TweenDesc& desc = tweenDesc[index];
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	//���� �ִϸ��̼�
	{
		desc.Curr.RunningTime += Time::Delta();

		float time = 1.0f / clip->FrameRate() / desc.Curr.Speed;//1������ �ð�
		
		desc.Curr.Time = desc.Curr.RunningTime / time;

		if (modelPlayMode[index].isPlay == true)
		{
			if (desc.Next.Clip > -1)
			{
				desc.Curr.CurrFrame = desc.Curr.CurrFrame;
				desc.Curr.NextFrame = desc.Curr.CurrFrame;
			}
			else if (desc.Curr.Time >= 1.0f)//���� �������� ��������
			{
				desc.Curr.RunningTime = 0;

				desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
				desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();//TODO :desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount()
			}
		}
		if (modelPlayMode[index].isPause == true)
		{
			desc.Curr.CurrFrame = desc.Curr.CurrFrame;
			desc.Curr.NextFrame = desc.Curr.CurrFrame;
		}

		if (modelPlayMode[index].isStop == true)
		{
			desc.Curr.CurrFrame = 0;
			desc.Curr.NextFrame = 0;
		}
	}

	if (desc.Next.Clip > -1)//���� Ŭ���� �ִ� ���
	{
		ModelClip* nextClip = model->ClipByIndex(desc.Next.Clip);

		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime;// / clip->FrameRate();

		if (desc.TweenTime >= 1.0f)//���� ���������� �Ѿ��
		{
			desc.Curr = desc.Next;//���������� ���� �������� 

			//���� ���� �ʱ�ȭ
			desc.Next.Clip = -1;
			desc.Next.CurrFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0.0f;

			desc.ChangeTime = 0.0f;
			desc.TweenTime = 0.0f;
		}
		else
		{
			desc.Next.RunningTime += Time::Delta();
			
			float time = 1.0f / nextClip->FrameRate() / desc.Next.Speed;//1������ �ð�
			
			if (desc.Next.Time >= 1.0f)
			{
				desc.Next.RunningTime = 0;

				desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % nextClip->FrameCount();
				desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % nextClip->FrameCount();
			}

			desc.Next.Time = desc.Next.RunningTime / time;
		}
	}
}

void ModelAnimator::UpdateBlendMode(UINT index)
{
	BlendDesc& desc = blendDesc[index];

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clip[i].Clip);

		desc.Clip[i].RunningTime += Time::Delta();
		float time = 1.0f / clip->FrameRate() / desc.Clip[i].Speed;

		if (modelPlayMode[index].isPlay == true)
		{
			if (desc.Clip[i].Time >= 1.0f)//���� ������ ��������
			{
				desc.Clip[i].RunningTime = 0;

				desc.Clip[i].CurrFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount();
				desc.Clip[i].NextFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount();
			}
		}

		if (modelPlayMode[index].isPause = true)
		{
			desc.Clip[i].CurrFrame = desc.Clip[i].CurrFrame;
			desc.Clip[i].NextFrame = desc.Clip[i].CurrFrame;
		}

		if (modelPlayMode[index].isStop == true)
		{
			desc.Clip[i].CurrFrame = 0;
			desc.Clip[i].NextFrame = 0;
		}

		desc.Clip[i].Time = desc.Clip[i].RunningTime / time;
	}
}

void ModelAnimator::UpdateNormalMode(UINT index)
{
	TweenDesc& desc = tweenDesc[index];
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	//���� �ִϸ��̼�
	{
		desc.Curr.RunningTime += Time::Delta();

		float time = 1.0f / clip->FrameRate() / desc.Curr.Speed;//1������ �ð�

		desc.Curr.Time = desc.Curr.RunningTime / time;

		if (modelPlayMode[index].isPlay == true)
		{
			if (desc.Curr.Time >= 1.0f)//���� �������� ��������
			{
				desc.Curr.RunningTime = 0;

				desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
				desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();//TODO :desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount()
			}
		}
		if (modelPlayMode[index].isPause == true)
		{
			desc.Curr.CurrFrame = desc.Curr.CurrFrame;
			desc.Curr.NextFrame = desc.Curr.CurrFrame;
		}

		if (modelPlayMode[index].isStop == true)
		{
			desc.Curr.CurrFrame = 0;
			desc.Curr.NextFrame = 0;
		}
	}

	if (desc.Next.Clip > -1)//���� Ŭ���� �ִ� ���
	{
		desc.Curr = desc.Next;//���������� ���� �������� 

		//���� ���� �ʱ�ȭ
		desc.Next.Clip = -1;
		desc.Next.CurrFrame = 0;
		desc.Next.NextFrame = 0;
		desc.Next.Time = 0.5;
		desc.Next.RunningTime = 0.0f;

		desc.ChangeTime = 0.0f;
		desc.TweenTime = 0.0f;
	}
}

void ModelAnimator::Render()
{
	sTweenBuffer->SetConstantBuffer(tweenBuffer->Buffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());

	instanceBuffer->Render();
	instanceColorBuffer->Render();
	
	int index = 0;

	for (ModelMesh* mesh : model->Meshes())
	{
		if (modelPlayMode[index].bRender == false) continue;
		mesh->Render(transforms.size());
		index++;
	}
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::PlayTweenMode(UINT instance, UINT clip, float speed, float takeTime)
{
	blendDesc[instance].Mode = 0;

	tweenDesc[instance].TakeTime = takeTime;
	tweenDesc[instance].Next.Clip = clip;
	tweenDesc[instance].Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT instance, UINT clip1, UINT clip2, UINT clip3)
{
	blendDesc[instance].Mode = 1;

	blendDesc[instance].Clip[0].Clip = clip1;
	blendDesc[instance].Clip[1].Clip = clip2;
	blendDesc[instance].Clip[2].Clip = clip3;
}

void ModelAnimator::PlayNormalMode(UINT instance, UINT clip, float speed, float takeTime)
{
	blendDesc[instance].Mode = 2;

	tweenDesc[instance].TakeTime = takeTime;
	tweenDesc[instance].Next.Clip = clip;
	tweenDesc[instance].Next.Speed = speed;
}

void ModelAnimator::SetBlendAlpha(UINT instance, float alpha)
{
	alpha = Math::Clamp(alpha, 0.0f, 2.0f);

	blendDesc[instance].Alpha = alpha;
}

void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

Transform * ModelAnimator::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	return transform;
}

void ModelAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);


	inputWorldBuffer->CopyToInput(worlds);
}

void ModelAnimator::SetCurrFrameTweenMode(int instance, int frame)
{
	/*ModelClip* clip = model->ClipByIndex(tweenDesc[instance].Curr.Clip);
	tweenDesc[instance].Curr.CurrFrame = frame;
	tweenDesc[instance].Curr.NextFrame = (frame + 1) % clip->FrameCount();*/
	TweenDesc& desc = tweenDesc[instance];
	desc.Curr.CurrFrame = desc.Curr.CurrFrame;
	desc.Curr.NextFrame = desc.Curr.CurrFrame;
}

void ModelAnimator::ResetFrame(UINT instance)//, UINT clip)
{
	TweenDesc& desc = tweenDesc[instance];
	ModelClip* clip = model->ClipByIndex(tweenDesc[instance].Curr.Clip);

	desc.Curr.CurrFrame = desc.Next.CurrFrame;
	desc.Curr.NextFrame = desc.Curr.CurrFrame;
	//desc.Curr.Time = 0;
	//desc.Curr.RunningTime = 0.0f;

	desc.Next.Clip = -1;
	desc.Next.CurrFrame = 0;
	desc.Next.NextFrame = 0;
	desc.Next.Time = 0;
	desc.Next.RunningTime = 0.0f;

	desc.ChangeTime = 0.0f;
	desc.TweenTime = 0.0f;
}

Matrix ModelAnimator::GetAttachTransform(UINT index, UINT boneIndex)
{
	ID3D11Texture2D* texture = outputBuffer->Result();

	Matrix matrix;

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(&matrix, (BYTE *)subResource.pData + (index * subResource.RowPitch + boneIndex * sizeof(Matrix)), sizeof(Matrix));
	}
	D3D::GetDC()->Unmap(texture, 0);

	return matrix;
}

void ModelAnimator::GetAttachTransforms(UINT index, Matrix * outResult)
{
	ID3D11Texture2D* texture = outputBuffer->Result();

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(outResult, (BYTE *)subResource.pData + (index * subResource.RowPitch), sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
	}
	D3D::GetDC()->Unmap(texture, 0);
}

void ModelAnimator::CreateTexture()
{
	clipTransforms = new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);
	
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;
		//void* p = malloc(pageSize * model->ClipCount());
		void* p = VirtualAlloc(NULL, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

		//MEMORY_BASIC_INFORMATION memory;
		//VirtualQuery(p, &memory, sizeof(MEMORY_BASIC_INFORMATION));
		
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;

			for (UINT y = 0; y < MAX_MODEL_KEYFRAMES; y++)
			{
				void* temp = (BYTE *)p + MAX_MODEL_TRANSFORMS * y * sizeof(Matrix) + start;
				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[y], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}//for(c)


		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE *)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize;
		}
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));


		SafeDeleteArray(subResource);
		VirtualFree(p, 0, MEM_RELEASE);
	}

	//CreateSRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv));
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->TransformsSRV(srv);
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];
	
	ModelClip* clip = model->ClipByIndex(index);
	for (UINT f = 0; f < clip->FrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix parent;
			Matrix inv = bone->Transform();
			D3DXMatrixInverse(&inv, NULL, &inv);

			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];


			Matrix animation;
			ModelKeyframe* frame = clip->Keyframe(bone->Name());
			if (frame != NULL)
			{
				ModelKeyframeData& data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}

			bones[b] = animation * parent;
			clipTransforms[index].Transform[f][b] = inv * bones[b];
		}//for(b)
	}//for(f)
}