#include "TargetBehaviour.h"

TargetBehaviour::TargetBehaviour() :
	IComponent(),
	_health(0.0f),
	HealthInPercentage(0),
	_maxHealth(0.0f),
	HealthUiName(""),
	_isBeingAttacked(false),
	_coolDownAlertCounter(1000)
{
}

TargetBehaviour::~TargetBehaviour() = default;

void TargetBehaviour::Update(float deltaTime)
{
	HealthInPercentage = (_health * 100) / _maxHealth;
	Alert(_isBeingAttacked);
}
void TargetBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	if (body->GetGameObject()->Name.find("Enemy") != std::string::npos) {
		std::string EnemyType = body->GetGameObject()->Get<EnemyBehaviour>()->EnemyType;
		if (EnemyType == "Fast Enemy") {
			_health = _health - 1;
		}
		else if (EnemyType == "Normal Enemy") {
			_health = _health - 3;
		}
		else if (EnemyType == "Large Enemy") {
			_health = _health - 5;
		}
		if (_health <= 0) {
			HealthInPercentage = (_health * 100) / _maxHealth;
			GetGameObject()->GetScene()->DeleteTarget(GetGameObject()->SelfRef());
		}
		_isBeingAttacked = true;
		_coolDownAlertCounter = 1000;
	}
}

void TargetBehaviour::RenderImGui(){
	LABEL_LEFT(ImGui::DragFloat, "Health", &_health, 1.0f);
	LABEL_LEFT(ImGui::DragFloat, "MaxHealth", &_maxHealth, 1.0f);
	LABEL_LEFT(ImGui::Checkbox, "Is Being Attacked", &_isBeingAttacked);
	LABEL_LEFT(ImGui::DragInt, "Cool Down Timer", &_coolDownAlertCounter, 1.0f);

}

nlohmann::json TargetBehaviour::ToJson() const
{
	return {
		{"health",_health},
		{"MaxHealth",_maxHealth},
		{"Being Attacked",_isBeingAttacked},
		{"Cool Down Timer",_coolDownAlertCounter}
	};
}

TargetBehaviour::Sptr TargetBehaviour::FromJson(const nlohmann::json& blob)
{
	TargetBehaviour::Sptr result = std::make_shared<TargetBehaviour>();
	result->_health = blob["Health"];
	result->_maxHealth = blob["MaxHealth"];
	result->_isBeingAttacked = blob["Being Attacked"];
	result->_coolDownAlertCounter = blob["Cool Down Timer"];
	return result;
}

void TargetBehaviour::Heal()
{
	_maxHealth += 10;
	_health = _maxHealth;
	_isBeingAttacked = false;
}

void TargetBehaviour::TargetSetUp(float MaxHealth)
{
	_maxHealth = MaxHealth;
	_health = MaxHealth;
	HealthInPercentage = (_health * 100) / MaxHealth;
	_isBeingAttacked = false;

	//TODO: Particles
	//this->GetGameObject()->Add<ParticleSystem>()->AddEmitter(GetGameObject()->GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

void TargetBehaviour::Alert(bool Attackstatus)
{
	////Being attacked
	if (Attackstatus) {
		//this->GetGameObject()->Get<ParticleSystem>()->AddEmitter(GetGameObject()->GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		_coolDownAlertCounter--;
	}
	//_TargetParticles->AddEmitter(GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	////Being attacked
	//_TargetParticles->AddEmitter(GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}
