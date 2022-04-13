#pragma once
#include "IComponent.h"
#include <Utils/ImGuiHelper.h>
#include "Utils/AudioEngine.h"
#include <Gameplay/InputEngine.h>
#include <Gameplay/Components/SimpleCameraControl.h>
#include <Gameplay/Components/PlayerBehaviour.h>
#include <Gameplay/Scene.h>

/// <summary>
/// Ability Class
/// </summary>
class AbilityBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<AbilityBehaviour> Sptr;

	AbilityBehaviour();
	virtual ~AbilityBehaviour();

	MAKE_TYPENAME(AbilityBehaviour);
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static AbilityBehaviour::Sptr FromJson(const nlohmann::json& blob);

	//Audio 
	AudioEngine* audioEngine = AudioEngine::instance();
	
	/// <summary>
	/// Set Players ability
	/// Abilities are 
	/// Johnson & Johnson
	/// Moderna
	/// Pfizer-BioNTech
	/// </summary>
	/// <param name="Ability">Johnson & Johnson, Moderna or Pfizer-BioNTech</param>
	void SetPlayersAbilityChoice(std::string Ability);

	/// <summary>
	/// Returns players ability choise as a string
	/// </summary>
	/// <returns></returns>
	std::string GetPlayersAbilityChoice() const;

private:
	bool _isAbilityActive;
	int _abilityIndex;
	/// <summary>
	/// Time till Ability avaible again
	/// </summary>
	float _coolDownTimer;

	/// <summary>
	/// Timer to turn off ability after set time
	/// </summary>
	float _abilityActiveCounter;
	
	/// <summary>
	/// Pfizer-BioNTech
	/// Increase Speed Boost 
	/// </summary>
	void _pfizerBioNTech();

	/// <summary>
	/// Moderna Ability
	/// Both abilities combine but longer cool down.
	/// </summary>
	void _moderna();

	/// <summary>
	/// Johnson and Johnson Ability
	/// Player can one hit all Enemies
	/// </summary>
	void _johnsonJohnson();
};

