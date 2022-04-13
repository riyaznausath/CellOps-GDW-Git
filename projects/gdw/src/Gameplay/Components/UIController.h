#pragma once
#include "IComponent.h"
#include "Gameplay/Scene.h"
#include <Utils/ImGuiHelper.h>
#include <Graphics/Font.h>
#include <Gameplay/Components/GUI/GuiText.h>
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/TargetBehaviour.h"
#include "Utils/AudioEngine.h"

/// <summary>
/// This class will be responsible for all Ui stuff
/// </summary>
class UiController :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<UiController> Sptr;

	UiController();
	virtual ~UiController();

	MAKE_TYPENAME(UiController);
	virtual nlohmann::json ToJson() const override;
	static UiController::Sptr FromJson(const nlohmann::json& blob);
	virtual void RenderImGui() override;

	//Audio 
	AudioEngine* audioEngine = AudioEngine::instance();

	Font::Sptr GameFont;

	Texture2D::Sptr GameTitleTexture;
	Texture2D::Sptr GameTutorialTexture;
	Texture2D::Sptr GameTutorialNextTexture;
	Texture2D::Sptr GamePauseTexture;
	Texture2D::Sptr GamePauseTutorialTexture;
	Texture2D::Sptr GamePauseTutorialNextTexture;
	Texture2D::Sptr GameOverTexture;
	Texture2D::Sptr GameWinTexture;
	Texture2D::Sptr FullHp;
	Texture2D::Sptr NintyPercentHp;
	Texture2D::Sptr EightyPercentHp;
	Texture2D::Sptr SeventyPercentHp;
	Texture2D::Sptr SixtyPercentHp;
	Texture2D::Sptr HalfHp;
	Texture2D::Sptr FortyPercentHp;
	Texture2D::Sptr ThirtyPercentHp;
	Texture2D::Sptr TwentyPercentHp;
	Texture2D::Sptr TenPercentHp;
	Texture2D::Sptr NoHp;

	void UpdateUI();

	/// <summary>
	/// Set Up Main Game UI
	/// </summary>
	void SetupGameScreen();

	/// <summary>
	/// Brings Up Title Screen
	/// </summary>
	void GameTitleScreen();

	/// <summary>
	/// Brings Up Pause Screen
	/// </summary>
	void GamePauseScreen();

	/// <summary>
	/// Brings Up GameOver Screen
	/// </summary>
	void GameOverScreen();

	/// <summary>
	/// Brings Up GameWin Screen
	/// </summary>
	void GameWinScreen();

	/// <summary>
	/// Deals with displaying game tutorial
	/// </summary>
	/// <param name="GameStatus">Either at Game "Start" or Game "Pause"</param>
	/// <param name="TutorialPageNumber">1st page or 2nd page</param>
	void GameTutorial(std::string GameStatus,int TutorialPageNumber);
private:
	/// <summary>
	/// Create Ui Object
	/// </summary>
	/// <param name="NameOfObject">Name of Object to find Later</param>
	/// <param name="Text">Test you want on the UI</param>
	/// <param name="SetSizeMinX">Size X value</param>
	/// <param name="SetSizeMinY">Size Y value</param>
	/// <param name="SetMinX">Min position X</param>
	/// <param name="SetMinY">Min position Y</param>
	/// <param name="Color">Color must be in glm vec4</param>
	void _createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, glm::vec4 Color);

	/// <summary>
	/// Create Ui Object
	/// </summary>
	/// <param name="NameOfObject">Name of Object to find Later</param>
	/// <param name="Text">Test you want on the UI</param>
	/// <param name="SetSizeMinX">Size X value</param>
	/// <param name="SetSizeMinY">Size Y value</param>
	/// <param name="SetMinX">Min position X</param>
	/// <param name="SetMinY">Min position Y</param>
	/// <param name="SetMaxX">Max position X</param>
	/// <param name="SetMaxY">Max position Y</param>
	/// <param name="Color">Color must be in glm vec4</param>
	void _createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, int SetMaxX, int SetMaxY, glm::vec4 Color);

	/// <summary>
	/// Create Ui Object
	/// </summary>
	/// <param name="NameOfObject">Name of Object to find Later</param>
	/// <param name="Text">Test you want on the UI</param>
	/// <param name="SetSizeMinX">Size X value</param>
	/// <param name="SetSizeMinY">Size Y value</param>
	/// <param name="SetMinX">Min position X</param>
	/// <param name="SetMinY">Min position Y</param>
	/// <param name="SetMaxX">Max position X</param>
	/// <param name="SetMaxY">Max position Y</param>
	/// <param name="Texture">Texture for the Ui</param>
	/// <param name="Color">Color must be in glm vec4</param>
	void _createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, int SetMaxX, int SetMaxY, Texture2D::Sptr Texture, glm::vec4 Color);
};
