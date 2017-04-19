1. parameter introduce:
	in the main.cpp, I give the example for parameter of test file
	-D: the database file name
	-E: the price file name
	-O: the result file name
	-N: the number of item
	-W: the max length of transaction
	-#: the number of transaction
	-S: the absolute value of threshold
	-U: the relative value of threshold
	-F: the factor to choose pseudo or materialization
	-R: the number of recursive

2. algorithm control
	you need change the pub_def.h file when you run in different system and run different algorithms
	2.1 when you execute the HUIMiner algorithm, you just need to open the "#define HUIMINER"
	2.2 when you execute the FHM algorithm, you need to open the "#define HUIMINER" and "#define EUCS_Strategy"
	2.3 when you execute the LA_PRUNE algorithm, you need to open the "#define LA_Prune"
	2.4 when you execute the NZEU strategy of the TKO algorithm, you need to open the "#define NZEU"

3. the efficiency
	I have compared my HUIMiner with the implement of original author. When the threshold is very small, the algorithm of original author is up to 3 times faster than mine. Maybe, the author used some strategies that didn't mention in the paper. If you have some suggestion for improving the algorithm, please contact me!