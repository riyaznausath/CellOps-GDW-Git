#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Components/RenderComponent.h"
class BackgroundObjectsBehaviour :public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<BackgroundObjectsBehaviour> Sptr;

	BackgroundObjectsBehaviour();
	virtual ~BackgroundObjectsBehaviour();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	MAKE_TYPENAME(BackgroundObjectsBehaviour);
	virtual nlohmann::json ToJson() const override;
	static BackgroundObjectsBehaviour::Sptr FromJson(const nlohmann::json& blob);
	virtual void RenderImGui() override;
	glm::vec3 GetPosition();


	glm::vec3 RoutePoint1;
	glm::vec3 RoutePoint2;
	glm::vec3 RoutePoint3;
	glm::vec3 RoutePoint4;

	float SegmentTimer = 0;
	float SegmentTimerMax = 10.0f;

	bool BezierMode;
};