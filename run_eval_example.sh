#!/bin/bash


./build/TORS --mode "EVAL" \
    --path_location "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed" \
    --path_scenario "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed/scenario.json" \
    --path_plan "./data/Demo/TUSS-Instance-Generator/kleine_brinkhorst_v2/Fixed/plan.json" \
    --plan_type "Solver"