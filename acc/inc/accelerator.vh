/*********************************************
* do not change
**********************************************/
`define HBM_AWIDTH      64
`define HBM_DWIDTH      512
`define HBM_EDGE_MASK   16
`define CACHELINE_LEN   16
`define CACHELINE_LEN_WIDTH 4
`define BITMAP_COMPRESSED_LENGTH    32
`define BITMAP_COMPRESSED_LENGTH_WIDTH  5

/*********************************************
* change vtx_num and edge_num to check different graph
*********************************************/
`define MAX_VTX_NUM     1048576
`define MAX_VTX_NUM_WIDTH 20
`define VTX_NUM         262144         // 1048576
`define VTX_NUM_WIDTH   18  // 20
`define EDGE_NUM        31400738
`define EDGE_NUM_WIDTH  25

/*********************************************
* change core_num and channel_num and bitmap_compressed_num to do simulation
**********************************************/
`define CORE_NUM        32
`define CORE_NUM_WIDTH  5
`define PSEUDO_CHANNEL_NUM  2
`define BITMAP_COMPRESSED_NUM       1024
`define BITMAP_COMPRESSED_NUM_WIDTH 10

/*************************************************
* do not change
*************************************************/
`define URAM_DELAY          3
`define BRAM_DELAY          2
`define WAIT_END_DELAY      20
`define DAMPING             32'h3f59999a
`define ONE_OVER_N          32'h36ffff80
`define E1                  32'h33d6df95
`define E2                  32'h3c23d70a
`define ADDED_CONST         32'h3599994d

`define ITERATION_WIDTH     4 // 注意最高只能支持31轮迭代
`define V_ID_WIDTH          `MAX_VTX_NUM_WIDTH
`define V_VALUE_WIDTH       32 // half float
`define V_OFF_AWIDTH        (`MAX_VTX_NUM_WIDTH - `CORE_NUM_WIDTH )
`define V_OFF_DWIDTH        32

`define GROUP_CORE_NUM          16
`define GROUP_CORE_NUM_WIDTH    4
`define HBM_DWIDTH_PER_CORE     (`HBM_DWIDTH / `GROUP_CORE_NUM)

`define DELTA_BRAM_AWIDTH       `V_OFF_AWIDTH
`define DELTA_BRAM_DWIDTH       (`V_VALUE_WIDTH + `ITERATION_WIDTH)
`define PR_URAM_AWIDTH          `V_OFF_AWIDTH
`define PR_URAM_DWIDTH          32

`define HPX_ROW_NUM    `PSEUDO_CHANNEL_NUM // row
`define HPX_ROW_NUM_WIDTH 1
`define HPX_COLUMN_NUM  `GROUP_CORE_NUM // col
`define HPX_COLUMN_NUM_WIDTH 4

`define HORIZONTAL_NUM `GROUP_CORE_NUM
`define HORIZONTAL_NUM_WIDTH 4
`define VERTICAL_NUM `PSEUDO_CHANNEL_NUM
`define VERTICAL_NUM_WIDTH 1

`define FIFO_SIZE           16
`define FIFO_SIZE_WIDTH     4
`define HPX_FIFO_SIZE       16
`define HPX_FIFO_SIZE_WIDTH 4

`define MAX_ITERATION_NUM 14