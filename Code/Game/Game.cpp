#include "Game/Game.h"
#include "Game/GameCommon.h"
#include "Game/App.h"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"

#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Window/Window.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Rgba8.h"
#include "Engine/Core/Vertex_PCU.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.h"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/AABB3.hpp"

Game::Game(App* owner)
	: m_app(owner)
{
}

Game::~Game()
{
}

void Game::StartUp()
{
	// Write control interface into devconsole
	g_theDevConsole->AddLine(Rgba8::CYAN, "Welcome to Protogame3D!");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "----------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8::CYAN, "CONTROLS:");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "ESC   - Quits the game");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "SPACE - Start game");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "SHIFT - Increase speed by factor of 10.");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "H     - Reset position and orientation back to 0.");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "Q/E   - Roll negative/positive");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "A/D   - Move left/right");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "W/S   - Move forward/backward");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "Z/C   - Move down/up");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "----------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8::CYAN, "DEBUG CONTROLS:");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "1   - Spawns an xray line");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "2   - Spawns a point at xy plane");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "3   - Spawns a wire sphere");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "4   - Spawns a world basis");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "5   - Spawns full opposing billboard text");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "6   - Spawns a wire cylinder");
	g_theDevConsole->AddLine(Rgba8::LIGHTYELLOW, "7   - Spanws a orientation message");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "----------------------------------------------------------------------");

	// Create and push back the entities
	m_player = new Player(this, Vec3(-1.f, 0.f, 0.5f));
	m_cube = new Prop(this, Vec3(2.f, 2.f, 0.f));
	m_identicalCube = new Prop(this, Vec3(-2.f, -2.f, 0.f));
	m_sphere = new Prop(this, Vec3(10.f, -5.f, 1.f));

	m_allEntities.push_back(m_cube);
	m_allEntities.push_back(m_identicalCube);

	// Create basis with debug arrows, giving them infinite duration
	float arrowRadius = 0.15f;
	DebugAddWorldArrow(Vec3::ZERO, Vec3::XAXE, arrowRadius, -1.f, Rgba8::RED);
	DebugAddWorldArrow(Vec3::ZERO, Vec3::YAXE, arrowRadius, -1.f, Rgba8::GREEN);
	DebugAddWorldArrow(Vec3::ZERO, Vec3::ZAXE, arrowRadius, -1.f, Rgba8::BLUE);

    // Set text for X axe
	Mat44 textMatrixX;
	textMatrixX.SetTranslation3D(Vec3(0.7f, 0.f, 0.20f));
	textMatrixX.Append(EulerAngles(0.f, 0.f, 90.f).GetAsMatrix_IFwd_JLeft_KUp());
	DebugAddWorldText("x - forward", textMatrixX, 0.1f, Vec2::ONEHALF, -1.f, Rgba8::RED);

	// Set text for Y axe
	Mat44 textMatrixY;
	textMatrixY.SetTranslation3D(Vec3(0.0f, 0.6f, 0.20f));
	textMatrixY.Append(EulerAngles(-90.f, 0.f, 90.f).GetAsMatrix_IFwd_JLeft_KUp());
	DebugAddWorldText("y - left", textMatrixY, 0.1f, Vec2::ONEHALF, -1.f, Rgba8::GREEN);

	// Set text for Z axe
	Mat44 textMatrixZ;
	textMatrixZ.SetTranslation3D(Vec3(0.f, -0.25f, 0.50f));
	textMatrixZ.Append(EulerAngles(-90.f, -90.f, 90.f).GetAsMatrix_IFwd_JLeft_KUp());
	DebugAddWorldText("z - up", textMatrixZ, 0.1f, Vec2::ONEHALF, -1.f, Rgba8::BLUE);

	// Adding a plus crosshair with infinite duration
	DebugAddScreenText("+", AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y), 20.f, Vec2::ONEHALF, -1.f);

	// Initialize the grid
	InitializeGrid();
}

void Game::Update()
{
	// Setting clock time variables
	double deltaSeconds = m_gameClock.GetDeltaSeconds();
	double totalTime    = Clock::GetSystemClock().GetTotalSeconds();
	double frameRate    = Clock::GetSystemClock().GetFrameRate();
	double scale        = Clock::GetSystemClock().GetTimeScale();

	m_colorBrightness += 30.f * static_cast<float>(deltaSeconds);

	// Brightness change over few seconds
	float sinColor = fabsf(SinDegrees(m_colorBrightness));
	unsigned char colorValue = static_cast<unsigned char>(GetClamped(sinColor, 0.f, 1.f) * 255);
	m_identicalCube->m_color = Rgba8(colorValue, colorValue, colorValue, 255);

	// Rotate about the x and y axis by 30 degrees
	m_cube->m_orientation.m_pitchDegrees += 30.f * static_cast<float>(deltaSeconds);
	m_cube->m_orientation.m_rollDegrees += 30.f * static_cast<float>(deltaSeconds);

	// Rotate sphere about z
	m_sphere->m_orientation.m_yawDegrees += 45.f * static_cast<float>(deltaSeconds);

	// Set text for position, time, FPS, and scale
	std::string positionText = Stringf("Player position: %0.2f %0.2f %0.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z);
	std::string timeScaleText = Stringf("Time: %0.2fs FPS: %0.2f Scale: %0.2f", totalTime, frameRate, scale);
	DebugAddScreenText(positionText, AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y), 10.f, Vec2(0.f, 0.97f), 0.f);
	DebugAddScreenText(timeScaleText, AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y), 15.f, Vec2(0.98f, 0.97f), 0.f);

	m_player->Update(static_cast<float>(deltaSeconds));

	AdjustForPauseAndTimeDistortion(static_cast<float>(deltaSeconds));
	KeyInputPresses();

	UpdateCameras();
}

