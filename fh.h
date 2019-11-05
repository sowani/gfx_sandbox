#ifndef FH_H_
#define FH_H_

#include <stdint.h>
#include "cb.h"

bool handleComponentFocus(KeypadKey key, ComponentBase* componentStack[], uint8_t stackSize);
int findClosestChild(KeypadKey key, ComponentBase* componentStack[], uint8_t stackSize);
uint16_t calculateDistance(ComponentBase *comp1, ComponentBase* comp2);
int getComponentIndexInFocus(ComponentBase* componentStack[], uint8_t stackSize);
ComponentBase* getComponentInFocus(ComponentBase* componentStack[], uint8_t stackSize);
void dumpComponentStack(ComponentBase* componentStack[], uint8_t stackSize);

#endif /* FH_H_ */
