#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <queue>
#include <vector>
#include "random.c"

#define NUM_EVENTS 5
#define NUM_VALUES_MANAGEMENT 4
#define BUSY 1
#define IDLE 2
#define BUYING_EVENT 2
#define SOWING_EVENT 3
#define MANAGEMENT_EVENT 4
#define HARVEST_EVENT 0
#define END_EVENT 1
#define TIME_LIMIT 1.0e+30
#define NONE_MANAGEMENT 1
#define REMOVE_WORM 2
#define LOOSEN_SOIL 3
#define BOTH_MANAGEMENT 4
#define MAX_FARMLAND 500
#define MAX_SEED 5000

using namespace std;

class MyComp {
public:
    bool operator() (const pair<float, int> &lhs, const pair<float, int> &rhs) {
        return lhs.first > rhs.first;
    }
};

using event_queue = priority_queue<pair<float, int>, vector<pair<float, int> >, MyComp>;
struct land_info {
    int status;
    int production;
};

land_info land_infos[MAX_FARMLAND];

int   initial_inv_level, end_time, num_policies, initial_seed, initial_num_land, seed, num_land, land_cost,
      growing_period, initial_production, seed_cost, sell_price,
	  remove_worm_cost, loosen_soil_cost, remove_worm_production, loosen_soil_production,
      bigs;
      
float sim_time, time_last_event, prob_distrib_management[NUM_VALUES_MANAGEMENT],
		smalls, initial_money, money;

event_queue buying_event_queue;
event_queue sowing_event_queue;
event_queue management_event_queue;
event_queue harvest_event_queue;
event_queue end_event_queue;

event_queue event_list[NUM_EVENTS] = {
    harvest_event_queue,
    end_event_queue,
    buying_event_queue,
    sowing_event_queue,
    management_event_queue
};
		
FILE  *infile, *outfile;

void  initialize(void);
pair<int,int> timing(void);
void  buying(void);
void  sowing(int index);
void  management(int index);
void  harvest(int index);

void  report(void);

float expon(float mean);
int   random_integer(float prob_distrib []);
float uniform(float a, float b);

float lcgrand(int stream);
void lcgrandst (long zset, int stream);
long lcgrandgt (int stream);


int main()  /* Main function. */
{
 	int i, next_event_type, index;
    /* Open input and output files. */
    infile  = fopen("proto_in.txt",  "r");
    outfile = fopen("proto_out.txt", "w");

    /* Read input parameters. */

    fscanf(infile, "%f%d%d%d%d%d%d%d%d%d%d%d%d",
			&initial_money, &initial_seed, &initial_num_land, &end_time,
			&growing_period, &initial_production, &land_cost, &seed_cost, &sell_price,
			&remove_worm_cost, &loosen_soil_cost, &remove_worm_production, &loosen_soil_production);
 
    for (i = 1; i < NUM_VALUES_MANAGEMENT; ++i)
        fscanf(infile, "%f", &prob_distrib_management[i]);

	fscanf(infile, "%d", &num_policies);
	
	
	fprintf(outfile, "   policy            money           seed             land\n");
	
	

    for (i = 1; i <= num_policies; ++i) {

        /* Read the inventory policy, and initialize the simulation. */

        fscanf(infile, "%f %d", &smalls, &bigs);
        initialize();

        /* Run the simulation until it terminates after an end-simulation event
           (type 3) occurs. */

        do {

            /* Determine the next event. */

            pair<int, int> timing_ret = timing();
            next_event_type = timing_ret.first;
            index = timing_ret.second;

            /* Update time-average statistical accumulators. */


            /* Invoke the appropriate event function. */
            

            switch (next_event_type) {
                case BUYING_EVENT:
                    buying();
                    break;
                case SOWING_EVENT:
                    sowing(index);
                    break;
                case MANAGEMENT_EVENT:
                    management(index);
                    break;
                case HARVEST_EVENT:
                    harvest(index);
                    break;
                case END_EVENT:
                	report();
                	break;
                default :
                	printf("\nError: next_event_type has not acceptable value\n");
                	exit(1);
            }
          
        /* If the event just executed was not the end-simulation event (type 3),
           continue simulating.  Otherwise, end the simulation for the current
           (s,S) pair and go on to the next pair (if any). */

        } while (next_event_type != END_EVENT);
    }

    /* End the simulations. */

    fclose(infile);
    fclose(outfile);

    return 0;
}


void initialize(void)  /* Initialization function. */
{
    int i;
    /* Initialize the simulation clock. */

    sim_time = 0.0;

    /* Initialize the state variables. */

    seed = initial_seed;
    money = initial_money;
    num_land = initial_num_land;
    
    for (i=0; i<MAX_FARMLAND; ++i) {
        land_infos[i].status = IDLE;
        land_infos[i].production = initial_production;
    } 
    
    time_last_event = 0.0;

    event_list[BUYING_EVENT].push(make_pair(0.0, -1)); // -1 means "no index"
    event_list[END_EVENT].push(make_pair(end_time, -1)); // -1 means "no index" 
}


