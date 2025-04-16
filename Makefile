ROOT_DIR := $(abspath .)

DATA_0 = "facebook"
DATA_1 = "rmat_17_24"
DATA_2 = "rmat_20_24"
DATA_3 = "v_10_e_26"
DATA_4 = "v_82167_e_948463"

.PHONY: pre_data_0
pre_data_0:
	${ROOT_DIR}/tools/delta_pr/delta_pr 4039 176468 ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_0}/debug_res.txt 1
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_0} 2 4039 176468 1

.PHONY: sim_data_0
sim_data_0:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_0}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_0}/ori/edge.txt 11 4039 176468 1 ${ROOT_DIR}/acc/data/${DATA_0}/debug_res.txt

.PHONY: pre_data_1
pre_data_1:
	${ROOT_DIR}/tools/delta_pr/delta_pr 131072 23748584 ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_1}/debug_res.txt 0
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_1} 2 131072 23748584 0

.PHONY: sim_data_1
sim_data_1:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_1}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_1}/ori/edge.txt 10 131072 23748584 0 ${ROOT_DIR}/acc/data/${DATA_1}/debug_res.txt

.PHONY: pre_data_2
pre_data_2:
	${ROOT_DIR}/tools/delta_pr/delta_pr 1048576 31399374 ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_2}/debug_res.txt 0
	${ROOT_DIR}/tools/k_channel_trans/k_channel_trans ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt ${ROOT_DIR}/acc/data/${DATA_2} 2 1048576 31399374 0

.PHONY: sim_data_2
sim_data_2:
	${ROOT_DIR}/simulator/build/acc_sim ${ROOT_DIR}/acc/data/${DATA_2}/ori/off.txt ${ROOT_DIR}/acc/data/${DATA_2}/ori/edge.txt 19 1048576 31399374 0 ${ROOT_DIR}/acc/data/${DATA_2}/debug_res.txt

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