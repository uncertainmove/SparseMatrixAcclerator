ROOT_DIR := $(abspath .)
FPGA_TYPE = "u55c"

DATA_0 = "facebook"
DATA_1 = "rmat_17_24"
DATA_2 = "rmat_20_24"
DATA_3 = "v_10_e_26"
DATA_4 = "v_82167_e_948463"
DATA_5 = "rmat_16_20"

.PHONY: pre_data_0
pre_data_0:
	${ROOT_DIR}/tools/delta_pr/delta_pr 4039 176468 ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_0}/debug_res.txt 1
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_0} 2 4039 176468 1

.PHONY: sim_data_0
sim_data_0:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt 11 4039 176468 1 ${ROOT_DIR}/acc/data/${DATA_0}/debug_res.txt

.PHONY: run_data_0
run_data_0:
	${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/delta_pr_accelerator ${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/binary_container_1.xclbin ${ROOT_DIR}/acc/data/${DATA_0}/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/edge_0.txt ${ROOT_DIR}/acc/data/${DATA_0}/edge_1.txt ${ROOT_DIR}/acc/data/${DATA_0}/debug_res.txt ${ROOT_DIR}/acc/data/${DATA_0}/res.txt 4039 89116 87352 10

.PHONY: pre_data_1
pre_data_1:
	${ROOT_DIR}/tools/delta_pr/delta_pr 131072 23748584 ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_1}/debug_res.txt 0
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_1} 2 131072 23748584 0

.PHONY: sim_data_1
sim_data_1:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt 10 131072 23748584 0 ${ROOT_DIR}/acc/data/${DATA_1}/debug_res.txt

.PHONY: run_data_1
run_data_1:
	${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/delta_pr_accelerator ${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/binary_container_1.xclbin ${ROOT_DIR}/acc/data/${DATA_1}/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/edge_0.txt ${ROOT_DIR}/acc/data/${DATA_1}/edge_1.txt ${ROOT_DIR}/acc/data/${DATA_1}/debug_res.txt ${ROOT_DIR}/acc/data/${DATA_1}/res.txt 131072 11898017 11850567 10

.PHONY: pre_data_2
pre_data_2:
	${ROOT_DIR}/tools/delta_pr/delta_pr 1048576 31399374 ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_2}/debug_res.txt 0
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_2} 2 1048576 31399374 0

.PHONY: sim_data_2
sim_data_2:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt 19 1048576 31399374 0 ${ROOT_DIR}/acc/data/${DATA_2}/debug_res.txt

.PHONY: run_data_2
run_data_2:
	${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/delta_pr_accelerator ${ROOT_DIR}/acc/bitstream/${FPGA_TYPE}/binary_container_1.xclbin ${ROOT_DIR}/acc/data/${DATA_2}/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/edge_0.txt ${ROOT_DIR}/acc/data/${DATA_2}/edge_1.txt ${ROOT_DIR}/acc/data/${DATA_2}/debug_res.txt ${ROOT_DIR}/acc/data/${DATA_2}/res.txt 1048576 15894273 15505101 19

.PHONY: pre_data_3
pre_data_3:
	${ROOT_DIR}/tools/delta_pr/delta_pr 10 26 ${ROOT_DIR}/acc/data/${DATA_3}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_3}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_3}/debug_res.txt 0
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_3}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_3}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_3} 2 10 26 0

.PHONY: sim_data_3
sim_data_3:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_3}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_3}/ori/edge.txt 16 10 26 0 ${ROOT_DIR}/acc/data/${DATA_3}/debug_res.txt

.PHONY: pre_data_4
pre_data_4:
	${ROOT_DIR}/tools/delta_pr/delta_pr 82167 948463 ${ROOT_DIR}/acc/data/${DATA_4}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_4}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_4}/debug_res.txt 1
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_4}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_4}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_4} 2 82167 948463 1

.PHONY: sim_data_4
sim_data_4:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_4}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_4}/ori/edge.txt 14 82167 948463 1 ${ROOT_DIR}/acc/data/${DATA_4}/debug_res.txt

.PHONY: gen_data_5
gen_data_5:
	${ROOT_DIR}/tools/PaRMAT/Release/PaRMAT -nVertices 65536 -nEdges 1048576 -a 0.30 -b 0.30 -c 0.30 -threads 16 -noEdgeToSelf -noDuplicateEdges -undirected -output ${ROOT_DIR}/tools/PaRMAT/Release/out.txt
	${ROOT_DIR}/tools/data_generator/data_generator ${ROOT_DIR}/tools/PaRMAT/Release/out.txt ${ROOT_DIR}/acc/data/${DATA_5}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_5}/ori/edge.txt 65536 1048576 0 0

.PHONY: pre_data_5
pre_data_5:
	${ROOT_DIR}/tools/delta_pr/delta_pr 65536 2162688 ${ROOT_DIR}/acc/data/${DATA_5}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_5}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_5}/debug_res.txt 1
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_5}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_5}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_5} 2 65536 2162688 1

.PHONY: sim_data_5
sim_data_5:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_5}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_5}/ori/edge.txt 11 65536 2162688 1 ${ROOT_DIR}/acc/data/${DATA_5}/debug_res.txt

.PHONY: reset_fpga 
reset_fpga:
	xbutil reset --device 0000:31:00.1

.PHONY: build_all
build_all:
	${MAKE} -C ${ROOT_DIR}/tools/data_generator
	${MAKE} -C ${ROOT_DIR}/tools/delta_pr
	${MAKE} -C ${ROOT_DIR}/tools/generate_trace
	${MAKE} -C ${ROOT_DIR}/tools/k_channel_trans
	${MAKE} -C ${ROOT_DIR}/tools/PaRMAT/Release
	${MAKE} -C ${ROOT_DIR}/tools/pr_check
	mkdir -p ${ROOT_DIR}/simulator/build
	cd ${ROOT_DIR}/simulator/build && cmake ..
	${MAKE} -C ${ROOT_DIR}/simulator/build

.PHONY: clean_all
clean_all:
	-${MAKE} -C ${ROOT_DIR}/tools/data_generator clean
	-${MAKE} -C ${ROOT_DIR}/tools/delta_pr clean
	-${MAKE} -C ${ROOT_DIR}/tools/generate_trace clean
	-${MAKE} -C ${ROOT_DIR}/tools/k_channel_trans clean
	-${MAKE} -C ${ROOT_DIR}/tools/PaRMAT/Release clean
	-${MAKE} -C ${ROOT_DIR}/tools/pr_check clean
	-rm -r ${ROOT_DIR}/simulator/build