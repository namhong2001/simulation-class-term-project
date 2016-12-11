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
	
	struct land land[100];		//each seed which has been sowed  on a farmland, is a vegetable
	int i;
	for(i=0; i<100; i++) land[i].buy = 0;		//set land[].buy to 0 at begining.
	
	int money = 100;		//current total money
	int ran;		//the random number
	
	srand(time(NULL));		//set the "seed"(seed of Random Number, not related to the Farm)
	//ran = rand()%4;		//get 0,1,2,3 randomly.  1: nothing   2: worm   3: soil   4: worm and soil 
	
	//test statistics input 
	for(i=0; i<10; i++){
		land[i].buy = 1;
		land[i].production = 10;
		land[i].period = 5;
		land[i].unitprice = 10;
	}
	
	for(i=0; land[i].buy==1; i++){
		
		ran = rand()%4;		//get a random number 0 1 2 3
		
		switch(ran){
			case 0: 										//ran is 0, nothing
				printf("Do nothing.\n");
				break;
				
			case 1:											//ran is 1, remove worms
				land[i].production +=5;
				printf("Remove worms.\n");
				break;
				
			case 2:											//ran is 2, loosen soil
				land[i].production +=5;
				printf("Loosen soil.\n");
				break;
				
			case 3:											//ran is 3, remove worms & loosen soil
				land[i].production +=10;
				printf("Remove worms and lossen soil.\n");
				break;
				
			default: printf("Error.\n");	
		}
	}
	
	return 0;
	
} 

