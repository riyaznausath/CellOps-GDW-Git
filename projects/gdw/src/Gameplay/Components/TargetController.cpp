#include "TargetController.h"
#include "Utils/ResourceManager/ResourceManager.h"

TargetController::TargetController():
	IComponent(),
	TargetNames(std::vector<std::string>()),
	TargetPositions(std::vector<glm::vec3>()),
	TargetMeshs(std::vector<Gameplay::MeshResource::Sptr>()),
	TargetMaterials(std::vector<Gameplay::Material::Sptr>()),
	TargetFrames(std::vector<Gameplay::MeshResource::Sptr>()),
	_isNotSafe(true),
	_targetPosition()

{
}

TargetController::~TargetController() = default;

void TargetController::Update(float deltaTime)
{
}
nlohmann::json TargetController::ToJson() const
{
	std::vector<std::string> _meshInString;
	std::vector<std::string> _materialnString;
	std::vector<std::string> _framesInString;
	for (auto targetmesh : TargetMeshs) {
		_meshInString.push_back(targetmesh->GetGUID().str());
	}
	for (auto targetmaterials : TargetMaterials) {
		_materialnString.push_back(targetmaterials->GetGUID().str());
	}
	for (auto targetframes : TargetFrames) {
		_framesInString.push_back(targetframes->GetGUID().str());
	}
	return {
		{ "TargetNames", TargetNames},
		{ "TargetPositions", TargetPositions},
		{ "TargetMeshs", _meshInString},
		{ "TargetMaterials", _materialnString},
		{ "TargetFrames", _framesInString}
	};
}
TargetController::Sptr TargetController::FromJson(const nlohmann::json& blob)
{
	TargetController::Sptr result = std::make_shared<TargetController>();
	result->TargetNames = JsonGet(blob, "TargetNames", result->TargetNames);
	result->TargetPositions = JsonGet(blob, "TargetPositions", result->TargetPositions);
	for (std::string meshGUID : blob["TargetMeshs"]) {
		result->TargetMeshs.push_back(ResourceManager::Get<Gameplay::MeshResource>(Guid(meshGUID)));
	};
	for (std::string materialGUID : blob["TargetMaterials"]) {
		result->TargetMaterials.push_back(ResourceManager::Get<Gameplay::Material>(Guid(materialGUID)));
	};
	for (std::string frameGUID : blob["TargetFrames"]) {
		result->TargetFrames.push_back(ResourceManager::Get<Gameplay::MeshResource>(Guid(frameGUID)));
	};
	return result;
}
void TargetController::RenderImGui()
{
}
void TargetController::Spawntargets()
{
	for (int i = 0; i < TargetNames.size(); i++) {
		Gameplay::GameObject::Sptr Target = GetGameObject()->GetScene()->CreateGameObject(TargetNames[i]);
		{
			Target->SetPostion(TargetPositions[i]);;
			
			Target->Add<ParticleSystem>();
			Target->Get<ParticleSystem>()->AddEmitter(Target->GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			
			//Target->Add<Light>();
		//	Target->Get<Light>()->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));

			// Add a render component
			RenderComponent::Sptr renderer = Target->Add<RenderComponent>();
			renderer->SetMesh(TargetMeshs[i]);
			renderer->SetMaterial(TargetMaterials[i]);

			Gameplay::Physics::TriggerVolume::Sptr volume = Target->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::ConvexMeshCollider::Sptr collider = Gameplay::Physics::ConvexMeshCollider::Create();
			volume->AddCollider(collider);

			Target->Add<TargetBehaviour>();
			Target->Get<TargetBehaviour>()->TargetSetUp(100);

			GetGameObject()->GetScene()->Targets.push_back(Target);
			GetGameObject()->GetScene()->FindObjectByName("List Of Targets")->AddChild(Target);
		}
		_isNotSafe = true;
	}
}
