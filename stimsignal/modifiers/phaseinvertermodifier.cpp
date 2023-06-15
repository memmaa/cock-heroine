#include "phaseinvertermodifier.h"

PhaseInverterModifier::PhaseInverterModifier()
{

}

void PhaseInverterModifier::modify(StimSignalSample &sample)
{
    sample.setPhase(1, sample.getPhase(1) + 0.5);
}
