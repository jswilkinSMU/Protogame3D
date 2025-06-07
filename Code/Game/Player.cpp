#include "Game/Player.hpp"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/MathUtils.h"
#include <Engine/Core/DevConsole.hpp>
#include <Engine/Core/DebugRender.hpp>

Player::Player(Game* owner, Vec3 const& position)
	:Entity(owner, position)
{
	m_position = position;
	m_orientation = EulerAngles(0.f, 0.f, 0.f);
	Mat44 cameraToRender(Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.f, 0.f, 0.f));
	m_playerCamera.SetCameraToRenderTransform(cameraToRender);
}

Player::~Player()
{
}

void Player::Update(float deltaSeconds)
{
	CameraKeyPresses(deltaSeconds);
	CameraControllerPresses(deltaSeconds);

	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);
	m_orientation.m_rollDegrees = GetClamped(m_orientation.m_rollDegrees, -45.f, 45.f);

	m_playerCamera.SetPositionAndOrientation(m_position, m_orientation);

	m_playerCamera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
}

void Player::Render() const
{
}

Vec3 Player::GetForwardNormal() const
{
	return Vec3::MakeFromPolarDegrees(m_orientation.m_pitchDegrees, m_orientation.m_yawDegrees, 2.f);
}

Camera Player::GetPlayerCamera() const
{
	return m_playerCamera;
}

void Player::CameraKeyPresses(float deltaSeconds)
{
	// Yaw and Pitch with mouse
	m_orientation.m_yawDegrees += 0.08f * g_theInput->GetCursorClientDelta().x;
	m_orientation.m_pitchDegrees -= 0.08f * g_theInput->GetCursorClientDelta().y;

	float movementSpeed = 2.f;
	// Increase speed by a factor of 10
	if (g_theInput->IsKeyDown(KEYCODE_SHIFT))
	{
		movementSpeed *= 10.f;
	}

	// Rolling
	if (g_theInput->IsKeyDown('Q'))
	{
		m_orientation.m_rollDegrees += -90.f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown('E'))
	{
		m_orientation.m_rollDegrees += 90.f * deltaSeconds;
	}

	// Move left or right
	if (g_theInput->IsKeyDown('A'))
	{
		m_position += movementSpeed * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetJBasis3D() * deltaSeconds;
	}
	if (g_theInput->IsKeyDown('D'))
	{
		m_position += -movementSpeed * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetJBasis3D() * deltaSeconds;
	}

	// Move Forward and Backward
	if (g_theInput->IsKeyDown('W'))
	{
		m_position += movementSpeed * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D() * deltaSeconds;
	}
	if (g_theInput->IsKeyDown('S'))
	{
		m_position += -movementSpeed * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D() * deltaSeconds;
	}

	// Move Up and Down
	if (g_theInput->IsKeyDown('Z'))
	{
		m_position += -movementSpeed * Vec3::ZAXE * deltaSeconds;
	}
	if (g_theInput->IsKeyDown('C'))
	{
		m_position += movementSpeed * Vec3::ZAXE * deltaSeconds;
	}

	// Reset position and orientation to zero
	if (g_theInput->WasKeyJustPressed('H'))
	{
		m_position = Vec3::ZERO;
		m_orientation = EulerAngles(0.f, 0.f, 0.f);
	}

	// Spawn Line/Cylinder
	if (g_theInput->WasKeyJustPressed('1'))
	{
		float lineRadius = 0.0625f;
		Vec3 lineLength = m_position + GetForwardNormal() * 10.f;
		DebugAddWorldCylinder(m_position, lineLength, lineRadius, 10.f, Rgba8::YELLOW, Rgba8::YELLOW, DebugRenderMode::X_RAY);
	}
	// Spawn point/sphere
	if (g_theInput->IsKeyDown('2'))
	{
		float pointRadius = 0.2f;
		Vec3 spawnPosition = Vec3(m_position.x, m_position.y, 0.f);
		DebugAddWorldSphere(spawnPosition, pointRadius, 60.f, Rgba8(150, 75, 0), Rgba8(150, 75, 0));
	}
	// Spawn wire sphere
	if (g_theInput->WasKeyJustPressed('3'))
	{
		Vec3 spawnPosition = m_position + GetForwardNormal();
		DebugAddWorldWireSphere(spawnPosition, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED);
	}
	// Spawn a world basis
	if (g_theInput->WasKeyJustPressed('4'))
	{
		DebugAddWorldBasis(GetModelToWorldTransform(), 20.f);
	}
	// Spawn full opposing billboard text
	if (g_theInput->WasKeyJustPressed('5'))
	{
		std::string posAndOrientationText = Stringf("Position: %0.1f %0.1f %0.1f, Orientation: %0.1f, %0.1f, %0.1f,",
			m_position.x, m_position.y, m_position.z, m_orientation.m_yawDegrees, m_orientation.m_pitchDegrees, m_orientation.m_rollDegrees);
		Vec3 spawnPosition = m_position + GetForwardNormal();
		float textSize = 0.15f;

		DebugAddWorldBillboardText(posAndOrientationText, spawnPosition, textSize, Vec2::ONEHALF, 10.f, Rgba8::WHITE, Rgba8::RED);
	}
	// Spawn wire cylinder
	if (g_theInput->WasKeyJustPressed('6'))
	{
		DebugAddWorldWireCylinder(m_position, m_position + Vec3::ZAXE, 0.5f, 10.f, Rgba8::WHITE, Rgba8::RED);
	}
	// Spawn message
	if (g_theInput->WasKeyJustPressed('7'))
	{
		std::string orientationText = Stringf("Orientation: %0.1f, %0.1f, %0.1f,", m_orientation.m_yawDegrees, m_orientation.m_pitchDegrees, m_orientation.m_rollDegrees);
		DebugAddMessage(orientationText, 5.f);
	}
}

void Player::CameraControllerPresses(float deltaSeconds)
{
	XboxController const& controller = g_theInput->GetController(0);
	float movementSpeed = 2.f;

	// Increase speed by a factor of 10
	if (controller.IsButtonDown(XBOX_BUTTON_A))
	{
		movementSpeed *= 10.f;
	}

	// Rolling
	if (controller.GetLeftTrigger())
	{
		m_orientation.m_rollDegrees = -90.f * deltaSeconds;
	}
	if (controller.GetRightTrigger())
	{
		m_orientation.m_rollDegrees = 90.f * deltaSeconds;
	}

	//// Move left, right, forward, and backward
	if (controller.GetLeftStick().GetMagnitude() > 0.f)
	{
		m_position += (-movementSpeed * controller.GetLeftStick().GetPosition().x * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetJBasis3D() * deltaSeconds);
		m_position += (movementSpeed * controller.GetLeftStick().GetPosition().y * m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D() * deltaSeconds);
	}

	//// Move Up and Down
	if (controller.IsButtonDown(XBOX_BUTTON_LSHOULDER))
	{
		m_position += -movementSpeed * Vec3::ZAXE * deltaSeconds;
	}
	if (controller.IsButtonDown(XBOX_BUTTON_RSHOULDER))
	{
		m_position += movementSpeed * Vec3::ZAXE * deltaSeconds;
	}

	//// Reset position and orientation to zero
	if (controller.WasButtonJustPressed(XBOX_BUTTON_START))
	{
		m_position = Vec3::ZERO;
		m_orientation = EulerAngles(0.f, 0.f, 0.f);
	}
}
