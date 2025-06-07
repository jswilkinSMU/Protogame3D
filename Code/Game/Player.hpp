#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.h"
// -----------------------------------------------------------------------------
class Player : public Entity
{
public:
	Player(Game* owner, Vec3 const& position);
	~Player();

	void Update(float deltaSeconds) override;
	void Render() const override;
	Vec3 GetForwardNormal() const;

	Camera GetPlayerCamera() const;

private:
	void CameraKeyPresses(float deltaSeconds);
	void CameraControllerPresses(float deltaSeconds);
	Camera m_playerCamera;
};