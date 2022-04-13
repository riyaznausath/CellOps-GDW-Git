#pragma once
#include "IComponent.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/TargetBehaviour.h"
#include <Gameplay/Components/MorphAnimator.h>
#include "Gameplay/Components/Light.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Scene.h"
#include <Utils/ImGuiHelper.h>
#include "Gameplay/Components/ParticleSystem.h"

class TargetController :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<TargetController> Sptr;

	TargetController();
	virtual ~TargetController();
	virtual void Update(float deltaTime) override;
	MAKE_TYPENAME(TargetController);
	virtual nlohmann::json ToJson() const override;
	static TargetController::Sptr FromJson(const nlohmann::json& blob);
	virtual void RenderImGui() override;

	//Names of Targets
	std::vector<std::string> TargetNames;

	//Target Position
	std::vector<glm::vec3> TargetPositions;

	//Materials stored in a vector
	std::vector<Gameplay::Material::Sptr> TargetMaterials;

	//Meshes stored in a vector
	std::vector<Gameplay::MeshResource::Sptr> TargetMeshs;

	//Animation
	std::vector<Gameplay::MeshResource::Sptr> TargetFrames;

	/// <summary>
	/// Spawns Targets Into Game
	/// </summary>
	void Spawntargets();
private:
	bool _isNotSafe;
	glm::vec3 _targetPosition;
};