#include "pch.h"
#include "HacksContainer.h"

void HacksContainer::init()
{
	fullMapAttackV1 = std::make_shared<FullMapAttackV1>();
	hacks.push_back(fullMapAttackV1);

	genericND = std::make_shared<GenericND>();
	hacks.push_back(genericND);

	flashJumpND = std::make_shared<FlashJumpND>();
	hacks.push_back(flashJumpND);

	jumpDownAnywhere = std::make_shared<JumpDownAnywhere>();
	hacks.push_back(jumpDownAnywhere);

	noCatchBreath = std::make_shared<NoCatchBreath>();
	hacks.push_back(noCatchBreath);

	unlimitedAttack = std::make_shared<UnlimitedAttack>();
	hacks.push_back(unlimitedAttack);
}