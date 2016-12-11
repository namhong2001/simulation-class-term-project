#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NUM_EVENTS 5
#define NUM_VALUES_MANAGEMENT 4
#define BUSY 1
#define IDLE 2


int   initial_inv_level, next_event_type,
      end_time, num_policies, initial_seed, initial_num_land, seed, num_land, growing_period, initial_productivity, productivity, buy_price, sell_price,
	  remove_worm_cost, loosen_soil_cost, land_status;
      
float sim_time, time_last_event, time_next_event[5], prob_distrib_management[NUM_VALUES_MANAGEMENT],
		smalls, bigs, initial_money, money;
		
FILE  *infile, *outfile;

void  initialize(void);
void  timing(void);
void  buying(void);
void  sowing();
void  management();
void  harvest();


void  report(void);
void  update_time_avg_stats(void);

float expon(float mean);
int   random_integer(float prob_distrib []);
float uniform(float a, float b);

float lcgrand(int stream);
void lcgrandst (long zset, int stream);
long lcgrandgt (int stream);


int main()  /* Main function. */
{
 	int i;
    /* Open input and output files. */
    infile  = fopen("proto_in.txt",  "r");
    outfile = fopen("proto_out.txt", "w");

    /* Read input parameters. */

    fscanf(infile, "%f%d%d%d%d%d%d%d%d%d",
			&initial_money, &initial_seed, &initial_num_land, &end_time,
			&growing_period, &initial_productivity, &buy_price, &sell_price,
			&remove_worm_cost, &loosen_soil_cost);
 
    for (i = 1; i < NUM_VALUES_MANAGEMENT; ++i)
        fscanf(infile, "%f", &prob_distrib_management[i]);

	fscanf(infile, "%d", &num_policies);
	
	
	fprintf(outfile, "   policy            money           seed             land\n");
	
	

    for (i = 1; i <= num_policies; ++i) {

        /* Read the inventory policy, and initialize the simulation. */

        fscanf(infile, "%f %f", &smalls, &bigs);
        initialize();

        /* Run the simulation until it terminates after an end-simulation event
           (type 3) occurs. */

        do {

            /* Determine the next event. */

            timing();

            /* Update time-average statistical accumulators. */

//            update_time_avg_stats();

            /* Invoke the appropriate event function. */
            

            switch (next_event_type) {
                case 1:
                    buying();
                    break;
                case 2:
                    sowing();
                    break;
                case 3:
                    management();
                    break;
                case 4:
                    harvest();
                    break;
                case 5:
                	report();
                	break;
                default :
                	printf("\nnext_event_type has not acceptable value\n");
                	exit(1);
            }
          
        /* If the event just executed was not the end-simulation event (type 3),
           continue simulating.  Otherwise, end the simulation for the current
           (s,S) pair and go on to the next pair (if any). */

        } while (next_event_type != 5);
    }

    /* End the simulations. */

    fclose(infile);
    fclose(outfile);

    return 0;
}


void initialize(void)  /* Initialization function. */
{
    /* Initialize the simulation clock. */

    sim_time = 0.0;

    /* Initialize the state variables. */

    seed = initial_seed;
    money = initial_money;
    num_land = initial_num_land;
    productivity = initial_productivity;
    
    land_status = IDLE; 
    
    
    time_last_event = 0.0;

    /* Initialize the event list.  Since no order is outstanding, the order-
       arrival event is eliminated from consideration. */

    time_next_event[1] = 0; //buying
    time_next_event[2] = 1.0e+30; //sowing
    time_next_event[3] = 1.0e+30; // management
    time_next_event[4] = 1.0e+30; // harvest
    time_next_event[5] = end_time; // end simulation
}


void timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    /* Determine the event type of the next event to occur. */

    for (i = 1; i <= NUM_EVENTS; ++i)
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }

    /* Check to see whether the event list is empty. */

    if (next_event_type == 0) {

        /* The event list is empty, so stop the simulation */

        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */

    sim_time = min_time_next_event;
}

