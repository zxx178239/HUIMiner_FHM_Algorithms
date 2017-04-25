#include "HUIMiner.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <string>


HUIMiner::HUIMiner(void) : absoluteValue(-1), relativeValue(-1), totalNum(0), 
	itemPrices(NULL), itemTWUs(NULL), itemSupports(NULL), initUtilityLists(NULL), globalIndex(NULL), candidatesNum(0)
{
}


HUIMiner::~HUIMiner(void)
{
	delete [] itemPrices;
	itemPrices = NULL;

	for (int index = 0; index != sortItems.size(); ++ index)
	{
		delete initUtilityLists[index];
		initUtilityLists[index] = NULL;
	}

	initUtilityLists.clear();

	delete [] globalIndex;
	globalIndex = NULL;

}

void HUIMiner::launch(int argc, char *argv[])
{
	/*
		1. handle the parameters
		2. init some variables
		3. construct the utility lists
		4. delete some variables
		5. hui miner
		6. print the result
	*/

	// 1. handle the parameters
	if(!handleParameter(argc, argv))
		return;

	// 2. init some variables
	itemTWUs = new double[itemCounts];
	itemPrices = new double[itemCounts];
	itemSupports = new int[itemCounts];
	globalIndex = new int[itemCounts];

	for (int index = 0; index != itemCounts; ++ index)
	{
		itemTWUs[index] = 0.0;
		itemPrices[index] = 0.0;
		itemSupports[index] = 0;
		globalIndex[index] = -1;
	}

	// 3. construct the utility lists
	double s1 = clock();
	constructUtilityLists();
	double e1 = clock();
	constructTime = (e1 - s1) / CLOCKS_PER_SEC;

	// 4. delete some variables
	delete [] itemTWUs;
	itemTWUs = NULL;
	delete [] itemSupports;
	itemSupports = NULL;

	// 5. hui miner
	std::vector<int> prefix;
	double s2 = clock();
	huiMiner(prefix, NULL, initUtilityLists);
	double e2 = clock();
	miningTime = (e2 - s2) / CLOCKS_PER_SEC;

	// 6. print the result
	printResult();

	std::cout << "relative value: " << relativeValue << " total number: " << totalNum << " init time: " << constructTime << " mining time: " << miningTime << std::endl;
}

int HUIMiner::handleParameter(int argc, char *argv[])
{
	char *tmp;
	for (int index = 1; index < argc; ++ index)
	{
		if (argv[index][0] == '-' && (argv[index][1] == 'D' || argv[index][1] == 'd'))
		{
			tmp = argv[index]; tmp += 2; transName = tmp;
		}else if(argv[index][0] == '-' && (argv[index][1] == 'E' || argv[index][1] == 'e'))
		{
			tmp = argv[index]; tmp += 2; priceName = tmp;
		}else if(argv[index][0] == '-' && (argv[index][1] == 'O' || argv[index][1] == 'o'))
		{
			tmp = argv[index]; tmp += 2; resultFileName = tmp;
		}else if (argv[index][0] == '-' && (argv[index][1] == 'N' || argv[index][1] == 'n'))
		{
			tmp = argv[index]; tmp += 2; itemCounts = atoi(tmp);
		}else if (argv[index][0] == '-' && (argv[index][1] == 'W' || argv[index][1] == 'w'))
		{
			tmp = argv[index]; tmp += 2; transactionWeight = atoi(tmp);
		}else if (argv[index][0] == '-' && argv[index][1] == '#')
		{
			tmp = argv[index]; tmp += 2; transactionCounts = atoi(tmp);
		}else if (argv[index][0] == '-' && (argv[index][1] == 'S' || argv[index][1] == 's'))
		{
			tmp = argv[index]; tmp += 2; absoluteValue = atof(tmp);
		}else if (argv[index][0] == '-' && (argv[index][1] == 'U' || argv[index][1] == 'u'))
		{
			tmp = argv[index]; tmp += 2; relativeValue = atof(tmp);
		}

	}

	if (absoluteValue == -1 && relativeValue == -1)
	{
		return 0;
	}else
	{
		return 1;
	}
}

void HUIMiner::constructUtilityLists()
{
	/*
		1. first read db (database) to eliminate the irrelevant items
		2. init the utility lists
		3. second read db to construct the utility lists
	*/
	// 1. first read db (database) to eliminate the irrelevant items
	firstReadDB();

	// 2. init the utility lists
	initUtilitylist();

	// 3. second read db to construct the utility lists
	secondReadDB();
}

