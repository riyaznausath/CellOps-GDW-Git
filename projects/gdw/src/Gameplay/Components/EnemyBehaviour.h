#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Utils/AudioEngine.h"

class EnemyBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<EnemyBehaviour> Sptr;

	EnemyBehaviour();
	virtual ~EnemyBehaviour();

	virtual void Awake() override;

public:
	virtual void RenderImGui() override;
	virtual void Update(float deltaTime) override;
	void Reset();
	MAKE_TYPENAME(EnemyBehaviour);
	virtual nlohmann::json ToJson() const override;
	static EnemyBehaviour::Sptr FromJson(const nlohmann::json& blob);
	/// <summary>
	/// Take 1 hp away from enemy
	/// </summary>
	void TakeDamage();

	/// <summary>
	/// Instantly kills Enemy
	/// </summary>
	void AbilityActiveDamage();

	float Health;
	float Speed;
	std::string EnemyType;
	Gameplay::GameObject::Sptr Target;

	glm::vec3 RespawnPosition;

	float lerpTimer = 0;
	float lerpTimerMax = 10.0f;

	//Audio 
	AudioEngine* audioEngine = AudioEngine::instance();

	/// <summary>
	/// Finds new target for enemy
	/// kind of rubber banding methods as this is called from scene to enemy 
	/// then back to scene and in reverse....oh well when in rome
	/// </summary>
	void NewTarget();

protected:
	float _dmg;
};