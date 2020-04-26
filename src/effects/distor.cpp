#include <iostream>
#include <math.h>
#include "distor.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

static float SamplingRate = 44100;

distor::distor(const std::string &param) {
  fase = 0;

  KeyValue kv(param);

  if (!kv.to_float("A_MAX", A_MAX))
    A_MAX = 0.5; //default value

  if (!kv.to_float("fm", fm))
    fm = 10; //default value

  inc_fase = 2 * M_PI * fm / SamplingRate;
}

void distor::command(unsigned int comm) {
  if (comm == 1) fase = 0;
}

void distor::operator()(std::vector<float> &x){
  for (unsigned int i = 0; i < x.size(); i++) {
      x[i] = 2*x[i];
      if (x[i] > A_MAX){
         x[i] = A_MAX;
      } else if (x[i] < -A_MAX){
          x[i] = -A_MAX;
      }
  }
}