void HUIMiner::firstReadDB()
{
	/*
		1. read price file
		2. calculate the TWU of each item
	*/
	// 1. read price file
	readPriceFile();

	// 2. calculate the TWU of each item
	calculateTWU();
}

inline void HUIMiner::readPriceFile()
{
	std::ifstream ifs(priceName);
	for (int index = 0; index < itemCounts; ++ index)
	{
		ifs >> itemPrices[index];
	}
	ifs.close();
}

void HUIMiner::calculateTWU()
{
	std::ifstream ifs(transName);
	int firstPara, secondPara, thirdPara;

	int curLineTid, curLineCounts;

	int *curRowItems = new int[transactionWeight];
	int curQuantity;

	double transactionUtility = 0.0;
	double sumTransactionUtility = 0.0;

	ifs >> firstPara >> secondPara >> thirdPara;

	while (ifs >> curLineTid >> curLineCounts)
	{
		// 1. calculate the transaction utility of each transaction
		for (int index = 0; index < curLineCounts; ++ index)
		{
			ifs >> curRowItems[index] >> curQuantity;
			transactionUtility += curQuantity * itemPrices[curRowItems[index]];
		}

		// 2. calculate the TWU of each item
		for (int index = 0; index < curLineCounts; ++ index)
		{
			itemTWUs[curRowItems[index]] += transactionUtility;
			itemSupports[curRowItems[index]] ++;
		}
		sumTransactionUtility += transactionUtility;

		transactionUtility = 0.0;
	}

	absoluteValue = (absoluteValue == -1 ? sumTransactionUtility * relativeValue : absoluteValue);

	relativeValue = (relativeValue == -1 ? absoluteValue / sumTransactionUtility : relativeValue);

	delete [] curRowItems;
	curRowItems = NULL;

	ifs.close();
}

void HUIMiner::initUtilitylist()
{
	/*
		1. sort items in ascending order of TWU
		2. init the utility lists
	*/

	// 1. sort items in ascending order of TWU
	for (int index = 0; index != itemCounts; ++ index)
	{
		if (itemTWUs[index] >= absoluteValue)
		{
			sortItems.push_back(index);
			int scan = sortItems.size() - 2;
			for (; scan >= 0 && itemTWUs[sortItems[scan]] > itemTWUs[index]; -- scan)
			{
				sortItems[scan + 1] = sortItems[scan];
			}
			sortItems[scan + 1] = index;
		}
	}

	// 2. init the utility lists
	initUtilityLists.resize(sortItems.size());

	for (int index = 0; index != sortItems.size(); ++ index)
	{
		globalIndex[sortItems[index]] = index;
		initUtilityLists[index] = new UtilityList(sortItems[index], itemSupports[sortItems[index]]);
	}
}

void HUIMiner::secondReadDB()
{
	std::ifstream ifs(transName);

	int firstPara, secondPara, thirdPara;
	
	int *curRowItems = new int[transactionWeight];
	int *curRowQuantities = new int[transactionWeight];
	int curLineTid, curLineLength;
	int curItem, curQuantity;
	int curCounts = 0;
	double curTransactionUtility = 0.0;
#ifdef EUCS_Strategy
	double tmpTU = 0.0;
#endif

	ifs >> firstPara >> secondPara >> thirdPara;

	while (ifs >> curLineTid >> curLineLength)
	{
		// 1. reorder current transaction
		for (int index = 0; index != curLineLength; ++ index)
		{
			ifs >> curItem >> curQuantity;

			if(globalIndex[curItem] == -1)
				continue;

			curTransactionUtility += curQuantity * itemPrices[curItem];

			int scan = curCounts - 1;
			for (; scan >= 0 && itemTWUs[curRowItems[scan]] > itemTWUs[curItem]; -- scan)
			{
				curRowItems[scan + 1] = curRowItems[scan];
				curRowQuantities[scan + 1] = curRowQuantities[scan];
			}
			curRowItems[scan + 1] = curItem;
			curRowQuantities[scan + 1] = curQuantity;
			curCounts ++;
		}
#ifdef EUCS_Strategy
		tmpTU = curTransactionUtility;
#endif
		// 2. init the utility lists of each item
		for (int index = 0; index != curCounts; ++ index)
		{
			double iutil = curRowQuantities[index] * itemPrices[curRowItems[index]];
			UTILITYLISTENTRY curEntry(curLineTid, iutil, curTransactionUtility - iutil
#ifdef Add_PUtil
				, 0.0
#endif
				);
			initUtilityLists[globalIndex[curRowItems[index]]]->appendEntry(&curEntry);
			curTransactionUtility -= iutil;

#ifdef EUCS_Strategy
			for (int scan = index + 1; scan != curCounts; ++ scan)
			{
				int hashKey = curRowItems[index] * itemCounts + curRowItems[scan];
				if(eucsHash.find(hashKey) != eucsHash.end())
					eucsHash[hashKey] += tmpTU;
				else
					eucsHash[hashKey] = tmpTU;
			}
#endif
		}
		curCounts = 0;
	}

	delete [] curRowItems;
	curRowItems = NULL;

	delete [] curRowQuantities;
	curRowQuantities = NULL;
	ifs.close();
}

