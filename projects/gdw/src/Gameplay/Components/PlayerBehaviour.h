#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Utils/AudioEngine.h"

/// <summary>
/// Player Behaviour Class
/// </summary>
class PlayerBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<PlayerBehaviour> Sptr;

	PlayerBehaviour();
	virtual ~PlayerBehaviour();

	virtual void Update(float deltaTime) override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static PlayerBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(PlayerBehaviour);

	int EnemiesKilled;
	bool isAbilityActive;

	

protected:
	GLFWwindow* _window;
};