void buying(void) {
	
		
//	struct land{
//		int buy;		//0 or 1. Set it to 0 at the begining. If bought, set it to 1. 
//		int production;
//		int period;
//		int unitprice;
//	};
//	
	float policy;		//policy is #ofseeds/#oflands
//	struct land land[100];		//each seed which has been sowed  on a farmland, is a vegetable
//	int num_seed = 1;			//initial number of seeds is 1 (this time, consider just one kind of seed, ex.cabbage)
	int i;
	float origin_money = money;
//	for(i=0; i<100; i++) land[i].buy = 0;
//	land[0].buy = 1;			//initial number of land is one, make land[0] be bought		
//	int num_land;
//	int money = 1000;		/*since this test is just about buying, not harvest, so no increase in money. 
//							That is why I make the initial money large*/
//	int s = 1;
//	int S = 1.2;
	
//	for(; money>0 ;){		//buying policy
		
		//get number of lands
//		num_land = 0;
//		for(i=0; i<100; i++){
//			if (land[i].buy == 1) num_land ++;
//		}
		
		policy = seed/(float)num_land;
		
	
		if (policy<smalls){
			int num_buy;		//number of seeds to buy
			num_buy = (int)bigs*num_land - seed;
			seed += num_buy;
			money -= buy_price*num_buy;
			printf("sim_time: %f, Buy %d seed(s). Current money is %f\n", sim_time, num_buy, money);
		}
		// do not buy land for prototype
//		else{
//			land[num_land].buy = 1;		//buy one land
//			money -= 100;
//			printf("Buy a land. Current money is %d\n", money);
//		}
		
//	}

	if (origin_money != money) {
		time_next_event[2] = sim_time;	
	}
	time_next_event[1] += 1;
	return;
}

void sowing() {
	if (land_status == IDLE) {
		printf("sim_time: %f, sow 1 seed\n", sim_time);
		land_status = BUSY;
		seed -= 1;
		time_next_event[3] = sim_time;
		time_next_event[4] = sim_time + growing_period;
	}
	
	time_next_event[2] = 1.0e+30;
}

void management() {
	
//	struct land{
//		int buy;		//0 or 1. Set it to 0 at the begining. If bought, set it to 1. 
//		int production;
//		int period;
//		int unitprice;
//	};
	
//	struct land land[100];		//each seed which has been sowed  on a farmland, is a vegetable
//	int i;
//	for(i=0; i<100; i++) land[i].buy = 0;		//set land[].buy to 0 at begining.
	
//	int money = 100;		//current total money
//	int ran;		//the random number
	
//	srand(time(NULL));		//set the "seed"(seed of Random Number, not related to the Farm)
	//ran = rand()%4;		//get 0,1,2,3 randomly.  1: nothing   2: worm   3: soil   4: worm and soil 
	
	//test statistics input 
//	for(i=0; i<10; i++){
//		land[i].buy = 1;
//		land[i].production = 10;
//		land[i].period = 5;
//		land[i].unitprice = 10;
//	}
	
//	for(i=0; land[i].buy==1; i++){

	int management_type = random_integer(prob_distrib_management);
		
//		ran = rand()%4;		//get a random number 0 1 2 3
		
		switch(management_type){
			case 1: 										//ran is 0, nothing
				printf("sim_time: %f, Do nothing.\n", sim_time);
				break;
				
			case 2:											//ran is 1, remove worms
				productivity +=5;
				money -= remove_worm_cost;
				printf("sim_time: %f, Remove worms.         money: %f\n", sim_time, money);
				break;
				
			case 3:											//ran is 2, loosen soil
				productivity +=5;
				money -= loosen_soil_cost;
				printf("sim_time: %f, Loosen soil.          money: %f\n", sim_time, money);
				break;
				
			case 4:											//ran is 3, remove worms & loosen soil
				productivity +=10;
				money -= remove_worm_cost + loosen_soil_cost;
				printf("sim_time: %f, Remove worms and lossen soil.      money: %f\n", sim_time, money);
				break;
				
			default: printf("Error.\n");	
		}
//	}
	
	time_next_event[3] = 1.0e+30;
}

void harvest() {
	money += sell_price * productivity;
	
	printf("sim_time: %f, harvest. income : %d      money: %f\n", sim_time, sell_price * productivity, money);
	
	land_status = IDLE;
	productivity = initial_productivity;
	
	time_next_event[4] = 1.0e+30;
	time_next_event[2] = sim_time;
}

void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    fprintf(outfile, "(%2.1f, %2.1f)%15.2f%17d%17d\n\n",
            smalls, bigs,
            money,
            seed, num_land);
}




float expon(float mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    return -mean * log(lcgrand(1));
//    return mean;
}


