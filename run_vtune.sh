#!/usr/bin/env bash

function measure_vtune() {
  CXXFLAGS=$1
  VARIANT=$2
  INPUT=$3

  make clean
  make ${CXXFLAGS}

  for algo in pull push; 
  do 
    amplxe-cl -collect-with runsa -knob event-config=CPU_CLK_UNHALTED.REF_P:sa=133000,CPU_CLK_UNHALTED.THREAD:sa=2000000,FP_COMP_OPS_EXE.X87:sa=2000000,FP_COMP_OPS_EXE.MMX:sa=2000000,FP_COMP_OPS_EXE.SSE_FP:sa=2000000,FP_COMP_OPS_EXE.SSE2_INTEGER:sa=2000000,INST_RETIRED.ANY:sa=2000000,MEM_INST_RETIRED.LOADS:sa=2000000,MEM_INST_RETIRED.STORES:sa=2000000,MEM_LOAD_RETIRED.L1D_HIT:sa=2000000,MEM_LOAD_RETIRED.L2_HIT:sa=200000,MEM_LOAD_RETIRED.LLC_UNSHARED_HIT:sa=200000,MEM_LOAD_RETIRED.LLC_MISS:sa=200000,MEM_LOAD_RETIRED.DTLB_MISS:sa=200000,BR_MISP_EXEC.ANY:sa=20000,BR_INST_EXEC.ANY:sa=20000 -start-paused -analyze-system -app-working-dir . -- ./pagerank inputs/${INPUT} ${algo} 1.0e-20 | tee pr_${VARIANT}_${algo}_${INPUT}.txt;
  done
}


for f in `ls inputs/`
do
  measure_vtune "" "aos" ${f}
  measure_vtune "CXXFLAGS=-DSP2018_CS377P_STRUCT_OF_ARRAYS" "soa" ${f}
done
