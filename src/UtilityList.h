#pragma once
#include <vector>
#include "pub_def.h"
typedef struct UtilityListEntry
{
	int tid;
	double iutil;
	double rutil;
#ifdef Add_PUtil
	double putil;
#endif
	UtilityListEntry() : tid(-1), iutil(0.0), rutil(0.0)
#ifdef Add_PUtil
		, putil(0.0)
#endif
	{}

	UtilityListEntry(int utid, double uIUtil, double uRUtil
#ifdef Add_PUtil
		, double uPUtil
#endif
		) : tid(utid), iutil(uIUtil), rutil(uRUtil)
#ifdef Add_PUtil
		, putil(uPUtil)
#endif
	{}
}UTILITYLISTENTRY;


class UtilityList
{
public:
	UtilityList(void);
	UtilityList(int uItem, int uItemSupports);
	~UtilityList(void);

public:
	void appendEntry(UTILITYLISTENTRY *curEntry);

	//void constuctCurULists(std::vector<UTILITYLISTENTRY> &allULists);

public:
	int item;
	double sumIUtil;

#ifdef TKO_NZEU
	double sumNZEU;
#endif

	double sumRUtil;
	int curCounts;

	UTILITYLISTENTRY *itemEntries;


	int supports;

};

