#include "UIController.h"

UiController::UiController() :
	IComponent(),
	GameFont(nullptr),
	GameTitleTexture(nullptr),
	GameTutorialTexture(nullptr),
	GameTutorialNextTexture(nullptr),
	GamePauseTexture(nullptr),
	GamePauseTutorialTexture(nullptr),
	GamePauseTutorialNextTexture(nullptr),
	GameOverTexture(nullptr),
	GameWinTexture(nullptr),
	FullHp(nullptr),
	NintyPercentHp(nullptr),
	EightyPercentHp(nullptr),
	SeventyPercentHp(nullptr),
	SixtyPercentHp(nullptr),
	HalfHp(nullptr),
	FortyPercentHp(nullptr),
	ThirtyPercentHp(nullptr),
	TwentyPercentHp(nullptr),
	TenPercentHp(nullptr),
	NoHp(nullptr)
{
}

UiController::~UiController() = default;

void UiController::UpdateUI()
{
	//Update Rounds
	std::string RoundText = "Round: ";
	RoundText += std::to_string(GetGameObject()->GetScene()->GameRound);
	GetGameObject()->GetScene()->FindObjectByName("Rounds")->Get<GuiText>()->SetText(RoundText);

	//Update Enemies Killed
	std::string EnemiesText = "Enemies Killed: ";
	EnemiesText += std::to_string(GetGameObject()->GetScene()->EnemiesKilled) + "/" + std::to_string(GetGameObject()->GetScene()->Enemies.size());
	GetGameObject()->GetScene()->FindObjectByName("EnemiesKilled")->Get<GuiText>()->SetText(EnemiesText);

	//Update Targets
	for (auto Target : GetGameObject()->GetScene()->Targets) {
		std::string TargetUIName = Target->Name+" UI";
		Gameplay::GameObject::Sptr TargetUI = GetGameObject()->GetScene()->FindObjectByName(TargetUIName);
		if (TargetUI == NULL)
			return;

		int TargetHealthPrecentage = Target->Get<TargetBehaviour>()->HealthInPercentage;

		if (TargetHealthPrecentage <= 0) {
			TargetUI->Get<GuiPanel>()->SetTexture(NoHp);
			GetGameObject()->GetScene()->RemoveGameObject(TargetUI);
		}
		else if (TargetHealthPrecentage < 10)
			TargetUI->Get<GuiPanel>()->SetTexture(TenPercentHp);
		else if (TargetHealthPrecentage < 20)
			TargetUI->Get<GuiPanel>()->SetTexture(TwentyPercentHp);
		else if (TargetHealthPrecentage < 30)
			TargetUI->Get<GuiPanel>()->SetTexture(ThirtyPercentHp);
		else if (TargetHealthPrecentage < 40)
			TargetUI->Get<GuiPanel>()->SetTexture(FortyPercentHp);
		else if (TargetHealthPrecentage < 50)
			TargetUI->Get<GuiPanel>()->SetTexture(HalfHp);
		else if (TargetHealthPrecentage < 60)
			TargetUI->Get<GuiPanel>()->SetTexture(SixtyPercentHp);
		else if (TargetHealthPrecentage < 70)
			TargetUI->Get<GuiPanel>()->SetTexture(SeventyPercentHp);
		else if (TargetHealthPrecentage < 80)
			TargetUI->Get<GuiPanel>()->SetTexture(EightyPercentHp);
		else if (TargetHealthPrecentage < 90)
			TargetUI->Get<GuiPanel>()->SetTexture(NintyPercentHp);
		else if (TargetHealthPrecentage < 100 || TargetHealthPrecentage == 100)
			TargetUI->Get<GuiPanel>()->SetTexture(FullHp);

		TargetUI->Get<GuiText>()->SetText(Target->Name + " " + std::to_string(TargetHealthPrecentage) + '%');
	}
}

