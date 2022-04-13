#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)
#include <GLM/gtc/random.hpp>

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"
#include "Utils/AudioEngine.h"
#include <Utils/AudioSource.h>

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Components/Light.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/EnemySpawnerBehaviour.h"
#include "Gameplay/Components/PlayerBehaviour.h"
#include "Gameplay/Components/EnemyBehaviour.h"
#include "Gameplay/Components/TargetBehaviour.h"
#include "Gameplay/Components/BackgroundObjectsBehaviour.h"
#include "Gameplay/Components/MorphAnimator.h"
#include "Gameplay/Components/UIController.h"
#include "Gameplay/Components/TargetController.h"
#include <Gameplay/Components/AbilityBehaviour.h>

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"



DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		////////////////////////////// SHADERS  ////////////////////////////////
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr basicShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});

		ShaderProgram::Sptr BackgroundShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/animation.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_animation.glsl" }
		});
		ShaderProgram::Sptr BreathingShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/breathing.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_shader.glsl" }
		});
		ShaderProgram::Sptr AnimationShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/Morph.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});
		ShaderProgram::Sptr Animation2Shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/Morph.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_animation.glsl" }
		});

		/////////////////////////////////////////// MESHES ////////////////////////////////////////////////
		// Load in the meshes
		MeshResource::Sptr PlayerMesh = ResourceManager::CreateAsset<MeshResource>("models/Player.obj");

		// Enemy Meshes
		MeshResource::Sptr LargeEnemyMesh = ResourceManager::CreateAsset<MeshResource>("models/LargeEnemy/LargeEnemy_001.obj");
		MeshResource::Sptr FastEnemyMesh = ResourceManager::CreateAsset<MeshResource>("models/Fast Enemy.obj");
		MeshResource::Sptr NormalEnemyMesh = ResourceManager::CreateAsset<MeshResource>("models/NormalIdle/NormalEnemy_001.obj");

		// Target Mesh
		MeshResource::Sptr LeftLungMesh = ResourceManager::CreateAsset<MeshResource>("models/LeftLung.obj");
		MeshResource::Sptr RightLungMesh = ResourceManager::CreateAsset<MeshResource>("models/RightLung.obj");
		MeshResource::Sptr HeartMesh = ResourceManager::CreateAsset<MeshResource>("models/Heart.obj");
		MeshResource::Sptr KidneyMesh = ResourceManager::CreateAsset<MeshResource>("models/Kidney.obj");

		// Background Meshes
		MeshResource::Sptr APCMesh = ResourceManager::CreateAsset<MeshResource>("models/APC.obj");
		MeshResource::Sptr APC2Mesh = ResourceManager::CreateAsset<MeshResource>("models/APC2.obj");
		MeshResource::Sptr BronchiMesh = ResourceManager::CreateAsset<MeshResource>("models/Bronchi.obj");
		MeshResource::Sptr CellMesh = ResourceManager::CreateAsset<MeshResource>("models/Cell.obj");
		MeshResource::Sptr Cell2Mesh = ResourceManager::CreateAsset<MeshResource>("models/Cell2.obj");
		MeshResource::Sptr Co2Mesh = ResourceManager::CreateAsset<MeshResource>("models/Co2.obj");
		MeshResource::Sptr LL37Mesh = ResourceManager::CreateAsset<MeshResource>("models/LL37.obj");
		MeshResource::Sptr McaMesh = ResourceManager::CreateAsset<MeshResource>("models/Mca.obj");
		MeshResource::Sptr MicrobiotaMesh = ResourceManager::CreateAsset<MeshResource>("models/Microbiota.obj");
		MeshResource::Sptr NewGermMesh = ResourceManager::CreateAsset<MeshResource>("models/New Germ.obj");
		MeshResource::Sptr OxygenMesh = ResourceManager::CreateAsset<MeshResource>("models/Oxygen.obj");
		MeshResource::Sptr PipeMesh = ResourceManager::CreateAsset<MeshResource>("models/Pipe.obj");
		MeshResource::Sptr SmokeplaqueMesh = ResourceManager::CreateAsset<MeshResource>("models/Smoke plaque.obj");
		MeshResource::Sptr SymbiontMesh = ResourceManager::CreateAsset<MeshResource>("models/Symbiont.obj");
		MeshResource::Sptr Symbiont2Mesh = ResourceManager::CreateAsset<MeshResource>("models/Symbiont2.obj");
		MeshResource::Sptr VeinMesh = ResourceManager::CreateAsset<MeshResource>("models/Vein.obj");
		MeshResource::Sptr VeinStickMesh = ResourceManager::CreateAsset<MeshResource>("models/VeinStick.obj");
		MeshResource::Sptr VeinYMesh = ResourceManager::CreateAsset<MeshResource>("models/VeinY.obj");
		MeshResource::Sptr WhiteBloodCellMesh = ResourceManager::CreateAsset<MeshResource>("models/White Blood Cell.obj");
		MeshResource::Sptr WhiteBloodCell2Mesh = ResourceManager::CreateAsset<MeshResource>("models/White Blood Cell2.obj");
		MeshResource::Sptr YellowMicrobiotaMesh = ResourceManager::CreateAsset<MeshResource>("models/YellowMicrobiota.obj");

		/////////////////////////////////////////// TEXTURES ////////////////////////////////////////////////
		// Load in some textures
		Texture2D::Sptr PlayerTexture = ResourceManager::CreateAsset<Texture2D>("textures/tempWhiteCell.jpg");

		// Enemy Textures
		Texture2D::Sptr	LargeEnemyTexture = ResourceManager::CreateAsset<Texture2D>("textures/Large Enemy.png");
		Texture2D::Sptr	FastEnemyTexture = ResourceManager::CreateAsset<Texture2D>("textures/Fast Enemy.png");
		Texture2D::Sptr	NormalEnemyTexture = ResourceManager::CreateAsset<Texture2D>("textures/Normal Enemy.png");

		// Target Textures
		Texture2D::Sptr	HeartTexture = ResourceManager::CreateAsset<Texture2D>("textures/Heart.jpg");
		Texture2D::Sptr	KidneyTexture = ResourceManager::CreateAsset<Texture2D>("textures/Kidney.png");
		Texture2D::Sptr	RightLungTexture = ResourceManager::CreateAsset<Texture2D>("textures/LungTexture.jpg");
		Texture2D::Sptr	LeftLungTexture = ResourceManager::CreateAsset<Texture2D>("textures/LungTexture.jpg");

		// Background Texture
		Texture2D::Sptr	APCTexture = ResourceManager::CreateAsset<Texture2D>("textures/APC.png");
		Texture2D::Sptr	APC2Texture = ResourceManager::CreateAsset<Texture2D>("textures/APC2.png");
		Texture2D::Sptr	BronchiTexture = ResourceManager::CreateAsset<Texture2D>("textures/Bronchi.png");
		Texture2D::Sptr	CellTexture = ResourceManager::CreateAsset<Texture2D>("textures/Cell.png");
		Texture2D::Sptr	Cell2Texture = ResourceManager::CreateAsset<Texture2D>("textures/Cell2.png");
		Texture2D::Sptr	Co2Texture = ResourceManager::CreateAsset<Texture2D>("textures/Co2.png");
		Texture2D::Sptr	FloorVeinANDVeinTexture = ResourceManager::CreateAsset<Texture2D>("textures/FloorVeinANDVein.png");
		Texture2D::Sptr	LL37Texture = ResourceManager::CreateAsset<Texture2D>("textures/LL37.png");
		Texture2D::Sptr	McaTexture = ResourceManager::CreateAsset<Texture2D>("textures/Mca.png");
		Texture2D::Sptr	MicrotbiotaTexture = ResourceManager::CreateAsset<Texture2D>("textures/Microbiota.png");
		Texture2D::Sptr	NewGermTexture = ResourceManager::CreateAsset<Texture2D>("textures/NewGerm.png");
		Texture2D::Sptr	OxygenTexture = ResourceManager::CreateAsset<Texture2D>("textures/Oxygen.png");
		Texture2D::Sptr	PipeTexture = ResourceManager::CreateAsset<Texture2D>("textures/Pipe.png");
		Texture2D::Sptr SmokeplaqueTexture = ResourceManager::CreateAsset<Texture2D>("textures/Smokeplaque.png");
		Texture2D::Sptr	SymbiontTexture = ResourceManager::CreateAsset<Texture2D>("textures/Symbiont.png");
		Texture2D::Sptr	Symbiont2Texture = ResourceManager::CreateAsset<Texture2D>("textures/Symbiont2.png");
		Texture2D::Sptr WhiteBloodCellTexture = ResourceManager::CreateAsset<Texture2D>("textures/White Blood Cell.png");
		Texture2D::Sptr WhiteBloodCell2Texture = ResourceManager::CreateAsset<Texture2D>("textures/White Blood Cell2.png");
		Texture2D::Sptr YellowMBiotaTexture = ResourceManager::CreateAsset<Texture2D>("textures/YellowMBiota.png");
		// UI Textures
		//TitleV
		Texture2D::Sptr TitleTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Title/Title.png");

		//Tutorial
		Texture2D::Sptr Tutorial1Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/Tutorial/tutorial1.png");
		Texture2D::Sptr Tutorial2Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/Tutorial/tutorial2.png");

		//GameOver
		Texture2D::Sptr GameOverTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Game Over Screen/GameOver.png");

		//Game Win
		Texture2D::Sptr GameWinTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Game Win Screen/GameWin.png");

		//GamePause
		Texture2D::Sptr GamePauseTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Game Pause Screen/GamePause.png");
		Texture2D::Sptr Tutorial1PauseTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Tutorial/tutorial1_pause.png");
		Texture2D::Sptr Tutorial2PauseTexture = ResourceManager::CreateAsset<Texture2D>("ui assets/Tutorial/tutorial2_pause.png");


		//Health
		Texture2D::Sptr Health100Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_100.png");
		Texture2D::Sptr Health90Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_90.png");
		Texture2D::Sptr Health80Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_80.png");
		Texture2D::Sptr Health70Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_70.png");
		Texture2D::Sptr Health60Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_60.png");
		Texture2D::Sptr Health50Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_50.png");
		Texture2D::Sptr Health40Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_40.png");
		Texture2D::Sptr Health30Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_30.png");
		Texture2D::Sptr Health20Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_20.png");
		Texture2D::Sptr Health10Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_10.png");
		Texture2D::Sptr Health0Texture = ResourceManager::CreateAsset<Texture2D>("ui assets/TargetHealth/Health_0.png");


		//Audio Files
		AudioEngine* audioEngine = AudioEngine::instance();
		audioEngine->init();

		audioEngine->loadSound("AbilityNotReady", "sounds/AbilityNotReady.wav", true);
		audioEngine->loadSound("AbilityReady", "sounds/AbilityReady.wav",true);
		audioEngine->loadSound("AbilityModerna", "sounds/ModernaPowerUp.wav", true);
		audioEngine->loadSound("AbilityPfizer-BioNTech", "sounds/AbilityPfizer-BioNTech.mp3",false);
		audioEngine->loadSound("AbilityJohnson&Johnson", "sounds/JJPowerUp.wav", false);
		audioEngine->loadSound("background", "sounds/Background(AiryHeartBeat).mp3", true,true,true);
		audioEngine->loadSound("menuBackground", "sounds/MenuBackgroundSound2.mp3", true,true);
		audioEngine->loadSound("targetDeath", "sounds/Background(AiryHeartBeat.Fast).mp3", true);
		audioEngine->loadSound("GameLose", "sounds/Background(AiryHeartBeat.FlatLine).mp3", true);
		audioEngine->loadSound("enemyDeath", "sounds/EnemyDeathSFX.mp3", true);
		audioEngine->loadSound("enemyHit", "sounds/HitFireSFX.mp3", true);
		AudioSource sound;

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/lung.png");
		ShaderProgram::Sptr skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		std::vector<MeshResource::Sptr> LargeEnemyFrames;

		for (int i = 1; i < 5; i++) {
			LargeEnemyFrames.push_back(ResourceManager::CreateAsset<MeshResource>("models/LargeEnemy/LargeEnemy_00" + std::to_string(i) + ".obj"));
		}

		std::vector<MeshResource::Sptr> NormalEnemyFrames;

		for (int i = 1; i < 5; i++) {
			NormalEnemyFrames.push_back(ResourceManager::CreateAsset<MeshResource>("models/NormalIdle/NormalEnemy_00" + std::to_string(i) + ".obj"));
		}

		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>();

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Create our materials
		// Player Material
		Material::Sptr PlayerMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			PlayerMaterial->Name = "PlayerMaterial";
			PlayerMaterial->Set("u_Material.Diffuse", PlayerTexture);
			PlayerMaterial->Set("u_Material.Shininess", 0.1f);
		}
		// Enemy Materials
		Material::Sptr LargeEnemyMaterial = ResourceManager::CreateAsset<Material>(AnimationShader);
		{
			LargeEnemyMaterial->Name = "LargeEnemyMaterial";
			LargeEnemyMaterial->Set("u_Material.Diffuse", LargeEnemyTexture);
			LargeEnemyMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr NormalEnemyMaterial = ResourceManager::CreateAsset<Material>(AnimationShader);
		{
			NormalEnemyMaterial->Name = "NormalEnemyMaterial";
			NormalEnemyMaterial->Set("u_Material.Diffuse", NormalEnemyTexture);
			NormalEnemyMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr FastEnemyMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			FastEnemyMaterial->Name = "FastEnemyMaterial";
			FastEnemyMaterial->Set("u_Material.Diffuse", FastEnemyTexture);
			FastEnemyMaterial->Set("u_Material.Shininess", 0.1f);
		}
		// Target Material
		Material::Sptr LeftLungMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			LeftLungMaterial->Name = "LeftLungMaterial";
			LeftLungMaterial->Set("u_Material.Diffuse", LeftLungTexture);
			LeftLungMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr RightLungMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			RightLungMaterial->Name = "LeftLungMaterial";
			RightLungMaterial->Set("u_Material.Diffuse", RightLungTexture);
			RightLungMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr HeartMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			HeartMaterial->Name = "HeartMaterial";
			HeartMaterial->Set("u_Material.Diffuse", HeartTexture);
			HeartMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr KidneyMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			KidneyMaterial->Name = "KidneyMateriall";
			KidneyMaterial->Set("u_Material.Diffuse", KidneyTexture);
			KidneyMaterial->Set("u_Material.Shininess", 0.1f);
		}

		// Background Materials
		Material::Sptr APCMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			APCMaterial->Name = "APCMaterial";
			APCMaterial->Set("u_Material.Diffuse", APCTexture);
			APCMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr APC2Material = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			APC2Material->Name = "APC2Material";
			APC2Material->Set("u_Material.Diffuse", APC2Texture);
			APC2Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr BronchiMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			BronchiMaterial->Name = "BronchiMateriall";
			BronchiMaterial->Set("u_Material.Diffuse", BronchiTexture);
			BronchiMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr CellMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			CellMaterial->Name = "CellMateriall";
			CellMaterial->Set("u_Material.Diffuse", CellTexture);
			CellMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr Cell2Material = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			Cell2Material->Name = "Cell2Materiall";
			Cell2Material->Set("u_Material.Diffuse", Cell2Texture);
			Cell2Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr Co2Material = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			Co2Material->Name = "Co2Material";
			Co2Material->Set("u_Material.Diffuse", Co2Texture);
			Co2Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr FloorVeinANDVeinMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			FloorVeinANDVeinMaterial->Name = "FloorVeinANDVeinMaterial";
			FloorVeinANDVeinMaterial->Set("u_Material.Diffuse", FloorVeinANDVeinTexture);
			FloorVeinANDVeinMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr LL37Material = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			LL37Material->Name = "LL37Material";
			LL37Material->Set("u_Material.Diffuse", LL37Texture);
			LL37Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr McaMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			McaMaterial->Name = "McaMaterial";
			McaMaterial->Set("u_Material.Diffuse", McaTexture);
			McaMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr MicrobiotaMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			MicrobiotaMaterial->Name = "MicrobiotaMaterial";
			MicrobiotaMaterial->Set("u_Material.Diffuse", MicrotbiotaTexture);
			MicrobiotaMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr NewGermMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			NewGermMaterial->Name = "NewGermMaterial";
			NewGermMaterial->Set("u_Material.Diffuse", NewGermTexture);
			NewGermMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr OxygenMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			OxygenMaterial->Name = "OxygenMaterial";
			OxygenMaterial->Set("u_Material.Diffuse", OxygenTexture);
			OxygenMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr PipeMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			PipeMaterial->Name = "PipeMaterial";
			PipeMaterial->Set("u_Material.Diffuse", PipeTexture);
			PipeMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr SmokeplaqueMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			SmokeplaqueMaterial->Name = "SmokeplaqueMaterial";
			SmokeplaqueMaterial->Set("u_Material.Diffuse", SmokeplaqueTexture);
			SmokeplaqueMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr SymbiontMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			SymbiontMaterial->Name = "SymbiontMaterial";
			SymbiontMaterial->Set("u_Material.Diffuse", SymbiontTexture);
			SymbiontMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr Symbiont2Material = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			Symbiont2Material->Name = "Symbiont2Material";
			Symbiont2Material->Set("u_Material.Diffuse", Symbiont2Texture);
			Symbiont2Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr WhiteBloodCellMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			WhiteBloodCellMaterial->Name = "WhiteBloodCellMaterial";
			WhiteBloodCellMaterial->Set("u_Material.Diffuse", WhiteBloodCellTexture);
			WhiteBloodCellMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr WhiteBloodCell2Material = ResourceManager::CreateAsset<Material>(basicShader);
		{
			WhiteBloodCell2Material->Name = "WhiteBloodCell2Material";
			WhiteBloodCell2Material->Set("u_Material.Diffuse", WhiteBloodCell2Texture);
			WhiteBloodCell2Material->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr YellowMicrobiotaMaterial = ResourceManager::CreateAsset<Material>(BackgroundShader);
		{
			YellowMicrobiotaMaterial->Name = "YellowMicrobiotaMaterial";
			YellowMicrobiotaMaterial->Set("u_Material.Diffuse", YellowMBiotaTexture);
			YellowMicrobiotaMaterial->Set("u_Material.Shininess", 0.1f);
		}
		// UI Material
		Material::Sptr GameOverMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			GameOverMaterial->Name = "GameOverMaterial";
			GameOverMaterial->Set("u_Material.Diffuse", GameOverTexture);
			GameOverMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr GameWinMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			GameWinMaterial->Name = "GameWinMaterial";
			GameWinMaterial->Set("u_Material.Diffuse", GameWinTexture);
			GameWinMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr GamePauseMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			GamePauseMaterial->Name = "GamePauseMaterial";
			GamePauseMaterial->Set("u_Material.Diffuse", GamePauseTexture);
			GamePauseMaterial->Set("u_Material.Shininess", 0.1f);
		}

		//// Create some lights for our scene
		/*GameObject::Sptr lightParent = scene->CreateGameObject("Lights");

		for (int ix = 0; ix < 100; ix++) {
			GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(glm::diskRand(25.0f), 1.0f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)));
			lightComponent->SetRadius(glm::linearRand(0.1f, 10.0f));
			lightComponent->SetIntensity(glm::linearRand(1.0f, 2.0f));
		}*/


		
		
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		//GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(10.0f,35.0f,15.0f));
			camera->LookAt(glm::vec3(1, -12, 1));
			camera->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));

			camera->Add<SimpleCameraControl>();

			//Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			//scene->MainCamera = cam;
		}
		GameObject::Sptr Player = scene->CreateGameObject("Player");
		{
			// Add a render component
			RenderComponent::Sptr renderer = Player->Add<RenderComponent>();

			renderer->SetMesh(PlayerMesh);
			renderer->SetMaterial(PlayerMaterial);


			Player->Add<PlayerBehaviour>();

			Player->Add<AbilityBehaviour>();


			TriggerVolume::Sptr trigger = Player->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(glm::vec3(-0.28f, 0.0f, -1.17f));
			collider->SetScale(glm::vec3(0.79f, 0.45f, 2.04f));

			trigger->AddCollider(collider);
		}
		/////////////////////////TARGETS//////////////////////////
		GameObject::Sptr ListOfTargets = scene->CreateGameObject("List Of Targets");

		GameObject::Sptr TargetSpawner = scene->CreateGameObject("Target Spawner");
		{
			TargetSpawner->Add<TargetController>();

			TargetSpawner->Get<TargetController>()->TargetNames.push_back("Left Lung");
			TargetSpawner->Get<TargetController>()->TargetPositions.push_back(glm::vec3(50.0f, 0.0f, 0.0));
			TargetSpawner->Get<TargetController>()->TargetMeshs.push_back(LeftLungMesh);
			TargetSpawner->Get<TargetController>()->TargetMaterials.push_back(LeftLungMaterial);
			//TargetSpawner->Get<TargetController>()->TargetFrames.push_back(LeftLungFrames);

			TargetSpawner->Get<TargetController>()->TargetNames.push_back("Right Lung");
			TargetSpawner->Get<TargetController>()->TargetPositions.push_back(glm::vec3(-50.0f, 0.0f, 0.0));
			TargetSpawner->Get<TargetController>()->TargetMeshs.push_back(RightLungMesh);
			TargetSpawner->Get<TargetController>()->TargetMaterials.push_back(RightLungMaterial);
			//TargetSpawner->Get<TargetController>()->TargetFrames.push_back(RightLungFrames);

			TargetSpawner->Get<TargetController>()->TargetNames.push_back("Heart");
			TargetSpawner->Get<TargetController>()->TargetPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0));
			TargetSpawner->Get<TargetController>()->TargetMeshs.push_back(HeartMesh);
			TargetSpawner->Get<TargetController>()->TargetMaterials.push_back(HeartMaterial);

			TargetSpawner->Get<TargetController>()->TargetNames.push_back("Kidney");
			TargetSpawner->Get<TargetController>()->TargetPositions.push_back(glm::vec3(50.0f, 50.0f, -50.0));
			TargetSpawner->Get<TargetController>()->TargetMeshs.push_back(KidneyMesh);
			TargetSpawner->Get<TargetController>()->TargetMaterials.push_back(KidneyMaterial);
		}

		////////////////////////Enemies/////////////////////////////// 
		GameObject::Sptr Enemies = scene->CreateGameObject("Enemies");

		GameObject::Sptr TopEnemySpawner = scene->CreateGameObject("Enemy Spawner Top");
		{
			TopEnemySpawner->SetPostion(glm::vec3(0.0f, 0.0f, 100.0f));
			TopEnemySpawner->Add<EnemySpawnerBehaviour>();

			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyMaterial = LargeEnemyMaterial;
			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyMesh = LargeEnemyMesh;
			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyFrames = LargeEnemyFrames;

			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyMaterial = NormalEnemyMaterial;
			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyMesh = NormalEnemyMesh;
			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyFrames = NormalEnemyFrames;

			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyMaterial = FastEnemyMaterial;
			TopEnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyMesh = FastEnemyMesh;
			//EnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyFrames = FastEnemyFrames;

			scene->EnemySpawnerObjects.push_back(TopEnemySpawner);
		}
		GameObject::Sptr BottomEnemySpawner = scene->CreateGameObject("Enemy Spawner Bottom");
		{
			BottomEnemySpawner->SetPostion(glm::vec3(0.0f, 0.0f, -100.0f));
			BottomEnemySpawner->Add<EnemySpawnerBehaviour>();

			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyMaterial = LargeEnemyMaterial;
			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyMesh = LargeEnemyMesh;
			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->LargeEnemyFrames = LargeEnemyFrames;

			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyMaterial = NormalEnemyMaterial;
			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyMesh = NormalEnemyMesh;
			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->NormalEnemyFrames = NormalEnemyFrames;

			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyMaterial = FastEnemyMaterial;
			BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyMesh = FastEnemyMesh;
			//BottomEnemySpawner->Get<EnemySpawnerBehaviour>()->FastEnemyFrames = FastEnemyFrames;

			scene->EnemySpawnerObjects.push_back(BottomEnemySpawner);
		}
		
		//OBJECTS BELOW HAVE A SPAWN RANGE OF - (X,Y,Z) TO + (X,Y,Z)
		//////////////// Background Objects ///// 50 max range

		GameObject::Sptr BackgroundObjects = scene->CreateGameObject("BackgroundObjects");

		GameObject::Sptr APC = scene->CreateGameObject("APC");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			APC->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = APC->Add<RenderComponent>();
			renderer->SetMesh(APCMesh);
			renderer->SetMaterial(APCMaterial);

			APC->Add<BackgroundObjectsBehaviour>();
			APC->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(APC);
		}
		GameObject::Sptr APC2 = scene->CreateGameObject("APC2");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			APC2->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = APC2->Add<RenderComponent>();
			renderer->SetMesh(APC2Mesh);
			renderer->SetMaterial(APC2Material);

			APC2->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(APC2);
		}
		GameObject::Sptr Bronchi = scene->CreateGameObject("Bronchi");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Bronchi->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Bronchi->Add<RenderComponent>();
			renderer->SetMesh(BronchiMesh);
			renderer->SetMaterial(BronchiMaterial);

			Bronchi->Add<BackgroundObjectsBehaviour>();
			Bronchi->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Bronchi);
		}
		GameObject::Sptr Cell = scene->CreateGameObject("Cell");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Cell->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Cell->Add<RenderComponent>();
			renderer->SetMesh(CellMesh);
			renderer->SetMaterial(CellMaterial);

			Cell->Add<BackgroundObjectsBehaviour>();
			Cell->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Cell);
		}
		GameObject::Sptr Cell2 = scene->CreateGameObject("Cell2");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Cell2->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Cell2->Add<RenderComponent>();
			renderer->SetMesh(Cell2Mesh);
			renderer->SetMaterial(Cell2Material);

			Cell2->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(Cell2);
		}
		GameObject::Sptr Co2 = scene->CreateGameObject("Co2");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Co2->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Co2->Add<RenderComponent>();
			renderer->SetMesh(Co2Mesh);
			renderer->SetMaterial(Co2Material);

			Co2->Add<BackgroundObjectsBehaviour>();
			Co2->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Co2);
		}
		GameObject::Sptr Mca = scene->CreateGameObject("Mca");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Mca->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Mca->Add<RenderComponent>();
			renderer->SetMesh(McaMesh);
			renderer->SetMaterial(McaMaterial);

			Mca->Add<BackgroundObjectsBehaviour>();
			Mca->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Mca);
		}
		GameObject::Sptr Microbiota = scene->CreateGameObject("Microbiota");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Microbiota->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Microbiota->Add<RenderComponent>();
			renderer->SetMesh(MicrobiotaMesh);
			renderer->SetMaterial(MicrobiotaMaterial);

			Microbiota->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(Microbiota);
		}
		GameObject::Sptr NewGerm = scene->CreateGameObject("NewGerm");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			NewGerm->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = NewGerm->Add<RenderComponent>();
			renderer->SetMesh(NewGermMesh);
			renderer->SetMaterial(NewGermMaterial);

			NewGerm->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(NewGerm);
		}
		GameObject::Sptr Oxygen = scene->CreateGameObject("Oxygen");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Oxygen->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Oxygen->Add<RenderComponent>();
			renderer->SetMesh(OxygenMesh);
			renderer->SetMaterial(OxygenMaterial);

			Oxygen->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(Oxygen);
		}
		GameObject::Sptr TopPipeEntrance = scene->CreateGameObject("Top Pipe Entrance");
		{

			TopPipeEntrance->SetPostion(glm::vec3(0.0f, 0.0f, 100.0f));
			TopPipeEntrance->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			TopPipeEntrance->SetScale(glm::vec3(5.0f));


			// Add a render component
			RenderComponent::Sptr renderer = TopPipeEntrance->Add<RenderComponent>();
			renderer->SetMesh(PipeMesh);
			renderer->SetMaterial(PipeMaterial);

			BackgroundObjects->AddChild(TopPipeEntrance);
		}
		GameObject::Sptr BottomPipeEntrance = scene->CreateGameObject("Bottom Pipe Entrance");
		{

			BottomPipeEntrance->SetPostion(glm::vec3(0.0f, 0.0f, -100.0f));
			BottomPipeEntrance->SetRotation(glm::vec3(90.0f, 180.0f, 0.0f));
			BottomPipeEntrance->SetScale(glm::vec3(5.0f));


			// Add a render component
			RenderComponent::Sptr renderer = BottomPipeEntrance->Add<RenderComponent>();
			renderer->SetMesh(PipeMesh);
			renderer->SetMaterial(PipeMaterial);

			BackgroundObjects->AddChild(BottomPipeEntrance);
		}
		GameObject::Sptr Smokeplaque = scene->CreateGameObject("Smokeplaque");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Smokeplaque->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Smokeplaque->Add<RenderComponent>();
			renderer->SetMesh(SmokeplaqueMesh);
			renderer->SetMaterial(SmokeplaqueMaterial);

			Smokeplaque->Add<BackgroundObjectsBehaviour>();
			Smokeplaque->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Smokeplaque);
		}
		GameObject::Sptr Symbiont = scene->CreateGameObject("Symbiont");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Symbiont->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = Symbiont->Add<RenderComponent>();
			renderer->SetMesh(SymbiontMesh);
			renderer->SetMaterial(SymbiontMaterial);

			Symbiont->Add<BackgroundObjectsBehaviour>();
			Symbiont->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(Symbiont);
		}
		GameObject::Sptr Symbiont2 = scene->CreateGameObject("Symbiont2");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			Symbiont2->SetPostion(glm::vec3(x, y, z));

			// Add a render component
			RenderComponent::Sptr renderer = Symbiont2->Add<RenderComponent>();
			renderer->SetMesh(Symbiont2Mesh);
			renderer->SetMaterial(Symbiont2Material);

			Symbiont2->Add<BackgroundObjectsBehaviour>();

			/*MorphAnimator::Sptr animation2 = Symbiont2->Add<MorphAnimator>();

			animation2->AddClip(Symbiont2Frames, 0.7f, "Idle");

			animation2->ActivateAnim("Idle"); */

			BackgroundObjects->AddChild(Symbiont2);
		}
		GameObject::Sptr Vein = scene->CreateGameObject("Vein");
		{

			Vein->SetPostion(glm::vec3(75.0f, 75.0f, 75.0f));
			Vein->SetRotation(glm::vec3(130.0f, 40.0f, 0.0f));


			// Add a render component
			RenderComponent::Sptr renderer = Vein->Add<RenderComponent>();
			renderer->SetMesh(VeinMesh);
			renderer->SetMaterial(FloorVeinANDVeinMaterial);

			BackgroundObjects->AddChild(Vein);
		}
		GameObject::Sptr VeinY = scene->CreateGameObject("VeinY");
		{

			VeinY->SetPostion(glm::vec3(-80.0f, -90.0f, -100.0f));
			VeinY->SetRotation(glm::vec3(75.0f, 63.0f, 18.0f));


			// Add a render component
			RenderComponent::Sptr renderer = VeinY->Add<RenderComponent>();
			renderer->SetMesh(VeinYMesh);
			renderer->SetMaterial(FloorVeinANDVeinMaterial);

			BackgroundObjects->AddChild(VeinY);
		}
		GameObject::Sptr VeinStick = scene->CreateGameObject("VeinStick");
		{

			VeinStick->SetPostion(glm::vec3(0.0f, 20.0f, 100.0f));
			VeinStick->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));


			// Add a render component
			RenderComponent::Sptr renderer = VeinStick->Add<RenderComponent>();
			renderer->SetMesh(VeinStickMesh);
			renderer->SetMaterial(FloorVeinANDVeinMaterial);

			BackgroundObjects->AddChild(VeinStick);
		}
		GameObject::Sptr WhiteBloodCell = scene->CreateGameObject("WhiteBloodCell");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			WhiteBloodCell->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = WhiteBloodCell->Add<RenderComponent>();
			renderer->SetMesh(WhiteBloodCellMesh);
			renderer->SetMaterial(WhiteBloodCellMaterial);

			WhiteBloodCell->Add<BackgroundObjectsBehaviour>();
			WhiteBloodCell->Get<BackgroundObjectsBehaviour>()->BezierMode = true;
			BackgroundObjects->AddChild(WhiteBloodCell);
		}
		GameObject::Sptr WhiteBloodCell2 = scene->CreateGameObject("WhiteBloodCell2");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			WhiteBloodCell2->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = WhiteBloodCell2->Add<RenderComponent>();
			renderer->SetMesh(WhiteBloodCell2Mesh);
			renderer->SetMaterial(WhiteBloodCell2Material);

			WhiteBloodCell2->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(WhiteBloodCell2);
		}
		GameObject::Sptr YellowMicrobiota = scene->CreateGameObject("YellowMicrobiota");
		{
			float x = (float)(rand() % 100 + (-50));
			float y = (float)(rand() % 100 + (-50));
			float z = (float)(rand() % 100 + (-50));
			YellowMicrobiota->SetPostion(glm::vec3(x, y, z));


			// Add a render component
			RenderComponent::Sptr renderer = YellowMicrobiota->Add<RenderComponent>();
			renderer->SetMesh(YellowMicrobiotaMesh);
			renderer->SetMaterial(YellowMicrobiotaMaterial);

			YellowMicrobiota->Add<BackgroundObjectsBehaviour>();
			BackgroundObjects->AddChild(YellowMicrobiota);
		}


		/////////////////////////// UI //////////////////////////////
		GameObject::Sptr UI = scene->CreateGameObject("UI");
		{
			UI->Add<UiController>();

			UI->Get<UiController>()->GameFont = ResourceManager::CreateAsset<Font>("fonts/Font.otf", 25.0f);
			UI->Get<UiController>()->GameFont->Bake();

			UI->Get<UiController>()->GameTitleTexture = TitleTexture;
			UI->Get<UiController>()->GameTutorialTexture = Tutorial1Texture;
			UI->Get<UiController>()->GameTutorialNextTexture = Tutorial2Texture;
			UI->Get<UiController>()->GamePauseTexture = GamePauseTexture;
			UI->Get<UiController>()->GamePauseTutorialTexture = Tutorial1PauseTexture;
			UI->Get<UiController>()->GamePauseTutorialNextTexture = Tutorial2PauseTexture;
			UI->Get<UiController>()->GameOverTexture = GameOverTexture;
			UI->Get<UiController>()->GameWinTexture = GameWinTexture;
			UI->Get<UiController>()->FullHp = Health100Texture;
			UI->Get<UiController>()->NintyPercentHp = Health90Texture;
			UI->Get<UiController>()->EightyPercentHp = Health80Texture;
			UI->Get<UiController>()->SeventyPercentHp = Health70Texture;
			UI->Get<UiController>()->SixtyPercentHp = Health60Texture;
			UI->Get<UiController>()->HalfHp = Health50Texture;
			UI->Get<UiController>()->FortyPercentHp = Health40Texture;
			UI->Get<UiController>()->ThirtyPercentHp = Health30Texture;
			UI->Get<UiController>()->TwentyPercentHp = Health20Texture;
			UI->Get<UiController>()->TenPercentHp = Health10Texture;
			UI->Get<UiController>()->NoHp = Health0Texture;

		}


		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("ui assets/menu screen/Title.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
