%% Motor driver
clear
STEP_RES = 1;
STEPS_PR_ROUND = 200*STEP_RES;
RADIUS = 0.005;
DIST_PR_STEP = (2*RADIUS*pi)/STEPS_PR_ROUND;

distance = 0;
distance_to_go = 2;
steps = 0;

%% 1 meter svarer til
1/DIST_PR_STEP;

%% 2 runder svarer til
2*STEPS_PR_ROUND*DIST_PR_STEP

%% Tidsberegning

time_to_go = 2*60*60  %% [s] The time to do the dist
steps_to_go = distance_to_go/DIST_PR_STEP
time_pr_step = time_to_go/steps_to_go
time_pr_step_ms = time_pr_step*10^3

