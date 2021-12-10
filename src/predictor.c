//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <string.h>

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t *g_BHT;  // gshare BHT
uint32_t g_history;  //global history
int g_BHT_index;
uint8_t g_result;
//tournament: local BHT, local PHT, global history, global BHT, selector
uint8_t *l_BHT;  // tournament local BHT
uint32_t *l_PHT;  // tournament local PHT
uint8_t *global_BHT;  // tournament global BHT
uint8_t *selector;  // selector fot tournament
uint8_t l_outcome; //result form local predict
uint8_t global_outcome; //result form global predict
int PHT_index;
int global_BHT_index;
//         ---------------------------//
//custom //Perceptron Branch Predictor//
//         ---------------------------//
int table_length_perceptron;
int num_weights;
int **weight_table;
int *bias_table;
int theta;
int max_weight_limit;
int min_weight_limit;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType) {
      g_history = 0;
    case STATIC:
      return;
    case GSHARE:
      g_BHT = malloc((1<<ghistoryBits)*sizeof(uint8_t));
      memset(g_BHT, WN, sizeof(uint8_t)*(1<<ghistoryBits));
      break;
    case TOURNAMENT:
      l_BHT = malloc((1 << lhistoryBits) * sizeof(uint8_t));
      l_PHT = malloc((1 << pcIndexBits) * sizeof(uint32_t));
      global_BHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
      selector = malloc((1 << ghistoryBits) * sizeof(uint8_t));
      memset(l_BHT, WN, sizeof(uint8_t) * (1 << lhistoryBits));
      memset(l_PHT, 0, sizeof(uint32_t) * (1 << pcIndexBits));
      memset(global_BHT, WN, sizeof(uint8_t) * (1 << ghistoryBits));
      memset(selector, WN, sizeof(uint8_t) * (1 << ghistoryBits));
      break;
    case CUSTOM:
      //Use perceptron predictor with @table_length_perceptron table length, @num_weights weights,
			//1 bias term and 8 bits for weight values
			//table_length_perceptron = 256; // 2^a
			//num_weights = 31; // 2^b
			max_weight_limit = 127;
			min_weight_limit = -128;
			weight_table = (int**) malloc(sizeof(int*) * table_length_perceptron);
			for(int i=0; i<table_length_perceptron; i++)
				weight_table[i] = (int*) calloc(num_weights, sizeof(int));
			bias_table = (int*) calloc(table_length_perceptron, sizeof(int));
			
			//Set training threshold as @theta
			//theta = 32;
			break;
    default:
      break;
  }
}

//Calculate the index of the perceptron table by using the last @table_length_perceptron bits of @pc
int calculate_weight_index(uint32_t pc)
{
	return pc & (table_length_perceptron - 1);
}

//Calculate the perceptron sum of a perceptron indexed by pc
int calculate_perceptron_sum(uint32_t pc)
{
	int weight_index = calculate_weight_index(pc);
	int sum = bias_table[weight_index];
	for(int i=0; i<num_weights; i++)
		sum = ((g_history>>i)&1) ? sum + weight_table[weight_index][i] : sum - weight_table[weight_index][i];
	return sum;
}

//Limit the weight values between -128 and 127 so that a max 8 bits are used for weight
void set_weight_limit(int *weight_value)
{
	if(*weight_value > max_weight_limit){
    *weight_value = *weight_value - 1;
  }
	else if(*weight_value < min_weight_limit) {
    *weight_value = *weight_value + 1;
  }
}

