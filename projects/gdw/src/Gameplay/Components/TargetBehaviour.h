#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/EnemyBehaviour.h"
#include <Utils/ImGuiHelper.h>
#include <Gameplay/Components/GUI/GuiText.h>
#include <Gameplay/Components/GUI/GuiPanel.h>
#include "Gameplay/Components/ParticleSystem.h"

/// <summary>
/// Target Behaviour
/// </summary>
class TargetBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<TargetBehaviour> Sptr;


	TargetBehaviour();
	virtual ~TargetBehaviour();

	virtual void Update(float deltaTime) override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static TargetBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(TargetBehaviour);

	/// <summary>
	/// Heals Target at end of round.
	/// </summary>
	void Heal();
	/// <summary>
	/// Prepares Target status for game.
	/// </summary>
	/// <param name="MaxHealth">Max Health of target</param>
	void TargetSetUp(float MaxHealth);
	/// <summary>
	/// Changes things to make player alert of target being attacked
	/// </summary>
	/// <param name="Attackstatus">Boolean on if being attacked or not</param>
	void Alert(bool Attackstatus);

	int HealthInPercentage;
	std::string HealthUiName;

protected:
	float _maxHealth;
	float _health;
	bool _isBeingAttacked;
	int _coolDownAlertCounter;
	RenderComponent::Sptr _renderer;
};