pair<int, int> timing(void)  /* Timing function. */
{
    const int no_event = -1;
    const int first_event = 0;
    int event_type;
    int index;
    int next_event_type;
    float min_time_next_event = TIME_LIMIT / 10;

    next_event_type = no_event;

    /* Determine the event type of the next event to occur. */

    for (event_type = first_event; event_type < NUM_EVENTS; ++event_type) {
        event_queue cur_queue = event_list[event_type];

        if (!cur_queue.empty() && cur_queue.top().first < min_time_next_event) { 
            min_time_next_event = cur_queue.top().first;
            next_event_type     = event_type;
        }
    }

    /* Check to see whether the event list is empty. */

    if (next_event_type == no_event) {

        /* The event list is empty, so stop the simulation */

        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */

    pair<float, int> next_event = event_list[next_event_type].top(); // pair<float time, int index>
    event_list[next_event_type].pop();
    sim_time = next_event.first;
    return make_pair(next_event_type, next_event.second);
}

void buying(void) {
	float policy;		//policy is #ofseeds/#oflands
    float origin_money;
    int index;

    while (money > 0 && origin_money != money) {
        origin_money = money;
		
		policy = seed/(float)num_land;
		
	
		if (policy < smalls) { 
            int possible_max = (int)money / seed_cost;
			int num_buy = bigs*num_land - seed; // number of seeds to buy
            if (possible_max < num_buy) {
                num_buy = possible_max;
            } 
			seed += num_buy;
			money -= seed_cost*num_buy;
            if (num_buy > 0) {
			    printf("sim_time: %f, Buy %d seed(s). Current money is %f\n", sim_time, num_buy, money);
            }
		}
        else if (money > land_cost) {
            money -= land_cost;
            ++num_land;
			printf("sim_time: %f, Buy a land. Current money is %f\n", sim_time, money);
		}

    }

    for (index=0; index<num_land; ++index) {
        if (land_infos[index].status == IDLE && seed > 0) {
            sowing(index);
        }
    }
	event_list[BUYING_EVENT].push(make_pair(sim_time + 1, -1));
	return;
}

void sowing(int index) {
	if (land_infos[index].status == IDLE && seed > 0) { // actually this is an duplication
		printf("sim_time: %f, sow 1 seed\n", sim_time);
		land_infos[index].status = BUSY;
		seed -= 1;
        management(index);
		event_list[HARVEST_EVENT].push(make_pair(sim_time + growing_period, index));
	}	
}

void management(int index) {
	int management_type = random_integer(prob_distrib_management);	
    switch(management_type){
        case NONE_MANAGEMENT: 										//ran is 0, nothing
            printf("sim_time: %f, Do nothing.\n", sim_time);
            break;
            
        case REMOVE_WORM:											//ran is 1, remove worms
            if (money >= remove_worm_cost) {
                land_infos[index].production += remove_worm_production;
                money -= remove_worm_cost;
                printf("sim_time: %f, Remove worms.         money: %f\n", sim_time, money);
            } else {
                printf("sim_time: %f, Remove worms failded. money: %f\n", sim_time, money);
            }
            break;
            
        case LOOSEN_SOIL:											//ran is 2, loosen soil
            if (money >= loosen_soil_cost) {
                land_infos[index].production += loosen_soil_production;
                money -= loosen_soil_cost;
                printf("sim_time: %f, Loosen soil.          money: %f\n", sim_time, money);
            } else {
                printf("sim_time: %f, Loosen soil failed.   money: %f\n", sim_time, money);
            }
            break;
         
        case BOTH_MANAGEMENT:											//ran is 3, remove worms & loosen soil
            if (money >= remove_worm_cost + loosen_soil_cost) {
                land_infos[index].production += remove_worm_production + loosen_soil_production;
                money -= remove_worm_cost + loosen_soil_cost;
                printf("sim_time: %f, Remove worms and lossen soil.      money: %f\n", sim_time, money);
            } else {
                printf("sim_time: %f, Remove worms and lossen soil failed. money: %f\n", sim_time, money);
            }
            break;
            
        default:
            printf("Error: Management type is invalid.\n");	
            exit(1);
    }	
}

void harvest(int index) {
	money += sell_price * land_infos[index].production;
	
	printf("sim_time: %f, harvest. income : %d      money: %f\n", sim_time, sell_price * land_infos[index].production, money);
	
    // initialize
	land_infos[index].status = IDLE;
    land_infos[index].production = initial_production;
	
    if (seed > 0) {
        sowing(index);
    }
}

void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    fprintf(outfile, "(%2.1f, %3d)%15.2f%17d%17d\n\n",
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
