#pragma once
#include "Game/GameCommon.h"
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.h"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Vertex_PCU.h"
// -----------------------------------------------------------------------------
class Player;
class Prop;
//------------------------------------------------------------------------------
typedef std::vector<Entity*> EntityList;
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static const int MAX_PROPS = 10;
// -----------------------------------------------------------------------------
class Game
{
public:
	App* m_app;
	Game(App* owner);
	~Game();
	void StartUp();

	void Update();
	void UpdateCameras();
	void UpdateEntities(float deltaSeconds);

	void Render() const;
	void RenderAttractMode() const;
	void RenderEntities() const;
	void RenderGrid() const;

	void Shutdown();

	void InitializeGrid();
	void KeyInputPresses();
	void AdjustForPauseAndTimeDistortion(float deltaSeconds);
	bool		m_isAttractMode = true;

private:
	Camera		m_screenCamera;
	Camera      m_gameWorldCamera;
	Clock		m_gameClock;

	Player* m_player = nullptr;
	Prop* m_cube = nullptr;
	Prop* m_identicalCube = nullptr;
	Prop* m_sphere = nullptr;

	EntityList m_allEntities;
	float m_colorBrightness = 0.f;
	std::vector<Vertex_PCU> m_gridVerts;
};