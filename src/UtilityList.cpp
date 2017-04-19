#include "UtilityList.h"
#include <cstdlib>

UtilityList::UtilityList(void) : curCounts(0), supports(0), sumIUtil(0.0), sumRUtil(0.0), itemEntries(NULL)
#ifdef NZEU
	, sumNZEU(0.0)
#endif
{
}

UtilityList::UtilityList(int uItem, int uItemSupports) : item(uItem), curCounts(0), supports(uItemSupports), sumIUtil(0.0), sumRUtil(0.0), itemEntries(NULL)
#ifdef NZEU
	, sumNZEU(0.0)
#endif
{
	itemEntries = new UTILITYLISTENTRY[supports];
}

UtilityList::~UtilityList(void)
{
	delete [] itemEntries;
	itemEntries = NULL;
}

void UtilityList::appendEntry(UTILITYLISTENTRY *curEntry)
{
	itemEntries[curCounts ++] = *curEntry;
	sumIUtil += curEntry->iutil;
	sumRUtil += curEntry->rutil;

#ifdef NZEU
	sumNZEU = ((curEntry->rutil == 0) ? sumNZEU : sumNZEU + curEntry->iutil);
#endif
}

// void UtilityList::constuctCurULists(std::vector<UTILITYLISTENTRY> &allULists)
// {
// 	supports = allULists.size();
// 	itemEntries = new UTILITYLISTENTRY[supports];
// 	
// 	for (int index = 0; index != supports; ++ index)
// 	{
// 		itemEntries[curCounts ++] = allULists[index];
// 	}
// }