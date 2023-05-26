#include "phaseinvertermodifier.h"

PhaseInverterModifier::PhaseInverterModifier()
{

}

void PhaseInverterModifier::modify(StereoStimSignalSample &sample)
{
    sample.setSecondaryPhase(sample.getSecondaryPhase() + 0.5);
}
