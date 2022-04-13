#pragma once
#include "IComponent.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/EnemyBehaviour.h"
#include <Gameplay/Components/MorphAnimator.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Scene.h"
#include <Utils/ImGuiHelper.h>

/// <summary>
/// Spawns Enemies
/// </summary>
class EnemySpawnerBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<EnemySpawnerBehaviour> Sptr;

	EnemySpawnerBehaviour();
	virtual ~EnemySpawnerBehaviour();
	virtual void Update(float deltaTime) override;
	MAKE_TYPENAME(EnemySpawnerBehaviour);
	virtual nlohmann::json ToJson() const override;
	static EnemySpawnerBehaviour::Sptr FromJson(const nlohmann::json& blob);
	virtual void RenderImGui() override;

	//Materials
	Gameplay::Material::Sptr LargeEnemyMaterial;
	Gameplay::Material::Sptr NormalEnemyMaterial;
	Gameplay::Material::Sptr FastEnemyMaterial;

	//Meshes
	Gameplay::MeshResource::Sptr LargeEnemyMesh;
	Gameplay::MeshResource::Sptr NormalEnemyMesh;
	Gameplay::MeshResource::Sptr FastEnemyMesh;

	//Animation
	std::vector<Gameplay::MeshResource::Sptr> LargeEnemyFrames;
	std::vector<Gameplay::MeshResource::Sptr> NormalEnemyFrames;
	std::vector<Gameplay::MeshResource::Sptr> FastEnemyFrames;

	/// <summary>
	/// Spawn Wave of Enemies
	/// </summary>
	/// <param name="LargeAmount">Amount of Large to Spawn</param>
	/// <param name="NomralAmount">Amount of Normal Enemies to Spawn</param>
	/// <param name="FastAmount">Amount of Fast Enemies to Spawn</param>
	void SpawnWave(int LargeAmount, int NormalAmount, int FastAmount);
	/// <summary>
	/// Increases Speed of Enemies.
	/// ATM only increase speed
	/// </summary>
	void IncreaseEnemySpeed();

private:
	int _largeAmount;
	int _normalAmount;
	int _fastAmount;
	int _totalAmount;
	int _spawned;

	//Enemy Stats
	float _largeEnemySpeed;
	float _normalEnemySpeed;
	float _fastEnemySpeed;

	/// <summary>
	/// Delay Counter
	/// Spawn in seconds example 500 = 5 seconds
	/// </summary>
	int _counter;

	/// <summary>
	/// If already in Enemy Spawning mode
	/// </summary>
	bool _isSpawning;

	/// <summary>
	/// Create Large Enemy
	/// </summary>
	void _createLargeEnemy();

	/// <summary>
	/// Create Normal Enemy
	/// </summary>
	void _createNormalEnemy();

	/// <summary>
	/// Create fast Enemy
	/// </summary>
	void _createFastEnemy();

	/// <summary>
	/// Spawns Enemies with seconds delay
	/// </summary>
	void _spawning();
};

