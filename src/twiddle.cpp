#include "twiddle.h"
#include <cmath>

Twiddler::Twiddler() {}

Twiddler::~Twiddler() {}

void Twiddler::Init(PID pid, double cte) {
    this->pid = pid;
    this->cte = cte;
}

const int length_p2 = 3;

void Twiddler::twiddle(double *p, double tol = 0.0001) {
    // Twiddle variable declarations
    double dp[3] = {1.0, 1.0, 1.0};
    double tw_err = 0.0;
    double sum = dp[0] + dp[1] + dp[2];
    double best_err = fabs(pid.TotalError()); // Take the absolute value of error

    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = 0.0;

    pid.Init(p[0], p[1], p[2]);

    int i = 0;

    while (sum > tol) {
        cout << "Iteration " << i << "Best error" << best_err;
        for (int j = 0; j < length_p2; j++) {

            // Twiddle Up
            p[j] += dp[j];
            pid.setTaus(p);
            pid.UpdateError(cte);
            tw_err = fabs(pid.TotalError()); // Take the absolute value of error

            if (tw_err < best_err) {
                best_err = tw_err;
                dp[j] *= 1.1;
            } else {
                // Twiddle Down
                p[j] -= 2 * dp[j];
                pid.setTaus(p);
                pid.UpdateError(cte);
                tw_err = fabs(pid.TotalError()); // Take the absolute value of error

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


