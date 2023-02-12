#include "phaseinvertermodifier.h"

PhaseInverterModifier::PhaseInverterModifier()
{

}

void PhaseInverterModifier::modify(StimSignalSample &sample)
{
    sample.setSecondaryPhase(sample.getSecondaryPhase() + 0.5);
}
