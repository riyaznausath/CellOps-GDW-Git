#include "MorphAnimator.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

MorphAnimator::MorphAnimator()
	: IComponent(),
	switchClip(false),
	timer(0.0f)
{ }

MorphAnimator::~MorphAnimator() = default;

void MorphAnimator::Awake()
{
	thisObject = this->GetComponent<RenderComponent>()->GetMesh();
}

void MorphAnimator::Update(float deltaTime)
{

	if (switchClip)
	{
		timer = 0.0f;
		switchClip = false;
	}

	else
	{
		timer += deltaTime;
		
	}

	float t = timer / currentClip.frameDuration;

	if (t > 1)
	{
		t = 0;
		timer = 0.0f;
		currentClip.currentFrame++;
		currentClip.nextFrame++;
		if (currentClip.currentFrame == currentClip.frames.size())
		{
			currentClip.currentFrame = 0;
		}

		else if (currentClip.nextFrame == currentClip.frames.size())
		{
			currentClip.nextFrame = 0;
		}
	}

	
	std::vector<BufferAttribute> pos0 = currentClip.frames[currentClip.currentFrame]->Mesh->GetBufferBinding(AttribUsage::Position)->GetAttributes();
	std::vector<BufferAttribute> pos1 = currentClip.frames[currentClip.nextFrame]->Mesh->GetBufferBinding(AttribUsage::Position)->GetAttributes();

	
	pos0.resize(1);
	//Changed to slot of inPosition2
	pos1[0].Slot = static_cast<GLint>(4);
	pos1.resize(1);

	thisObject->AddVertexBuffer(currentClip.frames[currentClip.currentFrame]->Mesh->GetBufferBinding(AttribUsage::Position)->GetBuffer(), pos0);
	thisObject->AddVertexBuffer(currentClip.frames[currentClip.nextFrame]->Mesh->GetBufferBinding(AttribUsage::Position)->GetBuffer(), pos1);
	this->GetComponent<RenderComponent>()->GetMaterial()->Set("t", t);
}

void MorphAnimator::AddClip(std::vector<Gameplay::MeshResource::Sptr> inFrames, float dur, std::string inName)
{
	animInfo clip;

	//Make a temporary string
	std::string tempStr = "";

	//Take the input name
	for (int i = 0; i < inName.length(); i++)
	{
		tempStr += std::tolower(inName[i]);
	}

	//Assigned all the variables
	clip.animName = tempStr;
	clip.frames = inFrames;
	clip.frameDuration = dur;
	clip.currentFrame = 0;
	
	if (clip.frames.size() == 0) clip.nextFrame = 0;
	else clip.nextFrame = 1;

	animClips.push_back(clip);
}

void MorphAnimator::ActivateAnim(std::string name)
{
	std::string tempStr = "";

	for (int i = 0; i < name.length(); i++)
	{
		tempStr += std::tolower(name[i]);
	}

	for (int j = 0; j < animClips.size(); j++)
	{
		if (animClips[j].animName == tempStr)
		{
			currentClip = animClips[j];
			switchClip = true;
			return;
		}
	}

	std::cout << "No animation clip of this name: " << tempStr << std::endl;
}

void MorphAnimator::RenderImGui()
{
}

nlohmann::json MorphAnimator::ToJson() const
{
	return nlohmann::json();
}

MorphAnimator::Sptr MorphAnimator::FromJson(const nlohmann::json& blob)
{
	return MorphAnimator::Sptr();
}