int random_integer(float prob_distrib[])  /* Random integer generation
                                             function. */
{
    int   i;
    float u;

    /* Generate a U(0,1) random variate. */

    u = lcgrand(1);

    /* Return a random integer in accordance with the (cumulative) distribution
       function prob_distrib. */

    for (i = 1; u >= prob_distrib[i]; ++i)
        ;
    return i;
}


float uniform(float a, float b)  /* Uniform variate generation function. */
{
    /* Return a U(a,b) random variate. */

    //return a + lcgrand(1) * (b - a);
    return (a + b) / 2;
}




/* Prime modulus multiplicative linear congruential generator
   Z[i] = (630360016 * Z[i-1]) (mod(pow(2,31) - 1)), based on Marse and Roberts'
   portable FORTRAN random-number generator UNIRAN.  Multiple (100) streams are
   supported, with seeds spaced 100,000 apart.  Throughout, input argument
   "stream" must be an int giving the desired stream number.  The header file
   lcgrand.h must be included in the calling program (#include "lcgrand.h")
   before using these functions.

   Usage: (Three functions)

   1. To obtain the next U(0,1) random number from stream "stream," execute
          u = lcgrand(stream);
      where lcgrand is a float function.  The float variable u will contain the
      next random number.

   2. To set the seed for stream "stream" to a desired value zset, execute
          lcgrandst(zset, stream);
      where lcgrandst is a void function and zset must be a long set to the
      desired seed, a number between 1 and 2147483646 (inclusive).  Default
      seeds for all 100 streams are given in the code.

   3. To get the current (most recently used) integer in the sequence being
      generated for stream "stream" into the long variable zget, execute
          zget = lcgrandgt(stream);
      where lcgrandgt is a long function. */

/* Define the constants. */

#define MODLUS 2147483647
#define MULT1       24112
#define MULT2       26143

/* Set the default seeds for all 100 streams. */

static long zrng[] =
{         1,
 1973272912, 281629770,  20006270,1280689831,2096730329,1933576050,
  913566091, 246780520,1363774876, 604901985,1511192140,1259851944,
  824064364, 150493284, 242708531,  75253171,1964472944,1202299975,
  233217322,1911216000, 726370533, 403498145, 993232223,1103205531,
  762430696,1922803170,1385516923,  76271663, 413682397, 726466604,
  336157058,1432650381,1120463904, 595778810, 877722890,1046574445,
   68911991,2088367019, 748545416, 622401386,2122378830, 640690903,
 1774806513,2132545692,2079249579,  78130110, 852776735,1187867272,
 1351423507,1645973084,1997049139, 922510944,2045512870, 898585771,
  243649545,1004818771, 773686062, 403188473, 372279877,1901633463,
  498067494,2087759558, 493157915, 597104727,1530940798,1814496276,
  536444882,1663153658, 855503735,  67784357,1432404475, 619691088,
  119025595, 880802310, 176192644,1116780070, 277854671,1366580350,
 1142483975,2026948561,1053920743, 786262391,1792203830,1494667770,
 1923011392,1433700034,1244184613,1147297105, 539712780,1545929719,
  190641742,1645390429, 264907697, 620389253,1502074852, 927711160,
  364849192,2049576050, 638580085, 547070247 };

/* Generate the next random number. */

float lcgrand(int stream)
{
    long zi, lowprd, hi31;

    zi     = zrng[stream];
    lowprd = (zi & 65535) * MULT1;
    hi31   = (zi >> 16) * MULT1 + (lowprd >> 16);
    zi     = ((lowprd & 65535) - MODLUS) +
             ((hi31 & 32767) << 16) + (hi31 >> 15);
    if (zi < 0) zi += MODLUS;
    lowprd = (zi & 65535) * MULT2;
    hi31   = (zi >> 16) * MULT2 + (lowprd >> 16);
    zi     = ((lowprd & 65535) - MODLUS) +
             ((hi31 & 32767) << 16) + (hi31 >> 15);
    if (zi < 0) zi += MODLUS;
    zrng[stream] = zi;
    return (zi >> 7 | 1) / 16777216.0;
}


void lcgrandst (long zset, int stream) /* Set the current zrng for stream
                                          "stream" to zset. */
{
    zrng[stream] = zset;
}


long lcgrandgt (int stream) /* Return the current zrng for stream "stream". */
{
    return zrng[stream];
}


