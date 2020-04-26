#ifndef DISTOR_H
#define DISTOR_H

#include <vector>
#include <string>
#include "effect.h"

namespace upc {
  class distor: public upc::Effect {
    private:
      float fase, inc_fase;
	  float	fm, A_MAX;
    public:
      distor(const std::string &param = "");
	  void operator()(std::vector<float> &x);
	  void command(unsigned int);
  };
}

#endif
