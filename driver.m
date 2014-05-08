%% Motor driver
clear
STEP_RES = 16;
STEPS_PR_ROUND = 200*STEP_RES;
RADIUS = 0.0272; %[m]
DIST_PR_STEP = (2*RADIUS*pi)/STEPS_PR_ROUND;

distance = 0;
distance_to_go = 2;
steps_to_go = distance_to_go/DIST_PR_STEP
steps = 0;

max_dist = (2^32)-1*DIST_PR_STEP
min_dist = DIST_PR_STEP

%% Omvendt beregning
steps_to_go = 20051
distance_to_go = 1.07
DIST_PR_STEP = distance_to_go/steps_to_go
RADIUS = (1/2)*(DIST_PR_STEP*STEPS_PR_ROUND)/pi



%% 1 meter svarer til
1/DIST_PR_STEP;

%% 2 runder svarer til
2*STEPS_PR_ROUND*DIST_PR_STEP

%% Tidsberegning

time_to_go = 600  %% [s] The time to do the dist
steps_to_go = distance_to_go/DIST_PR_STEP
time_pr_step = time_to_go/steps_to_go
time_pr_step_ms = time_pr_step*10^3


total_time=(time_pr_step*steps_to_go)-(0.003*steps_to_go)

%%
m = 10
s = m*60
ms = s*10^3
