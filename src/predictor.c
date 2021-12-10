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
//
// TODO:Student Information
//


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
int number_per = 128;
int addr_per;
int i = 0;
int per_g_bits = 15;
int per_pc_bits = 7;
int8_t theta = (int8_t)((1.93*15)+7);
int8_t* bias;
int8_t** weights;
int per_value;


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
      bias = malloc(number_per * sizeof(int8_t));
      memset(bias, (int8_t)1, number_per * sizeof(int8_t));
      weights = malloc(number_per * sizeof(int8_t*));
      for(i = 0; i < number_per; i++)
      {
        weights[i] = malloc(per_g_bits * sizeof(int8_t));
	memset(weights[i], (int8_t)0, per_g_bits * sizeof(int8_t));
      }
      break;
    default:
      break;
  }
}

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
      addr_per = pc & ((1 << per_pc_bits) - 1);
      per_value = bias[addr_per];
      g_history &= ((1 << per_g_bits) - 1);
      for(i = 0; i < per_g_bits; i++)
      {
        if(((g_history >> i) & 1) == 0)
        {
          per_value -= weights[addr_per][i];
        }
        else
        {
          per_value += weights[addr_per][i];
        }
      }
      if (per_value >= 0) return TAKEN;
      else {return NOTTAKEN;}
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

void per_train(uint32_t pc, uint8_t outcome)
{
	per_value = abs(per_value);
	int sign;
	if(per_value >= 0) sign = 1;
	else{sign = -1;}
	if((sign != (outcome ? 1 : -1)) || (per_value <= theta))
	{
		if((outcome == 1) && (bias[addr_per] < 127))		
		  bias[addr_per] = bias[addr_per] + 1;
		else if((outcome == 0) && (bias[addr_per] > -127))
		  bias[addr_per] = bias[addr_per] - 1;
		for(i = 0; i < per_g_bits; i++)
		{
		  if(outcome == ((g_history >> i) & 1))
		  {
		    if(weights[addr_per][i] < 127)
		    {
		      weights[addr_per][i] += 1;
		    }
		  }
		    
		  else if(outcome != (((g_history & (1 << i)) >> i) & 1))
	          {
		    if(weights[addr_per][i] > -127)
		    {
		      weights[addr_per][i] -= 1;
		    }
		  }
		}
	}
	g_history = (((g_history << 1) | (outcome)) & ((1 << per_g_bits) - 1));
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
      per_train(pc, outcome);
      break;
    default:
      break;
    }
}
