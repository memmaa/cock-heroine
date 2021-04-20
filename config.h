#ifndef CONFIG_H
#define CONFIG_H

#include "beatanalysis.h"

#define SEMIQUAVER (1.0/4.0)
#define QUAVER_TRIPLET (1.0/3.0)
#define QUAVER (1.0/2.0)
#define CROCHET_TRIPLET (2.0/3.0)
#define THREE_SEMIQUAVERS (3.0/4.0)
#define CROCHET 1
#define FIVE_SEMIQUAVERS (5.0/4.0)
#define TWO_CROCHET_TRIPLETS (4.0/3.0)
#define THREE_QUAVERS (3.0/2.0)
#define TWO_BEATS 2
#define THREE_BEATS 3
#define FOUR_BEATS 4
#define FIVE_BEATS 5
#define SIX_BEATS 6
#define SEVEN_BEATS 7
#define EIGHT_BEATS 8
//replaced by 'break length detection':
//#define TWELVE_BEATS 12
//#define SIXTEEN_BEATS 16
//#define TWENTY_FOUR_BEATS 24
//#define THIRTY_TWO_BEATS 32

#ifdef CH_GROOVE
#define SEVEN_SEMIQUAVERS (7.0/4.0)
#define NINE_SEMIQUAVERS (9.0/4.0)
#define SEVENTEEN_SEMIQUAVERS (17.0/4.0)
#endif



#endif // CONFIG_H
