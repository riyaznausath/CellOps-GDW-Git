#include "BackgroundObjectsBehaviour.h"

template<typename T>
T Catmull(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    return 0.5f * (2.0f * p1 + t * (-p0 + p2)
        + t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)
        + t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
}

// TODO: Templated Bezier function
template<typename T>
T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    return ((p0)+t * (3.0f * -p0 + 3.0f * p1)
        + t * t * (3.0f * p0 - 6.0f * p1 + 3.0f * p2)
        + t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
}

BackgroundObjectsBehaviour::~BackgroundObjectsBehaviour() = default;

BackgroundObjectsBehaviour::BackgroundObjectsBehaviour() :
    IComponent(),
    BezierMode(false)
{}

void BackgroundObjectsBehaviour::Awake()
{

    RoutePoint1 = GetPosition();
    RoutePoint2 = GetPosition();
    RoutePoint3 = GetPosition();
    RoutePoint4 = GetPosition();
}

void BackgroundObjectsBehaviour::Update(float deltaTime)
{
    SegmentTimer += deltaTime;
    if (SegmentTimer >= SegmentTimerMax) {
        SegmentTimer = 0;
    }
    float t;
    t = SegmentTimer / SegmentTimerMax;

    if (BezierMode)
    GetGameObject()->SetPostion(Bezier(RoutePoint1, RoutePoint2, RoutePoint3, RoutePoint4, t));
    else 
        GetGameObject()->SetPostion(Catmull(RoutePoint1, RoutePoint2, RoutePoint3, RoutePoint4, t));
}

void BackgroundObjectsBehaviour::RenderImGui() {
   
}
/// <summary>
/// Sets Random points for Routes
/// </summary>
/// <returns>xyz points in floats</returns>
glm::vec3 BackgroundObjectsBehaviour::GetPosition()
{
    float x = (float)(rand() % 100 + (-50));
    float y = (float)(rand() % 100 + (-50));
    float z = (float)(rand() % 100 + (-50));
    return glm::vec3(x,y,z);
}
nlohmann::json BackgroundObjectsBehaviour::ToJson() const {
    return {
        {"BezierMode",BezierMode}
    };
}
BackgroundObjectsBehaviour::Sptr BackgroundObjectsBehaviour::FromJson(const nlohmann::json& blob) {
    BackgroundObjectsBehaviour::Sptr result = std::make_shared<BackgroundObjectsBehaviour>();
    result->BezierMode = blob["BezierMode"];
    return result;
}
