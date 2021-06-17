#pragma once

#include <vector>
#include <memory>

#include "Singleton.h"
#include "Hack.h"

#include "FullMapAttackV1.h"
#include "GenericND.h"
#include "FlashJumpND.h"
#include "JumpDownAnywhere.h"
#include "NoCatchBreath.h"
#include "UnlimitedAttack.h"

class HacksContainer : public Singleton<HacksContainer>
{
public:
	std::vector<std::shared_ptr<Hack>> hacks;

	std::shared_ptr<FullMapAttackV1> fullMapAttackV1;
	std::shared_ptr<GenericND> genericND;
	std::shared_ptr<FlashJumpND> flashJumpND;
	std::shared_ptr<JumpDownAnywhere> jumpDownAnywhere;
	std::shared_ptr<NoCatchBreath> noCatchBreath;
	std::shared_ptr<UnlimitedAttack> unlimitedAttack;

	void init();
};