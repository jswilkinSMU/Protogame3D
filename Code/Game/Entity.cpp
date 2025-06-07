#include "Game/Entity.hpp"
#include "Engine/Math/Mat44.hpp"

Entity::Entity(Game* owner, Vec3 const& position)
	:m_game(owner), m_position(position)
{
}

Entity::~Entity()
{
}

Mat44 Entity::GetModelToWorldTransform() const
{
	Mat44 modelToWorldMatrix;
	modelToWorldMatrix.SetTranslation3D(m_position);
	modelToWorldMatrix.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWorldMatrix;
}