void HUIMiner::huiMiner(std::vector<int> &prefix, UtilityList *parent, std::vector<UtilityList *> &children)
{
	int childrenNum = children.size();
	

	for(int i = 0; i != childrenNum; ++ i) 
	{
		UtilityList* newParent = children[i];
		double iutil = newParent->sumIUtil;
#ifdef NZEU
		double nzeu = newParent->sumNZEU;
#endif
		double rutil = newParent->sumRUtil;
		std::vector<int> newPrefix(prefix);

		newPrefix.push_back(newParent->item);

	
		if(iutil >= absoluteValue) 
		{
			totalNum ++;
		}

		// 20170418, I delete the condition, and I need to test the influence
// 		if(childrenNum == 1) 
// 		{
// 			break;
// 		}
		
		// 20170418 add rutil != 0.0, it is not very useful, but I need it through analysis
#ifdef NZEU
		if(rutil != 0.0 && nzeu + rutil >= absoluteValue)
#else
		if(rutil != 0.0 && iutil + rutil >= absoluteValue) 
#endif
		{
			std::vector<UtilityList *> newChildren; 
			for(int k = i + 1; k != childrenNum; ++ k) 
			{
				UtilityList* neighUL = children[k];
#ifdef EUCS_Strategy
				int hashKey = newParent->item * itemCounts + neighUL->item;
				if (eucsHash.find(hashKey) == eucsHash.end() || eucsHash[hashKey] < absoluteValue)
				{
					continue;
				}
				
#endif
				candidatesNum ++;
				
				//20170418 add the condition, it is not very useful, but I need it through analysis
				if ((newParent->itemEntries[newParent->curCounts - 1].tid < neighUL->itemEntries[0].tid) 
					|| (newParent->itemEntries[0].tid > neighUL->itemEntries[neighUL->curCounts - 1].tid))
				{
					continue;
				}

				UtilityList* newChild = construct(parent, newParent, neighUL);
				if(newChild != NULL) 
				{
					newChildren.push_back(newChild);
				}
			}

			// 20170418 add the if condition, we need to mine recursively when the newChildren is not empty
			if(!newChildren.empty())
			{
				huiMiner(newPrefix, newParent, newChildren);

				for (int index = 0; index != newChildren.size(); ++ index)
				{
					delete newChildren[index];
					newChildren[index] = NULL;
				}
				newChildren.clear();
			}
		}
	}
}

