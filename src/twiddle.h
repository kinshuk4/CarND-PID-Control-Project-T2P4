#ifndef TWIDDLER_H
#define TWIDDLER_H
#include <iostream>
#include <math.h>
#include "PID.h"
using namespace std;

class Twiddler {
public:
    /*
    * PID
    */
    PID pid;
    double cte;

    /*
    * Constructor
    */
    Twiddler();

    /*
    * Destructor.
    */
    virtual ~Twiddler();

    /*
    * Initialize Twiddler.
    */
    void Init(PID pid, double cte);

    /*
    * Twiddle the PID error variables given cross track error.
    */
    void twiddle(double* p, double tolerance);

};

#endif /* TWIDDLER_H */
