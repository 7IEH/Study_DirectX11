#include "pch.h"
#include "EHEntity.h"

UINT Entity::g_NextID = 0;

Entity::Entity()
	:m_ID(g_NextID++)
{
}

Entity::~Entity()
{
}
