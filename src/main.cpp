#include "HUIMiner.h"
#include "pub_def.h"

//-Damount2.txt -Eprice2.txt -Ooutfile.txt -N8 -W6 -#5 -S30 -A5
//-Dchess.txt  -Elogn76.txt  -Osee-DDHUPF1R3.txt  -N76 -W37  -#3196  -U0.25     -K -F1 -R3 -C -A5

int main(int argc, char *argv[])
{
	HUIMiner myAlgorithm;
	myAlgorithm.launch(argc, argv);
}