void UiController::SetupGameScreen()
{
	GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Game Title"));
	GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Tutorial"));
	if (GetGameObject()->GetScene()->IsLoseScreenUp) {
		GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("GameOver"));
		GetGameObject()->GetScene()->IsLoseScreenUp = false;
	}
	if (GetGameObject()->GetScene()->IsWinScreenUp) {
		GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("GameWin"));
		GetGameObject()->GetScene()->IsWinScreenUp = false;
	}

	if (!GetGameObject()->GetScene()->FindObjectByName("EnemiesKilled"))
	_createUiObject("EnemiesKilled", "Enemied Killed :0/0", 10, 10, 60, 29, glm::vec4(1.0f));

	if (!GetGameObject()->GetScene()->FindObjectByName("Rounds"))
	_createUiObject("Rounds", "Round: 0", 10, 10, 750, 29, 750, 29, glm::vec4(1.0f));

	std::string RoundText = "Round: ";
	RoundText += std::to_string(GetGameObject()->GetScene()->GameRound);
	GetGameObject()->GetScene()->FindObjectByName("Rounds")->Get<GuiText>()->SetText(RoundText);
	
	int SetMinY=278;
	int SetMaxX = 192;
	int SetMaxY = 382;
	for (auto Target : GetGameObject()->GetScene()->Targets) {
		std::string TargetName=Target->Name;
		if (!GetGameObject()->GetScene()->FindObjectByName(TargetName + " UI"))
			_createUiObject(TargetName + " UI", TargetName + " Health 100 % ", 185, 102, 8, SetMinY, SetMaxX, SetMaxY, FullHp, glm::vec4(1.0f));

		SetMinY += 22;
		SetMaxX -= 2;
		SetMaxY += 18;
	}
}

void UiController::GameTitleScreen()
{
	Gameplay::GameObject::Sptr GameTitle = GetGameObject()->GetScene()->CreateGameObject("Game Title"); {
		audioEngine->playSoundByName("menuBackground");
		RectTransform::Sptr transform = GameTitle->Add<RectTransform>();
		transform->SetSize({ 800,800 });
		transform->SetMin({ 0,0 });
		transform->SetMax({ 800,800 });

		GuiPanel::Sptr GameMenuPanel = GameTitle->Add<GuiPanel>();
		GameMenuPanel->SetTexture(GameTitleTexture);

		GetGameObject()->AddChild(GameTitle);
	}
}

void UiController::GamePauseScreen()
{
	Gameplay::GameObject::Sptr GamePause = GetGameObject()->GetScene()->CreateGameObject("Game Pause"); {

		RectTransform::Sptr transform = GamePause->Add<RectTransform>();
		transform->SetSize({ 800,800 });
		transform->SetMin({ 0,0 });
		transform->SetMax({ 800,800 });

		GuiPanel::Sptr GamePausePanel = GamePause->Add<GuiPanel>();
		GamePausePanel->SetTexture(GamePauseTexture);

		GetGameObject()->AddChild(GamePause);
	}
}

void UiController::GameOverScreen()
{
	Gameplay::GameObject::Sptr GameOver = GetGameObject()->GetScene()->CreateGameObject("GameOver"); {

		RectTransform::Sptr transform = GameOver->Add<RectTransform>();
		transform->SetSize({ 800,800 });
		transform->SetMin({ 0,0 });
		transform->SetMax({ 800,800 });

		GuiPanel::Sptr GamePausePanel = GameOver->Add<GuiPanel>();
		GamePausePanel->SetTexture(GameOverTexture);

		GetGameObject()->AddChild(GameOver);
	}
}

void UiController::GameWinScreen()
{
	if (!GetGameObject()->GetScene()->FindObjectByName("GameWin")) {
		Gameplay::GameObject::Sptr GameWin = GetGameObject()->GetScene()->CreateGameObject("GameWin"); {

			RectTransform::Sptr transform = GameWin->Add<RectTransform>();
			transform->SetSize({ 800,800 });
			transform->SetMin({ 0,0 });
			transform->SetMax({ 800,800 });

			GuiPanel::Sptr GamePausePanel = GameWin->Add<GuiPanel>();
			GamePausePanel->SetTexture(GameWinTexture);

			GetGameObject()->AddChild(GameWin);
		}
	}
}

