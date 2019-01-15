#ifndef HOBJ_AURA_TRAIN_PARAMETER_H_INCLUDED
#define HOBJ_AURA_TRAIN_PARAMETER_H_INCLUDED
#include "IO/endian.h"
#include "IO/BINA.h"

namespace HedgeLib::RFL
{
	struct DObjAuraTrainParameter
	{
		HedgeLib::IO::BINA::DBINAV2DataNode Header;
		float frontDistance;
		float effectInterval;

		ENDIAN_SWAP(Header, frontDistance, effectInterval);
	};
}
#endif