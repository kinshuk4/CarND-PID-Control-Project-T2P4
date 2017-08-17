#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include "twiddle.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }

double deg2rad(double x) { return x * pi() / 180; }

double rad2deg(double x) { return x * 180 / pi(); }

const double max_steering = 1.0;
const double target_speed = 0.75;
double p[3] = {0.0, 0.0, 0.0};
//Modify the twiddle flag to enable or disable twiddle
bool twiddle_flag = false;

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
    auto found_null = s.find("null");
    auto b1 = s.find_first_of("[");
    auto b2 = s.find_last_of("]");
    if (found_null != std::string::npos) {
        return "";
    } else if (b1 != std::string::npos && b2 != std::string::npos) {
        return s.substr(b1, b2 - b1 + 1);
    }
    return "";
}


double modulate_throttle_by_steering_discrete(double steering_value) {
    double absolute_steering_angle = fabs(steering_value);
    double throttle_value = 0.0;
    if (absolute_steering_angle >= 0.9) {
        throttle_value = 0.07;
    } else if (absolute_steering_angle >= 0.8) {
        throttle_value = 0.1;
    } else if (absolute_steering_angle >= 0.6) {
        throttle_value = 0.2;
    } else if (absolute_steering_angle >= 0.4) {
        throttle_value = 0.3;
    } else if (absolute_steering_angle >= 0.2) {
        throttle_value = 0.5;
    } else {
        throttle_value = target_speed;
    }
    return throttle_value;
}

double fix_steering(double current_steer) {

    if (current_steer > max_steering) {
        current_steer = max_steering;
    }
    if (current_steer < -max_steering) {
        current_steer = -max_steering;
    }

    return current_steer;
}

int main() {
    uWS::Hub h;
    p[0] = 0.3; // Kp
    p[1] = 0.001; // Ki
    p[2] = 4.0; // Kd
    PID pid_steer, pid_throttle;
    // DONE: Initialize the pid variable.
    pid_steer.Init(0.3, 0.001, 3.0);
    pid_throttle.Init(0.3, 0.0000, 0.02);

    h.onMessage(
            [&pid_steer, &pid_throttle](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
                // "42" at the start of the message means there's a websocket message event.
                // The 4 signifies a websocket message
                // The 2 signifies a websocket event
                if (length && length > 2 && data[0] == '4' && data[1] == '2') {
                    auto s = hasData(std::string(data).substr(0, length));
                    if (s != "") {
                        auto j = json::parse(s);
                        std::string event = j[0].get<std::string>();
                        if (event == "telemetry") {
                            // j[1] is the data JSON object
                            double cte = std::stod(j[1]["cte"].get<std::string>());
                            double speed = std::stod(j[1]["speed"].get<std::string>());
                            double angle = std::stod(j[1]["steering_angle"].get<std::string>());
                            double steer_value, throttle_value;
                            /*
                            * DONE: Calcuate steering value here, remember the steering value is
                            * [-1, 1].
                            * NOTE: Feel free to play around with the throttle and speed. Maybe use
                            * another PID controller to control the speed!
                            */
                            pid_steer.UpdateError(cte);
                            if (twiddle_flag) {
                                Twiddler twiddler;
                                twiddler.Init(pid_steer, cte);
                                twiddler.twiddle(p, 0.0001);
                            }
                            steer_value = fix_steering(pid_steer.TotalError());

                            pid_throttle.UpdateError(fabs(cte));
                            if (twiddle_flag) {
                                Twiddler twiddler;
                                twiddler.Init(pid_throttle, cte);
                                twiddler.twiddle(p, 0.0001);
                            }
                            throttle_value = fmin(target_speed + pid_throttle.TotalError(),
                                                  modulate_throttle_by_steering_discrete(steer_value));

                            // DEBUG
                            std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

                            json msgJson;
                            msgJson["steering_angle"] = steer_value;
                            msgJson["throttle"] = throttle_value;
                            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                            std::cout << msg << std::endl;
                            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                        }
                    } else {
                        // Manual driving
                        std::string msg = "42[\"manual\",{}]";
                        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                    }
                }
            });

    // We don't need this since we're not using HTTP but if it's removed the program
    // doesn't compile :-(
    h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
        const std::string s = "<h1>Hello world!</h1>";
        if (req.getUrl().valueLength == 1) {
            res->end(s.data(), s.length());
        } else {
            // i guess this should be done more gracefully?
            res->end(nullptr, 0);
        }
    });

    h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
        std::cout << "Connected!!!" << std::endl;
    });

    h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
        ws.close();
        std::cout << "Disconnected" << std::endl;
    });

    int port = 4567;
    if (h.listen(port)) {
        std::cout << "Listening to port " << port << std::endl;
    } else {
        std::cerr << "Failed to listen to port" << std::endl;
        return -1;
    }
    h.run();
}