void UiController::GameTutorial(std::string GameStatus, int TutorialPageNumber)
{
	if (GameStatus == "Pause") {
		GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Game Pause"));
		if (TutorialPageNumber == 1) {
			Gameplay::GameObject::Sptr Tutorial = GetGameObject()->GetScene()->CreateGameObject("Tutorial"); {
				RectTransform::Sptr transform = Tutorial->Add<RectTransform>();
				transform->SetSize({ 800,800 });
				transform->SetMin({ 0,0 });
				transform->SetMax({ 800,800 });

				GuiPanel::Sptr TutorialPanel = Tutorial->Add<GuiPanel>();
				TutorialPanel->SetTexture(GamePauseTutorialTexture);

				GetGameObject()->AddChild(Tutorial);
			}
		}
		else if (TutorialPageNumber == 2) {
			//first remove previous page
			GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Tutorial"));
			//put up next page
			Gameplay::GameObject::Sptr Tutorial = GetGameObject()->GetScene()->CreateGameObject("Tutorial"); {
				RectTransform::Sptr transform = Tutorial->Add<RectTransform>();
				transform->SetSize({ 800,800 });
				transform->SetMin({ 0,0 });
				transform->SetMax({ 800,800 });

				GuiPanel::Sptr TutorialPanel = Tutorial->Add<GuiPanel>();
				TutorialPanel->SetTexture(GamePauseTutorialNextTexture);

				GetGameObject()->AddChild(Tutorial);
			}
		}
	}
	else if (GameStatus == "Start") {

		GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Game Title"));

		if (TutorialPageNumber == 1) {
			Gameplay::GameObject::Sptr Tutorial = GetGameObject()->GetScene()->CreateGameObject("Tutorial"); {
				RectTransform::Sptr transform = Tutorial->Add<RectTransform>();
				transform->SetSize({ 800,800 });
				transform->SetMin({ 0,0 });
				transform->SetMax({ 800,800 });

				GuiPanel::Sptr TutorialPanel = Tutorial->Add<GuiPanel>();
				TutorialPanel->SetTexture(GameTutorialTexture);

				GetGameObject()->AddChild(Tutorial);
			}
		}
		else if (TutorialPageNumber == 2) {
			//first remove previous page
			GetGameObject()->GetScene()->RemoveGameObject(GetGameObject()->GetScene()->FindObjectByName("Tutorial"));
			//put up next page
			Gameplay::GameObject::Sptr Tutorial = GetGameObject()->GetScene()->CreateGameObject("Tutorial"); {
				RectTransform::Sptr transform = Tutorial->Add<RectTransform>();
				transform->SetSize({ 800,800 });
				transform->SetMin({ 0,0 });
				transform->SetMax({ 800,800 });

				GuiPanel::Sptr TutorialPanel = Tutorial->Add<GuiPanel>();
				TutorialPanel->SetTexture(GameTutorialNextTexture);

				GetGameObject()->AddChild(Tutorial);
			}
		}
	}
} 

void UiController::_createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, glm::vec4 Color)
{
	Gameplay::GameObject::Sptr UIObject = GetGameObject()->GetScene()->CreateGameObject(NameOfObject);
	{
		RectTransform::Sptr transform = UIObject->Add<RectTransform>();
		transform->SetSize({ SetSizeMinX,SetSizeMinY });
		transform->SetMin({ SetMinX,SetMinY });

		GuiText::Sptr UiText = UIObject->Add<GuiText>();
		UiText->SetText(Text);
		UiText->SetFont(GameFont);
		UiText->SetColor(glm::vec4(1.0f));

		GetGameObject()->AddChild(UIObject);
	}
}

void UiController::_createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, int SetMaxX, int SetMaxY, glm::vec4 Color)
{
	Gameplay::GameObject::Sptr UIObject = GetGameObject()->GetScene()->CreateGameObject(NameOfObject);
	{
		RectTransform::Sptr transform = UIObject->Add<RectTransform>();
		transform->SetSize({ SetSizeMinX,SetSizeMinY });
		transform->SetMin({ SetMinX,SetMinY });
		transform->SetMax({ SetMaxX,SetMaxY });

		GuiText::Sptr UiText = UIObject->Add<GuiText>();
		UiText->SetText(Text);
		UiText->SetFont(GameFont);
		UiText->SetColor(glm::vec4(1.0f));

		GetGameObject()->AddChild(UIObject);
	}
}

