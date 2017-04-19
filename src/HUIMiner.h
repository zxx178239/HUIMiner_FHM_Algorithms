#pragma once
#include "pub_def.h"
#include "UtilityList.h"
#include <vector>

class HUIMiner
{
public:
	HUIMiner(void);
	~HUIMiner(void);

	void launch(int argc, char *argv[]);


private:
	// 1. handle the parameters
	int handleParameter(int argc, char *argv[]);

	// 2. construct the utility lists
	void constructUtilityLists();
	// 2.1 first read db
	void firstReadDB();
	void readPriceFile();
	void calculateTWU();

	// 2.2 init the utility lists
	void initUtilitylist();

	// 2.3 second read db
	void secondReadDB();

	// 3. mining
	void huiMiner(std::vector<int> &prefix, UtilityList *parent, std::vector<UtilityList *> &children);

	UtilityList* construct(UtilityList *pUL, UtilityList *X, UtilityList *Y);

	// I test the binary search, and I find that it is not good
	//int binarySearch(UTILITYLISTENTRY *curEntries, int curTID, int start, int end);

	// 4. print the result
	void printResult();
private:
	char *transName;
	char *priceName;
	char *resultFileName;
	double absoluteValue;
	double relativeValue;
	int	itemCounts;
	int	transactionWeight;
	int	transactionCounts;
	int partLineNumber;

private:
	double *itemPrices;
	double *itemTWUs;
	int *itemSupports;

	int *globalIndex;

	std::vector<UtilityList *> initUtilityLists;
	std::vector<int> sortItems;

#ifdef EUCS_Strategy
	hash_map<int, double> eucsHash;
#endif

	double constructTime;
	double miningTime;
	double totalTime;
	int totalNum;
	int candidatesNum;

#ifdef COUNT_THE_MEMORY_DELETE
	int newCounts;
#endif
};

