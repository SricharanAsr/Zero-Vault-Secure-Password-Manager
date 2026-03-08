#ifndef RULE_H
#define RULE_H

#include "decision.h"
#include "input/validate.h"

typedef RiskDecision (*RiskRuleFn)(const RiskInput*);

#endif
