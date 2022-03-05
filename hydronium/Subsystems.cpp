#include "Subsystems.h"

template<typename P>
void SubsystemBase<P>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}

