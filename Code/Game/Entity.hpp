#pragma once
#include "Engine/Math/Vec3.h"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.h"
// -----------------------------------------------------------------------------
class Game;
class Mat44;
// -----------------------------------------------------------------------------
class Entity
{
public:
	Entity(Game* owner, Vec3 const& position);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual Mat44 GetModelToWorldTransform() const;

public:
	Game* m_game = nullptr;
	Rgba8 m_color = Rgba8::WHITE;

	Vec3 m_position = Vec3::ZERO;
	EulerAngles m_orientation = EulerAngles(0.f, 0.f, 0.f);
};