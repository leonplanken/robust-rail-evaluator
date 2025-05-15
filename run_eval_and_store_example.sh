#!/bin/bash


./build/TORS --mode "EVAL_AND_STORE" \
    --path_location "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed" \
    --path_scenario "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed/scenario.json" \
    --path_plan "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed/plan.json" \
    --path_eval_result "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed/evaluation_results.txt" \
    --plan_type "Solver"