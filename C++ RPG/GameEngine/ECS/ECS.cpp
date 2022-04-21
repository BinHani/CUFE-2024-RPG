#include "ECS.h"

void Entity::addGroup(Group mGroup) noexcept {

	groupBitSet[mGroup] = true;
	manager.addToGroup(this, mGroup);
}