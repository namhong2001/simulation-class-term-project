#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(){
	
	struct land{
		int buy;		//0 or 1. Set it to 0 at the begining. If bought, set it to 1. 
		int production;
		int period;
		int unitprice;
	};
	
	double policy;		//policy is #ofseeds/#oflands
	struct land land[100];		//each seed which has been sowed  on a farmland, is a vegetable
	int num_seed = 1;			//initial number of seeds is 1 (this time, consider just one kind of seed, ex.cabbage)
	int i;
	for(i=0; i<100; i++) land[i].buy = 0;
	land[0].buy = 1;			//initial number of land is one, make land[0] be bought		
	int num_land;
	int money = 1000;		/*since this test is just about buying, not harvest, so no increase in money. 
							That is why I make the initial money large*/
	int s = 1;
	int S = 1.2;
	
	for(; money>0 ;){		//buying policy
		
		//get number of lands
		num_land = 0;
		for(i=0; i<100; i++){
			if (land[i].buy == 1) num_land ++;
		}
		
		policy = num_seed/num_land;
		
		if (policy<s){
			int num_buy;		//number of seeds to buy
			num_buy = S*num_land - num_seed;
			num_seed += num_buy;
			money -= 5*num_buy;
			printf("Buy %d seed(s). Current money is %d\n", num_buy, money);
		}
		
		else{
			land[num_land].buy = 1;		//buy one land
			money -= 100;
			printf("Buy a land. Current money is %d\n", money);
		}
		
	}
	return 0;
	
} 

