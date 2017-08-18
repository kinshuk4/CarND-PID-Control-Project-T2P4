## Introduction

The purpose of the project is to implement a PID controller in C++ to maneuver the vehicle around the track. 

The simulator will provide you the cross track error (CTE) and the velocity (mph) in order to compute the appropriate steering angle.

The rubric of the project can be found [here](https://review.udacity.com/#!/rubrics/824/view).

## Rubric Reflection

### *Describe the effect each of the P, I, D components had in your implementation.*

#### The Hyperparameters

- **P or Proportional** controls how fast the syesm reacts to error and has most direct effect as the name suggests. If P is high, it is good for varying track, but oscillations may increase due to added noise.
- **I or Integral** helps in recovery from deviation and counter various bias (for eg. when car gets driving near sides instead of center) . It can increase oscillations due to the fact that its sum of CTEs.
- **D or Differential** smoothens driving by counteracting P components tendency to overshoot the center line.  If D is high, it can make car very rigid , specially for bigger turns.

### *Describe how the final hyperparameters were chosen.*

#### Implementation of P controller for Steering

First I tried P controller and the car oscillates and leaves the track. [Here](https://www.youtube.com/watch?v=_SkXMnki_wQ) is the video for it.

#### Implementation of PD controller for Steering

Then, I implemented PD controller and that worked but the oscillations were lot. So, to fix it I had to add I component. [Here](https://www.youtube.com/watch?v=mgYyUyZ0oeU) is the video for it.

#### Implementation of PID controller for Steering

To fix the oscillations, the "I" component worked okayish. [Here](https://www.youtube.com/watch?v=9ExqtY2d-O0) is the final video for it. 

#### PD Controller for Throttle

I also added the PD controller for throttle. I checked that the effect of I was not visible. 

Also, I updated the throttle based on steering angle as well. When the steering angle is high, that means there is a sharp turn, so at that time we will like to give less thrust, and hence lower throttle of 0.07. Likewise I created a method  `modulate_throttle_by_steering_discrete` which returns a throttle based on range. Later, I am taking the throttle which is minimum from the pid controller and this method.

Here are the chosen values:

| Coefficient | Steering Value | Throttle Value |
| ----------- | -------------- | -------------- |
| Kp          | 0.15           | 0.2            |
| Ki          | 0.0002         | 0.0            |
| Kd          | 2.0            | 0.02           |

Hyperparameters were tuned manually to begin with as the car was leaving the track.  Once I had some idea of the parameters, I tried implementing the twiddle from Sebastian's [lecture note](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/1397890f-71c5-4d83-aae7-0a031eedd1f2/concepts/87d07dd3-70f3-46a0-b35c-c8528792d90f), using the tolerance, pid and cte. I added the code in Twiddler class.

## Summary

The car looks stable at the lower speeds and I can only get maximum of around 50MPH. I feel further tweaking can also be done with the parameters.