void Game::Render() const
{
	if (m_isAttractMode == true)
	{
		g_theRenderer->BeginCamera(m_screenCamera);
		RenderAttractMode();
		g_theRenderer->EndCamera(m_screenCamera);
	}
	if (m_isAttractMode == false)
	{
		g_theRenderer->BeginCamera(m_player->GetPlayerCamera());
		g_theRenderer->ClearScreen(Rgba8(70, 70, 70, 255));
		RenderEntities();
		m_sphere->RenderSphere();
		RenderGrid();
		g_theRenderer->EndCamera(m_player->GetPlayerCamera());

		DebugRenderWorld(m_player->GetPlayerCamera());
		DebugRenderScreen(m_screenCamera);
	}
}

void Game::Shutdown()
{
	for (size_t entityIndex = 0; entityIndex < m_allEntities.size(); ++entityIndex)
	{
		delete m_allEntities[entityIndex];
	}
	m_allEntities.clear();
}

void Game::InitializeGrid()
{
	// Layout
	for (int gridIndex = 0; gridIndex < 100; ++gridIndex)
	{
		AddVertsForAABB3D(m_gridVerts, AABB3(-50.f, -50.01f + gridIndex, -0.005f, 50.f, -49.99f + gridIndex, 0.005f), Rgba8::DARKGRAY);
		AddVertsForAABB3D(m_gridVerts, AABB3(-50.01f + gridIndex, -50.0f, -0.005f, -49.99f + gridIndex, 50.f, 0.005f), Rgba8::DARKGRAY);
	}

	// Y axis
	for (int x = 0; x < 105; x += 5)
	{
		if (x == 50)
		{
			AddVertsForAABB3D(m_gridVerts, AABB3(-50.05f + x, -50.f, -0.05f, -49.95f + x, 50.f, 0.05f), Rgba8::GREEN);
		}
		else
		{
			AddVertsForAABB3D(m_gridVerts, AABB3(-50.05f + x, -50.f, -0.05f, -49.95f + x, 50.f, 0.05f), Rgba8::SEAWEED);
		}
	}

	// X axis
	for (int y = 0; y < 105; y += 5)
	{
		if (y == 50)
		{
			AddVertsForAABB3D(m_gridVerts, AABB3(-50.f, -50.05f + y, -0.05f, 50.f, -49.95f + y, 0.05f), Rgba8::RED);
		}
		else
		{
			AddVertsForAABB3D(m_gridVerts, AABB3(-50.f, -50.05f + y, -0.05f, 50.f, -49.95f + y, 0.05f), Rgba8::DARKRED);
		}
	}
}

void Game::KeyInputPresses()
{
	// Attract Mode
	if (g_theInput->WasKeyJustPressed(' '))
	{
		m_isAttractMode = false;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC))
	{
		m_isAttractMode = true;
	}}

void Game::AdjustForPauseAndTimeDistortion(float deltaSeconds) {

	UNUSED(deltaSeconds);

	if (g_theInput->IsKeyDown('T'))
	{
		m_gameClock.SetTimeScale(0.1);
	}
	else
	{
		m_gameClock.SetTimeScale(1.0);
	}

	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_gameClock.TogglePause();
	}

	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_gameClock.StepSingleFrame();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) && m_isAttractMode)
	{
		g_theEventSystem->FireEvent("Quit");
	}
}

void Game::UpdateCameras()
{
	m_screenCamera.SetOrthoView(Vec2::ZERO, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
}

void Game::UpdateEntities(float deltaSeconds)
{
	for (size_t entityIndex = 0; entityIndex < m_allEntities.size(); ++entityIndex)
	{
		if (m_allEntities[entityIndex] != nullptr)
		{
			m_allEntities[entityIndex]->Update(deltaSeconds);
		}
	}
}

void Game::RenderAttractMode() const
{
}

void Game::RenderEntities() const
{
	for (size_t entityIndex = 0; entityIndex < m_allEntities.size(); ++entityIndex)
	{
		if (m_allEntities[entityIndex] != nullptr)
		{
			g_theRenderer->SetModelConstants(m_allEntities[entityIndex]->GetModelToWorldTransform(), m_allEntities[entityIndex]->m_color);
			m_allEntities[entityIndex]->Render();
		}
	}
}

void Game::RenderGrid() const
{
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(m_gridVerts);
}
