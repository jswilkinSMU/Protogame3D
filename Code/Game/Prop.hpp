#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.h"
#include <vector>
// -----------------------------------------------------------------------------
struct Rgba8;
class  Texture;
// -----------------------------------------------------------------------------
class Prop : public Entity
{
public:
	Prop(Game* owner, Vec3 const& position);
	~Prop();

	void Update(float deltaSeconds) override;
	void Render() const override;

	void RenderCube() const;
	void RenderSphere() const;
private:
	std::vector<Vertex_PCU> m_vertexes;
	Texture* m_texture = nullptr;

};