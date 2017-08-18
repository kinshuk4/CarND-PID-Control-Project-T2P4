#include "PID.h"

using namespace std;

/*
* DONE: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    this->p_error = this->d_error = this->i_error = 0.0;
}

void PID::setTaus(double *p) {
    this->Kp = p[0];
    this->Ki = p[1];
    this->Kd = p[2];
}

void PID::UpdateError(double cte) {
    // p_error is previous cte
    d_error = cte - p_error;
    p_error = cte;
    i_error += cte;
}

double PID::TotalError() {
    return -1 * (Kp * p_error + Ki * i_error + Kd * d_error);
}