void UiController::_createUiObject(std::string NameOfObject, std::string Text, int SetSizeMinX, int SetSizeMinY, int SetMinX, int SetMinY, int SetMaxX, int SetMaxY, Texture2D::Sptr Texture, glm::vec4 Color)
{
	Gameplay::GameObject::Sptr UIObject = GetGameObject()->GetScene()->CreateGameObject(NameOfObject);
	{
		RectTransform::Sptr transform = UIObject->Add<RectTransform>();
		transform->SetSize({ SetSizeMinX,SetSizeMinY });
		transform->SetMin({ SetMinX,SetMinY });
		transform->SetMax({ SetMaxX,SetMaxY });

		GuiPanel::Sptr Health = UIObject->Add<GuiPanel>();
		Health->SetTexture(Texture);

		GuiText::Sptr UiText = UIObject->Add<GuiText>();
		UiText->SetText(Text);
		UiText->SetFont(GameFont);
		UiText->SetColor(glm::vec4(1.0f));

		GetGameObject()->AddChild(UIObject);
	}
}

nlohmann::json UiController::ToJson() const
{
	return {
		{"Font",GameFont->GetGUID().str()},
		{"GameTitleTexture", GameTitleTexture->GetGUID().str()},
		{"GameTutorialTexture", GameTutorialTexture->GetGUID().str()},
		{"GameTutorialNextTexture", GameTutorialNextTexture->GetGUID().str()},
		{"GamePauseTexture",GamePauseTexture->GetGUID().str()},
		{"GamePauseTutorialTexture",GamePauseTutorialTexture->GetGUID().str()},
		{"GamePauseTutorialNextTexture",GamePauseTutorialNextTexture->GetGUID().str()},
		{"GameOverTexture",GameOverTexture->GetGUID().str()},
		{"GameWinTexture",GameWinTexture->GetGUID().str()},
		{"FullHp",FullHp->GetGUID().str()},
		{"NintyPercentHp",NintyPercentHp->GetGUID().str()},
		{"EightyPercentHp",EightyPercentHp->GetGUID().str()},
		{"SeventyPercentHp",SeventyPercentHp->GetGUID().str()},
		{"SixtyPercentHp",SixtyPercentHp->GetGUID().str()},
		{"HalfHp",HalfHp->GetGUID().str()},
		{"FortyPercentHp",FortyPercentHp->GetGUID().str()},
		{"ThirtyPercentHp",ThirtyPercentHp->GetGUID().str()},
		{"TwentyPercentHp",TwentyPercentHp->GetGUID().str()},
		{"TenPercentHp",TenPercentHp->GetGUID().str()},
		{"NoHp",NoHp->GetGUID().str()}
	};
}

UiController::Sptr UiController::FromJson(const nlohmann::json& blob)
{
	UiController::Sptr result = std::make_shared<UiController>();
	result->GameFont = ResourceManager::Get<Font>(Guid(blob["Font"]));
	result->GameTutorialTexture = ResourceManager::Get<Texture2D>(Guid(blob["GameTutorialTexture"]));
	result->GameTutorialNextTexture = ResourceManager::Get<Texture2D>(Guid(blob["GameTutorialNextTexture"]));
	result->GameTitleTexture = ResourceManager::Get<Texture2D>(Guid(blob["GameTitleTexture"]));
	result->GamePauseTexture = ResourceManager::Get<Texture2D>(Guid(blob["GamePauseTexture"]));
	result->GamePauseTutorialTexture = ResourceManager::Get<Texture2D>(Guid(blob["GamePauseTutorialTexture"]));
	result->GamePauseTutorialNextTexture = ResourceManager::Get<Texture2D>(Guid(blob["GamePauseTutorialNextTexture"]));
	result->GameOverTexture = ResourceManager::Get<Texture2D>(Guid(blob["GameOverTexture"]));
	result->GameWinTexture = ResourceManager::Get<Texture2D>(Guid(blob["GameWinTexture"]));
	result->FullHp=ResourceManager::Get<Texture2D>(Guid(blob["FullHp"]));
	result->NintyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["NintyPercentHp"]));
	result->EightyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["EightyPercentHp"]));
	result->SeventyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["SeventyPercentHp"]));
	result->SixtyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["SixtyPercentHp"]));
	result->HalfHp=ResourceManager::Get<Texture2D>(Guid(blob["HalfHp"]));
	result->FortyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["FortyPercentHp"]));
	result->ThirtyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["ThirtyPercentHp"]));
	result->TwentyPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["TwentyPercentHp"]));
	result->TenPercentHp=ResourceManager::Get<Texture2D>(Guid(blob["TenPercentHp"]));
	result->NoHp=ResourceManager::Get<Texture2D>(Guid(blob["NoHp"]));
	return result;
}

void UiController::RenderImGui()
{

}