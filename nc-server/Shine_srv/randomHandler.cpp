/** @file randomHandler.cpp
 *  \brief A file to generate random elements
 *    */
#include "randomHandler.h"
#include "randomUniform.h"

float randomNumber(float minNumber, float maxNumber){
    long seed = 1;
    float randNum = randomUniformFunction(&seed);

    float result = (randNum * (maxNumber - minNumber)) + minNumber;

    return result;
}
