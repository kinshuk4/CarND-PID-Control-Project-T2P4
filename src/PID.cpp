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


void PID::twiddle(double cte, double tol) {
    // Twiddle variable declarations
    double p[3] = {0.0, 0.0, 0.0};
    double dp[3] = {1.0, 1.0, 1.0};
    double tw_err = 0.0;
    double sum = dp[0] + dp[1] + dp[2];
    double best_err = fabs(this->TotalError());


    this->Init(p[0], p[1], p[2]);

    int i = 0;

    while (sum > tol) {
        cout << "Iteration " << i << "Best error" << best_err;
        for (int j = 0; j < length_p; j++) {
            // Twiddle Up
            p[j] += dp[j];
            this->setTaus(p);
            this->UpdateError(cte);
            tw_err = fabs(this->TotalError());

            if (tw_err < best_err) {
                best_err = tw_err;
                dp[j] *= 1.1;
            } else {
                // Twiddle Down
                p[j] -= 2 * dp[j];
                this->setTaus(p);
                this->UpdateError(cte);
                tw_err = fabs(this->TotalError()); // Take the absolute value of error

                if (tw_err < best_err) {
                    best_err = tw_err;
                    dp[j] *= 1.1;
                } else {
                    p[j] += dp[j];
                    dp[j] *= 0.9;
                }
            }
        }
        sum = dp[0] + dp[1] + dp[2];
        i += 1;
    }
}

