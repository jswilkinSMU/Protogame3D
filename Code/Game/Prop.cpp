#include "Game/Prop.hpp"
#include "Game/GameCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/EngineCommon.h"

Prop::Prop(Game* owner, Vec3 const& position)
	:Entity(owner, position)
{
	m_position = position;
	m_orientation = EulerAngles(0.f, 0.f, 0.f);
	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
}

Prop::~Prop()
{
}

void Prop::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Prop::Render() const
{
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetModelConstants(GetModelToWorldTransform(), m_color);
	RenderCube();
}

void Prop::RenderCube() const
{
	std::vector<Vertex_PCU> cubeVerts;

	// +X
	AddVertsForQuad3D(cubeVerts, Vec3(0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Rgba8::RED);

	// -X
	AddVertsForQuad3D(cubeVerts, Vec3(-0.5f, 0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f), Rgba8::CYAN);

	// +Y
	AddVertsForQuad3D(cubeVerts, Vec3(0.5f, 0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f), Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Rgba8::GREEN);

	// -Y
	AddVertsForQuad3D(cubeVerts, Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f), Rgba8::MAGENTA);

	// +Z
	AddVertsForQuad3D(cubeVerts, Vec3(0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Rgba8::BLUE);

	// -Z
	AddVertsForQuad3D(cubeVerts, Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f), Rgba8::YELLOW);

	g_theRenderer->DrawVertexArray(cubeVerts);
}

void Prop::RenderSphere() const
{
	std::vector<Vertex_PCU> sphereVerts;

	AddVertsForSphere3D(sphereVerts, Vec3(0.f, 0.f, 0.f), 1.f, Rgba8::WHITE, AABB2(Vec2::ZERO, Vec2::ONE), 32, 16);
	//AddVertsForArrow3D(sphereVerts, Vec3::ZERO, Vec3(0.f, 1.f, 0.f), 0.2f, Rgba8::LIMEGREEN);
	//AddVertsForPyramidZ3D(sphereVerts, Vec3::ONE, 10.f, 6.f);
	//AddVertsForPyramid3D(sphereVerts, Vec3::ONE, 5.f, 8.f, Vec3(90.f, 90.f, 90.f));
	//AddVertsForMathArrow3D(sphereVerts, Vec3::ZERO, Vec3(0.f, 0.f, 1.f), 0.2f);
	//AddVertsForCylinderZ3D(sphereVerts, Vec3::ZERO, 0.5f, 2.f, Rgba8::LIMEGREEN);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->SetModelConstants(GetModelToWorldTransform(), m_color);
	g_theRenderer->DrawVertexArray(sphereVerts);
}