//Return TAKEN if the perceptron sum >=0, otherwise return NOTTAKEN
int make_perceptron_prediction(uint32_t pc)
{
	return (calculate_perceptron_sum(pc) >= 0) ? TAKEN : NOTTAKEN;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      g_BHT_index = (pc ^ g_history) & ((1 << ghistoryBits) - 1);
      uint8_t predict = g_BHT[g_BHT_index];
      if (predict == WN || predict == SN){
        g_result = NOTTAKEN;
      }
      else{g_result = TAKEN;}
      
      return g_result;   
    case TOURNAMENT:
      PHT_index = pc & ((1 << pcIndexBits) - 1);
      uint32_t l_BHTindex = l_PHT[PHT_index];
      uint8_t local_Prediction = l_BHT[l_BHTindex];
      if(local_Prediction == WN || local_Prediction == SN)
      {
        l_outcome = NOTTAKEN;
      }
      else{l_outcome = TAKEN;}
      global_BHT_index = g_history & ((1 << ghistoryBits) - 1);
      uint8_t global_Prediction = global_BHT[global_BHT_index];
      if(global_Prediction == WN || global_Prediction == SN)
      {
        global_outcome = NOTTAKEN;
      }
      else{global_outcome = TAKEN;}

      // selection of predictor
      //elector[globalBHTindex] == 00 / 01--global predictor; 10 / 11--local predictor
      uint8_t res = selector[global_BHT_index];
      if (res == WN || res == SN) {
        return global_outcome;
      } 
      else {return l_outcome;}
    case CUSTOM:
      return make_perceptron_prediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void gshare_train(uint32_t pc, uint8_t outcome) {
    int BHTindex = (pc ^ g_history) & ((1 << ghistoryBits) - 1);
    if (outcome == TAKEN) {
        if (g_BHT[BHTindex] != ST)
            g_BHT[BHTindex]++;
    } else {
        if (g_BHT[BHTindex] != SN)
            g_BHT[BHTindex]--;
    }
}
  
void tournament_shift(uint32_t pc, uint8_t outcome) {
    PHT_index = pc & ((1 << pcIndexBits) - 1);
    uint32_t local_BHT_index = l_PHT[PHT_index];
    if (outcome == TAKEN) {
        if (l_BHT[local_BHT_index] != ST)
            l_BHT[local_BHT_index]++;
    } else {
        if (l_BHT[local_BHT_index] != SN)
            l_BHT[local_BHT_index]--;
    }

    int global_BHT_index = g_history & ((1 << ghistoryBits) - 1);
    if (outcome == TAKEN) {
        if (global_BHT[global_BHT_index] != ST)
            global_BHT[global_BHT_index]++;
    } else {
        if (global_BHT[global_BHT_index] != SN)
            global_BHT[global_BHT_index]--;
    }
}

void tournament_train(uint32_t pc, uint8_t outcome) {
    if (l_outcome != global_outcome) {
        global_BHT_index = g_history & ((1 << ghistoryBits) - 1);
        if (l_outcome == outcome)
        {
          if (selector[global_BHT_index] != ST)
            selector[global_BHT_index] += 1;
        } 
        else if (l_outcome != outcome)
        {
          if (selector[global_BHT_index] != SN)
            selector[global_BHT_index] -= 1;
        }
    }
    tournament_shift(pc, outcome);
    int PHT_index = pc & ((1 << pcIndexBits) - 1);
    l_PHT[PHT_index] <<= 1;
    l_PHT[PHT_index] &= ((1 << lhistoryBits) - 1);
    l_PHT[PHT_index] |= outcome;
    g_history <<= 1;
    g_history  &= ((1 << ghistoryBits) - 1);
    g_history |= outcome;
}

//Train the perceptron predictor
void train_perceptron(uint32_t pc, uint8_t outcome)
{
	int weight_index = calculate_weight_index(pc);
	int perceptron_sum = calculate_perceptron_sum(pc);
	int perceptron_outcome = perceptron_sum>=0;
	
	//Train predictor only if prediction != outcome or perceptron sum is less than threshold 
	if (abs(perceptron_sum) <= theta || perceptron_outcome != outcome) 
	{
		//Update bias table entries
		bias_table[weight_index] += (outcome==TAKEN) ? 1 : -1; 
		set_weight_limit(&bias_table[weight_index]);
    	
    	//Update perceptron table entries 
    	for (int i=0; i<num_weights; i++) 
    	{
    		weight_table[weight_index][i] += (((g_history>>i) & 1)==outcome) ? 1 : -1;
    		set_weight_limit(&weight_table[weight_index][i]);
    	}
  	}
  	g_history = (g_history<<1 | outcome) & ((1<<num_weights) - 1);
}

    
  

void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      gshare_train(pc, outcome);
      g_history <<= 1;
      g_history  &= ((1 << ghistoryBits) - 1);
      g_history |= outcome;
      break;
    case TOURNAMENT:
      tournament_train(pc, outcome);
      break;
    case CUSTOM:
      train_perceptron(pc, outcome);
      break;
    default:
      break;
    }
}
