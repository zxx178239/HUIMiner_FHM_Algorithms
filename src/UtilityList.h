#pragma once
#include <vector>
#include "pub_def.h"
typedef struct UtilityListEntry
{
	int tid;
	double iutil;
	double rutil;
	UtilityListEntry() : tid(-1), iutil(0.0), rutil(0.0){}

	UtilityListEntry(int utid, double uIUtil, double uRUtil) : tid(utid), iutil(uIUtil), rutil(uRUtil){}
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

#ifdef NZEU
	double sumNZEU;
#endif

	double sumRUtil;
	int curCounts;

	UTILITYLISTENTRY *itemEntries;


	int supports;

};