UtilityList *HUIMiner::construct(UtilityList *pUL, UtilityList *X, UtilityList *Y)
{
	int xNumber = X->curCounts;
	int yNumber = Y->curCounts;
	int pNumber = ((pUL == NULL) ? 0 : pUL->curCounts);
	int index = 0, scan = 0, k = 0;


	UtilityList *pxy = new UtilityList(Y->item, xNumber < yNumber ? xNumber : yNumber);
	//int counts = 0;
	
//	std::vector<UtilityListEntry> curAllULists;
//	double iutilSum = 0.0, rutilSum = 0.0;

	UtilityListEntry curEntry = X->itemEntries[index];
	UTILITYLISTENTRY curEntryY = Y->itemEntries[scan];
// 	int xTID = curEntry.tid;
// 	int yTID = curEntryY.tid;

#ifdef LA_Prune
	bool flag = false;
	double totalUtility = X->sumIUtil + X->sumRUtil;
#endif


	while (index != xNumber && scan != yNumber)
	{
		if (curEntry.tid > curEntryY.tid)
		{
			curEntryY = Y->itemEntries[++ scan];
		}else if (curEntry.tid < curEntryY.tid)
		{
			curEntry = X->itemEntries[++ index];
#ifdef LA_Prune
			flag = false;
#endif
		}else
		{

#ifdef LA_Prune
			flag = true;
#endif
			double curiutil = 0;

			if (pNumber == 0)
			{
				curiutil = curEntry.iutil + curEntryY.iutil;
			}else
			{
			//	k = binarySearch(pUL->itemEntries, curEntry.tid, k, pNumber);
#ifdef Add_PUtil
				curiutil = curEntry.iutil + curEntryY.iutil - curEntry.putil;
#else
				while (k < pNumber && pUL->itemEntries[k].tid != curEntry.tid)
				{
					k ++;
				}

				curiutil = curEntry.iutil + curEntryY.iutil - pUL->itemEntries[k ++].iutil;
#endif

			}

			UTILITYLISTENTRY newEntry(curEntry.tid, curiutil, curEntryY.rutil
#ifdef Add_PUtil
				, curEntry.iutil
#endif
				);
//			counts ++;
//			iutilSum += curiutil;

//			rutilSum += curEntryY.rutil;
// 			if (curEntryY.rutil == 0.0)
// 			{
// 				std::cout << '.';
// 			}
			pxy->appendEntry(&newEntry);
//			curAllULists.push_back(newEntry);

			curEntryY = Y->itemEntries[++ scan];

			curEntry = X->itemEntries[++ index];
		}

	}
	
#ifdef LA_Prune
	if (scan == yNumber && flag == false)
	{
		totalUtility -= (curEntry.iutil + curEntry.rutil);
		if(totalUtility < absoluteValue)
		{
			delete pxy;
			return NULL;
		}
	}
#endif

	if (pxy->sumIUtil == 0.0)
	{
		delete pxy;
		pxy = NULL;
	}

	return pxy;
}


void HUIMiner::printResult()
{
	time_t t = time(0); 
	char tmp[32]={NULL};
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&t));

	char machinename[1024];
#ifdef  Linux
	struct utsname u;
	uname(&u); // cout << u.sysname << u.release << u.machine << u.nodename << endl;
	strcpy(machinename, u.nodename);

#else
	size_t requiredSize;
	getenv_s( &requiredSize, machinename, 1024, "computername" );

#endif

	std::ofstream ofs(resultFileName, std::ios::app);

	// determine the algorithm name
	std::string algoName("HUIMiner");

#ifdef EUCS_Strategy
	algoName += "_EUCS";
#endif

#ifdef LA_Prune
	algoName += "_LA";
#endif

#ifdef NZEU
	algoName += "_NZEU";
#endif


	ofs << "Machine\t"
		<< "Algo\t"
		<< "Trans_Quan_File\t"
		<< "Item_Price_File\t"
		<< "absMinUtil\t"
		<< "relMinUtil\t"
		<< "candidates\t"
		<< "#UtilPatterns\t"
		<< "construct time(s)\t"
		<< "mining time(s)\t"
		<< "time(s)\t"
		<< "dbsize\t"
		<< "maxWidth\t"
		<< "numItemsDistinct\t"
		<< "when_starting\n";


	ofs << machinename << "\t";

// #if defined HUIMINER && !defined EUCS_Strategy && !defined LA_Prune
// 	ofs << "HUIMiner\t";
// #elif defined HUIMINER && defined EUCS_Strategy && !defined LA_Prune
// 	ofs << "FHM\t";
// #elif defined HUIMINER && !defined EUCS_Strategy && defined LA_Prune
// 	ofs << "HUPMiner\t";
// #elif defined HUIMINER && defined EUCS_Strategy && defined LA_Prune
// 	ofs << "HUIMiner_EUCS_LA\t";
// #endif
	ofs << algoName << "\t";


	ofs	<< transName << "\t"
		<< priceName << "\t"
		<< absoluteValue << "\t"
		<< relativeValue << "\t"
		<< candidatesNum << "\t"
		<< totalNum << "\t"
		<< constructTime << "\t"
		<< miningTime << "\t"
		<< constructTime + miningTime << "\t"
		<< transactionCounts << "\t"
		<< transactionWeight << "\t"
		<< itemCounts << "\t"
		<< tmp << "\n";



	ofs.close();
}

// inline int HUIMiner::binarySearch(UTILITYLISTENTRY *curEntries, int curTID, int start, int end)
// {
// 	int mid;
// 
// 	while (start < end)
// 	{
// 		mid = (start + end) / 2; 
// 		if (curEntries[mid].tid == curTID)
// 		{
// 			return mid;
// 		}else if(curEntries[mid].tid < curTID)
// 		{
// 			start = mid + 1;
// 		}else
// 		{
// 			end = mid - 1;
// 		}
// 	}
// 	return start;
// }