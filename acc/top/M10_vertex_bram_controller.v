`timescale 1ns / 1ps

`include "accelerator.vh"

module vtx_ram_controller #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    DELTA_BRAM_DWIDTH = `DELTA_BRAM_DWIDTH,
    PR_URAM_AWIDTH = `PR_URAM_AWIDTH,
    PR_URAM_DWIDTH = `PR_URAM_DWIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    URAM_DELAY = `URAM_DELAY,
    BRAM_DELAY = `BRAM_DELAY
) (
    input                                       clk,
    input [CORE_NUM - 1 : 0]                    rst,
    input [31 : 0]                              vertex_num,
    input                                       initial_ram,
    input                                       transfer_ram,
    input                                       hbm_wr_ready,
    input                                       hbm_wr_trsp, // from hbm
    input [CORE_NUM * V_OFF_AWIDTH - 1 : 0]     rd_addr_src,
    input [CORE_NUM - 1 : 0]                    rd_valid_src,
    input [CORE_NUM * V_OFF_AWIDTH - 1 : 0]     wr_addr_dst,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    wr_v_value_dst,
    input [CORE_NUM - 1 : 0]                    wr_valid_dst,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,

    // to hbm
    output reg [CORE_NUM - 1 : 0]               next_rst,
    output reg                                  initial_done,
    output reg                                  transfer_done,
    output [1024 - 1 : 0]                       hbm_wb_data,
    output [64 - 1 : 0]                         hbm_wb_addr,
    output                                      hbm_wb_valid,
    output [CORE_NUM - 1 : 0]                   stage_full,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   src_recv_update_v_value,
    output [CORE_NUM - 1 : 0]                   src_recv_update_v_valid,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      backend_active_v_id,
    output [CORE_NUM - 1 : 0]                   backend_active_v_updated,
    output [CORE_NUM - 1 : 0]                   backend_active_v_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid,
    output reg [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id
);

    // from ram
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta1_bram_data [0 : CORE_NUM - 1];
    reg [BRAM_DELAY - 1 : 0]            delta1_bram_data_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_uram_data [0 : CORE_NUM - 1];
    reg [URAM_DELAY - 1 : 0]            delta2_uram_data_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_DWIDTH - 1 : 0]       pr_uram_data [0 : CORE_NUM - 1];
    reg [URAM_DELAY - 1 : 0]            pr_uram_data_valid [0 : CORE_NUM - 1];

    // to ram
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta1_bram_rd_addr [0 : CORE_NUM - 1];
    wire                                delta1_bram_rd_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_uram_rd_addr [0 : CORE_NUM - 1];
    wire                                delta2_uram_rd_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_AWIDTH - 1 : 0]       pr_uram_rd_addr [0 : CORE_NUM - 1];
    wire                                pr_uram_rd_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta1_bram_wr_addr [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta1_bram_wr_value [0 : CORE_NUM - 1];
    wire                                delta1_bram_wr_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_uram_wr_addr [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_uram_wr_value [0 : CORE_NUM - 1];
    wire                                delta2_uram_wr_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_AWIDTH - 1 : 0]       pr_uram_wr_addr [0 : CORE_NUM - 1];
    wire [PR_URAM_DWIDTH - 1 : 0]       pr_uram_wr_value [0 : CORE_NUM - 1];
    wire                                pr_uram_wr_valid [0 : CORE_NUM - 1];

    wire [31 : 0] vertex_num_f;
    wire vertex_num_f_valid;

    reg [PR_URAM_AWIDTH - 1 : 0] local_uram_addr;
    reg local_uram_valid;
    reg [64 - 1 : 0] local_hbm_addr;
    reg [PR_URAM_AWIDTH - 1 : 0] initial_uram_addr ;
    reg initial_uram_valid;
    reg [31 : 0] send_ct, rv_ct;
    wire [31 : 0] delta1_initial_v;
    wire delta1_initial_v_valid;
    wire [31 : 0] delta2_initial_v;
    wire delta2_initial_v_valid;
    wire [31 : 0] pr_initial_v;
    wire pr_initial_v_valid;
    
    // 因为全同步，所有采用0号core的信号即可
    assign hbm_wb_valid = transfer_ram && pr_uram_data_valid[0][URAM_DELAY - 1];
    assign hbm_wb_addr = local_hbm_addr;
    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assign hbm_wb_data[(i + 1) * HBM_DWIDTH_PER_CORE - 1 : i * HBM_DWIDTH_PER_CORE] = pr_uram_data[i];
        end
    endgenerate

    always @ (posedge clk) begin
        next_rst <= rst;
        iteration_id <= front_iteration_id;
    end

    // 设置本地的uram传输地址
    always @ (posedge clk) begin
        if (rst) begin
            local_uram_addr <= {PR_URAM_AWIDTH{1'b1}};
            local_uram_valid <= 0;
            send_ct <= 0;
            rv_ct <= 0;
            transfer_done <= 0;
        end else begin
            if (!transfer_ram) begin
                local_uram_addr <= {PR_URAM_AWIDTH{1'b1}};
                local_uram_valid <= 0;
                send_ct <= 0;
                rv_ct <= 0;
                transfer_done <= 0;
            end else begin
                if (hbm_wr_trsp) begin
                    rv_ct <= rv_ct + 1;
                end
                if (!hbm_wr_ready || (local_uram_addr != {PR_URAM_AWIDTH{1'b1}} && local_uram_addr >= (vertex_num >> CORE_NUM_WIDTH))) begin
                    local_uram_valid <= 0;
                    transfer_done <= send_ct == rv_ct;
                end else begin
                    local_uram_addr <= local_uram_addr + 1;
                    local_uram_valid <= 1'b1;
                    send_ct <= send_ct + 1;
                end
            end
        end
    end
    // 设置远端的hbm写入地址
    always @ (posedge clk) begin
        if (rst) begin
            local_hbm_addr <= 64'hffffffffffffff80;
        end else begin
            if (!transfer_ram) begin
                local_hbm_addr <= 64'hffffffffffffff80;
            end else begin
                if (pr_uram_data_valid[0][URAM_DELAY - 2]) begin
                    local_hbm_addr <= local_hbm_addr + 64'h0000000000000080;
                end
            end
        end
    end

    // 初始化地址设置
    always @ (posedge clk) begin
        if(rst) begin
            if (!initial_ram) begin
                initial_uram_addr <= {PR_URAM_AWIDTH{1'b1}};
                initial_uram_valid <= 0;
                initial_done <= 0;
            end else begin
                if ((initial_uram_addr == {PR_URAM_AWIDTH{1'b1}} || initial_uram_addr < ((vertex_num - 1) >> CORE_NUM_WIDTH)) & delta1_initial_v_valid) begin
                    initial_uram_addr <= initial_uram_addr + 1;
                    initial_uram_valid <= 1;
                    initial_done <= 0;
                end else if (initial_uram_addr != {PR_URAM_AWIDTH{1'b1}} && initial_uram_addr >= ((vertex_num - 1) >> CORE_NUM_WIDTH)) begin
                    initial_done <= 1;
                    initial_uram_valid <= 0;
                end else begin
                    initial_uram_valid <= 0;
                end
            end
        end else begin
            initial_uram_addr <= {PR_URAM_AWIDTH{1'b1}};
            initial_uram_valid <= 0;
            initial_done <= 0;
        end
    end
    
    // 通过vertex_num计算delta1, delta2, pr的初始值
    // delta1: 1.0 / vertex_num
    // delta2: -0.85 / vertex_num
    // pr: 0.15 / vertex_num
    // 先将整数类型的vertex_num转换为小数
    int2float INT2FLOAT (
        .aclk                    (clk),
        .s_axis_a_tdata          (vertex_num),
        .s_axis_a_tvalid         (initial_ram),
        .m_axis_result_tdata     (vertex_num_f),
        .m_axis_result_tvalid    (vertex_num_f_valid)
    );
    float_divider DELTA1_DIV (
        .aclk       (clk),
        .s_axis_a_tdata         (32'h3F800000),
        .s_axis_a_tvalid        (vertex_num_f_valid),
        .s_axis_b_tdata         (vertex_num_f),
        .s_axis_b_tvalid        (vertex_num_f_valid),
        .m_axis_result_tdata    (delta1_initial_v),
        .m_axis_result_tvalid   (delta1_initial_v_valid)
    );
    float_divider DELTA2_DIV (
        .aclk       (clk),
        .s_axis_a_tdata         (32'hBF59999A),
        .s_axis_a_tvalid        (vertex_num_f_valid),
        .s_axis_b_tdata         (vertex_num_f),
        .s_axis_b_tvalid        (vertex_num_f_valid),
        .m_axis_result_tdata    (delta2_initial_v),
        .m_axis_result_tvalid   (delta2_initial_v_valid)
    );
    float_divider PR_DIV (
        .aclk       (clk),
        .s_axis_a_tdata         (32'h3E19999A),
        .s_axis_a_tvalid        (vertex_num_f_valid),
        .s_axis_b_tdata         (vertex_num_f),
        .s_axis_b_tvalid        (vertex_num_f_valid),
        .m_axis_result_tdata    (pr_initial_v),
        .m_axis_result_tvalid   (pr_initial_v_valid)
    );

    integer j;
    generate
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            vtx_ram_controller_single#(.CORE_ID(i)) VTX_RAM_CONTROLLER_SINGLE (
                .clk                (clk),
                .rst                (rst[i]),
                .rd_addr_src        (rd_addr_src[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .rd_valid_src       (rd_valid_src[i]),
                .wr_addr_dst        (wr_addr_dst[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .wr_v_value_dst     (wr_v_value_dst[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .wr_valid_dst       (wr_valid_dst[i]),
                .front_iteration_end(front_iteration_end[i]),
                .front_iteration_end_valid  (front_iteration_end_valid[i]),
                .front_iteration_id (front_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),

                .delta1_bram_data               (delta1_bram_data[i]),
                .delta1_bram_data_valid         (delta1_bram_data_valid[i][BRAM_DELAY - 1]),
                .delta2_uram_data               (delta2_uram_data[i]),
                .delta2_uram_data_valid         (delta2_uram_data_valid[i][URAM_DELAY - 1]),
                .pr_uram_data                   (pr_uram_data[i]),
                .pr_uram_data_valid             (pr_uram_data_valid[i][URAM_DELAY - 1] && !transfer_ram),


                .delta1_bram_rd_addr            (delta1_bram_rd_addr[i]),
                .delta1_bram_rd_valid           (delta1_bram_rd_valid[i]),
                .delta2_uram_rd_addr            (delta2_uram_rd_addr[i]),
                .delta2_uram_rd_valid           (delta2_uram_rd_valid[i]),
                .pr_uram_rd_addr                (pr_uram_rd_addr[i]),
                .pr_uram_rd_valid               (pr_uram_rd_valid[i]),
                .delta1_bram_wr_addr            (delta1_bram_wr_addr[i]),
                .delta1_bram_wr_value           (delta1_bram_wr_value[i]),
                .delta1_bram_wr_valid           (delta1_bram_wr_valid[i]),
                .delta2_uram_wr_addr            (delta2_uram_wr_addr[i]),
                .delta2_uram_wr_value           (delta2_uram_wr_value[i]),
                .delta2_uram_wr_valid           (delta2_uram_wr_valid[i]),
                .pr_uram_wr_addr                (pr_uram_wr_addr[i]),
                .pr_uram_wr_value               (pr_uram_wr_value[i]),
                .pr_uram_wr_valid               (pr_uram_wr_valid[i]),

                .stage_full                     (stage_full[i]),
                .src_recv_update_v_value        (src_recv_update_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .src_recv_update_v_valid        (src_recv_update_v_valid[i]),
                .backend_active_v_id            (backend_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .backend_active_v_updated       (backend_active_v_updated[i]),
                .backend_active_v_valid         (backend_active_v_valid[i]),
                .iteration_end                  (iteration_end[i]),
                .iteration_end_valid            (iteration_end_valid[i])
            );

            delta1_bram_ip_single DELTA1_BRAM_IP_SINGLE (
                .clka       (clk),
                .ena        (1'b1),
                .wea        (initial_uram_valid || delta1_bram_wr_valid[i]),
                .addra      (initial_ram ? initial_uram_addr : delta1_bram_wr_addr[i]),
                .dina       (initial_ram ? {delta1_initial_v, 4'b0} : delta1_bram_wr_value[i]),
                .clkb       (clk),
                .enb        (!rst[i] ),
                .addrb      (delta1_bram_rd_addr[i]),
                .doutb      (delta1_bram_data[i])
            );

            delta_uram DELTA2_URAM_IP_SINGLE (
                .clk        (clk),
                .we         (initial_uram_valid || delta2_uram_wr_valid[i]),
                .mem_en     (1'b1),
                .din        (initial_ram ? {delta2_initial_v, 4'b0} : delta2_uram_wr_value[i]),
                .addra      (initial_ram ? initial_uram_addr : delta2_uram_wr_addr[i]),
                .addrb      (delta2_uram_rd_addr[i]),
                .dout       (delta2_uram_data[i])
            );

            pr_uram PR_URAM (
                .clk    (clk),
                .we     (initial_uram_valid || pr_uram_wr_valid[i]),
                .mem_en (1'b1),
                .din    (initial_ram ? pr_initial_v : pr_uram_wr_value[i]),
                .addra  (initial_ram ? initial_uram_addr : pr_uram_wr_addr[i]),
                .addrb  (transfer_ram ? local_uram_addr : pr_uram_rd_addr[i]),
                .dout   ( pr_uram_data[i])
            );

            always @ (posedge clk) begin
                delta1_bram_data_valid[i][0] <= delta1_bram_rd_valid[i];
                delta2_uram_data_valid[i][0] <= delta2_uram_rd_valid[i];
                pr_uram_data_valid[i][0] <= transfer_ram ? local_uram_valid : pr_uram_rd_valid[i];
                for (j = 1; j < BRAM_DELAY; j = j + 1) begin
                    delta1_bram_data_valid[i][j] <= delta1_bram_data_valid[i][j - 1];
                end
                for (j = 1; j < URAM_DELAY; j = j + 1) begin
                    delta2_uram_data_valid[i][j] <= delta2_uram_data_valid[i][j - 1];
                    pr_uram_data_valid[i][j] <= pr_uram_data_valid[i][j - 1];
                end
            end
        end
    endgenerate
    
endmodule

module vtx_ram_controller_single #(parameter
    CORE_ID = 0,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    DELTA_BRAM_DWIDTH = `DELTA_BRAM_DWIDTH,
    PR_URAM_AWIDTH = `PR_URAM_AWIDTH,
    PR_URAM_DWIDTH = `PR_URAM_DWIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    FIFO_SIZE_WIDTH = `FIFO_SIZE_WIDTH,
    CORE_NUM = `CORE_NUM,
    E2 = `E2,
    FLOAT_ADD_DELAY = 19,
    FLOAT_MULTIPLY_DELAY = 6 - 1, // 减1的原因在于寄存器传递过程中多出1cycle时延
    FLOAT_COMPARE_DELAY = 2,
    FLOAT_SUBSTRACT_DELAY = 11
) (
    input                   clk,
    input                   rst,
    input [V_OFF_AWIDTH - 1 : 0]      rd_addr_src,
    input                           rd_valid_src,
    input [V_OFF_AWIDTH - 1 : 0]      wr_addr_dst,
    input [V_VALUE_WIDTH - 1 : 0]   wr_v_value_dst,
    input                           wr_valid_dst,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] front_iteration_id,

    // from ram
    input [DELTA_BRAM_DWIDTH - 1 : 0]   delta1_bram_data,
    input                               delta1_bram_data_valid,
    input [DELTA_BRAM_DWIDTH - 1 : 0]   delta2_uram_data,
    input                               delta2_uram_data_valid,
    input [PR_URAM_DWIDTH - 1 : 0]      pr_uram_data,
    input                               pr_uram_data_valid,

    // to ram
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta1_bram_rd_addr ,
    output reg                              delta1_bram_rd_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta2_uram_rd_addr,
    output reg                              delta2_uram_rd_valid,
    output reg [PR_URAM_AWIDTH - 1 : 0]     pr_uram_rd_addr,
    output reg                              pr_uram_rd_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta1_bram_wr_addr,
    output reg [DELTA_BRAM_DWIDTH - 1 : 0]  delta1_bram_wr_value,
    output reg                              delta1_bram_wr_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta2_uram_wr_addr,
    output reg [DELTA_BRAM_DWIDTH - 1 : 0]  delta2_uram_wr_value,
    output reg                              delta2_uram_wr_valid,
    output reg [PR_URAM_AWIDTH - 1 : 0]     pr_uram_wr_addr,
    output reg [PR_URAM_DWIDTH - 1 : 0]     pr_uram_wr_value,
    output reg                              pr_uram_wr_valid,

    output                                  stage_full,
    output reg [V_VALUE_WIDTH - 1 : 0]  src_recv_update_v_value,
    output reg                              src_recv_update_v_valid,
    output reg [V_ID_WIDTH - 1 : 0]         backend_active_v_id,
    output reg                              backend_active_v_updated,
    output reg                              backend_active_v_valid,
    output reg                              iteration_end,
    output reg                              iteration_end_valid
);
    wire [V_OFF_AWIDTH - 1 : 0] wr_delta_addr_top;
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta_value_top;
    wire [V_OFF_AWIDTH - 1 : 0] wr_pr_addr_top;
    wire [V_OFF_AWIDTH - 1 : 0] active_addr_top;
    wire [V_VALUE_WIDTH - 1 : 0]  active_pr_top;
    wire [V_VALUE_WIDTH - 1 : 0] active_delta_top_1, active_delta_top_2;
    wire delta_buffer_empty, pr_buffer_empty;
    wire delta_buffer_full, pr_buffer_full;
    wire active_addr_buffer_empty, active_delta_buffer_empty_0, active_delta_buffer_empty_1, active_pr_buffer_empty;
    reg [V_OFF_AWIDTH - 1 : 0] conflict_check_buffer [0 : FLOAT_ADD_DELAY - 1];
    reg conflict_valid_buffer [0 : FLOAT_ADD_DELAY - 1];
    wire [0 : FLOAT_ADD_DELAY - 1] conflict_array;
    wire conflict;
    wire active_read_signal;
    wire [V_VALUE_WIDTH - 1 : 0] active_data_multiply_result;
    wire active_data_multiply_result_valid;
    wire [V_VALUE_WIDTH - 1 : 0] active_data_substract_result;
    wire active_data_substract_result_valid;
    wire active_data_compare_result, active_data_compare_result_valid;
    reg [V_OFF_AWIDTH - 1 : 0] active_addr_reg [0 : FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY + FLOAT_SUBSTRACT_DELAY];
    reg active_addr_reg_valid [0 : FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY + FLOAT_SUBSTRACT_DELAY];
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta1_adder_result;
    wire wr_delta1_adder_result_valid;
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta2_adder_result;
    wire wr_delta2_adder_result_valid;
    wire [V_VALUE_WIDTH - 1 : 0] wr_pr_adder_result;
    wire wr_pr_adder_result_valid;

    reg [15 : 0] backend_send, backend_recv;
    reg [31 : 0] end_ct;

    generate
        genvar i;
        for (i = 0; i < FLOAT_ADD_DELAY; i = i + 1) begin
            assign conflict_array[i] = (conflict_check_buffer[i] == wr_delta_addr_top && conflict_valid_buffer[i]);
            // 这里判断如果在FLOAT_ADD_DELAY个时钟内，存在冲突发生，即有相同的地址再次访问，那么就判断有冲突发生
            // pr正确性的要求，需要每次都从bram里面读到最新值，但是浮点加的延迟导致最新值可能还在浮点加法器里面，并没有写回bram，所以必须延迟等它写回bram之后才能读
        end
    endgenerate
    
    assign conflict = !(conflict_array == {FLOAT_ADD_DELAY{1'b0}});
    assign active_read_signal = !active_addr_buffer_empty && !active_delta_buffer_empty_0 && !active_pr_buffer_empty;

    // full信号
    assign stage_full = delta_buffer_full || pr_buffer_full;

    reg [DELTA_BRAM_AWIDTH - 1 : 0] tmp_delta1_bram_rd_addr;
    reg tmp_delta1_bram_rd_valid;
    // reg [DELTA_BRAM_AWIDTH - 1 : 0] tmp_delta2_uram_rd_addr;
    // reg tmp_delta2_uram_rd_valid;


    // Note: (front_iteration_id[0] && delta1_bram_data_valid) == (!front_iteration_id[0] && delta2_uram_data_valid) == pr_uram_data_valid
    float_adder WR_DELTA1_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (front_iteration_id[0] && delta1_bram_data_valid),
        .s_axis_b_tdata         (delta1_bram_data[ITERATION_WIDTH - 1 : 0] == front_iteration_id ? delta1_bram_data[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH] : 0),
        .s_axis_b_tvalid        (front_iteration_id[0] && delta1_bram_data_valid),
        .m_axis_result_tdata    (wr_delta1_adder_result),
        .m_axis_result_tvalid   (wr_delta1_adder_result_valid)
    );//更新delta值，如果delta值在当前轮次已经被更新过，则再次更新；否则从0开始更新；

    float_adder WR_DELTA2_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (!front_iteration_id[0] && delta2_uram_data_valid),
        .s_axis_b_tdata         (delta2_uram_data[ITERATION_WIDTH - 1 : 0] == front_iteration_id ? delta2_uram_data[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH] : 0),
        .s_axis_b_tvalid        (!front_iteration_id[0] && delta2_uram_data_valid),
        .m_axis_result_tdata    (wr_delta2_adder_result),
        .m_axis_result_tvalid   (wr_delta2_adder_result_valid)
    );//更新delta值，如果delta值在当前轮次已经被更新过，则再次更新；否则从0开始更新；更新后加上轮次信息并写回

    float_adder WR_PR_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (pr_uram_data_valid),
        .s_axis_b_tdata         (pr_uram_data),
        .s_axis_b_tvalid        (pr_uram_data_valid),
        .m_axis_result_tdata    (wr_pr_adder_result),
        .m_axis_result_tvalid   (wr_pr_adder_result_valid)
    );// 更新pagerank值

    wr_delta_addr_fifo_ft WR_DELTA_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_addr_dst),
        .wr_en      (wr_valid_dst),
        .rd_en      (!delta_buffer_empty && !conflict), // 地址被存在fifo中，该地址在当前ADD_DELAY个时钟内未被访问，则进行访问
        .dout       (wr_delta_addr_top),
        .prog_full  (delta_buffer_full),
        .full       (),
        .empty      (delta_buffer_empty),
        .valid      ()
    );//这一fifo负责发出地址读取delta_bram和pr_uram的地址

    wr_delta_value_fifo_ft WR_DELTA_VALUE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_v_value_dst),
        .wr_en      (wr_valid_dst),
        .rd_en      (pr_uram_data_valid),
        .dout       (wr_delta_value_top),
        .full       (),
        .empty      (),
        .valid      ()
    );//wr_delta_value_top表示用于更新pr值和delta值的更新值,也即点的value值

    wr_pr_addr_fifo_ft WR_PR_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_addr_dst),
        .wr_en      (wr_valid_dst),
        .rd_en      (wr_pr_adder_result_valid && (wr_delta1_adder_result_valid || wr_delta2_adder_result_valid)),
        .dout       (wr_pr_addr_top),
        .prog_full       (pr_buffer_full),
        .empty      (pr_buffer_empty),
        .valid      ()
    );// 用于将点ID，也即地址进行记录，在写回pr值和delta值时输出；

    float_substract ACTIVE_DATA_SUB (
        .aclk                   (clk),
        .s_axis_a_tdata         (active_pr_top),
        .s_axis_a_tvalid        (active_read_signal),
        .s_axis_b_tdata         ( front_iteration_id == 0 ? 0: active_delta_top_1 ),
        .s_axis_b_tvalid        ( active_read_signal),
        .m_axis_result_tdata    (active_data_substract_result),
        .m_axis_result_tvalid   (active_data_substract_result_valid)
    );// pagerank[src_id].first - (iter_id == 1 ? 0 :next_delta[src_id]) 

    float_multiply ACTIVE_DATA_MULTIPLY (
        .aclk                   (clk),
        .s_axis_a_tdata         (active_data_substract_result),
        .s_axis_a_tvalid        (active_data_substract_result_valid),
        .s_axis_b_tdata         (E2),
        .s_axis_b_tvalid        (active_data_substract_result_valid),
        .m_axis_result_tdata    (active_data_multiply_result),
        .m_axis_result_tvalid   (active_data_multiply_result_valid)
    );//e2 *(pagerank[src_id].first - (iter_id == 1 ? 0 :next_delta[src_id]))

    float_compare ACTIVE_DATA_COMPARE (
        .aclk                   (clk),
        .s_axis_a_tdata         ({1'b0,active_delta_top_2[V_VALUE_WIDTH - 2 : 0]}),
        .s_axis_a_tvalid        (active_data_multiply_result_valid),
        .s_axis_b_tdata         (active_data_multiply_result),
        .s_axis_b_tvalid        (active_data_multiply_result_valid),
        .m_axis_result_tdata    (active_data_compare_result),
        .m_axis_result_tvalid   (active_data_compare_result_valid)
    ); // fabs(next_delta[src_id]) > e2 *(pagerank[src_id].first - (iter_id == 1 ? 0 :next_delta[src_id]))

    active_addr_fifo_ft ACTIVE_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (delta2_uram_wr_valid ? delta2_uram_wr_addr : delta1_bram_wr_addr),
        .wr_en      (delta1_bram_wr_valid || delta2_uram_wr_valid),
        .rd_en      (active_read_signal),
        .dout       (active_addr_top),
        .full       (),
        .empty      (active_addr_buffer_empty),
        .valid      ()
    );  // 记录计算pagerank值的点

    active_delta_fifo_ft ACTIVE_DELTA_FIFO_1_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (delta2_uram_wr_valid ? delta2_uram_wr_value[ DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH ] : delta1_bram_wr_value[ DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH ]),
        .wr_en      (delta2_uram_wr_valid || delta1_bram_wr_valid),
        .rd_en      (active_read_signal), //乘法完成后进行比较
        .dout       (active_delta_top_1),
        .full       (),
        .empty      (active_delta_buffer_empty_0),
        .valid      ()
    ); // 存放更新后的next_delta[src_id],用于做差

    active_delta_fifo_ft ACTIVE_DELTA_FIFO_2_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (active_delta_top_1),
        .wr_en      (active_read_signal),
        .rd_en      (active_data_multiply_result_valid), //乘法完成后进行比较
        .dout       (active_delta_top_2),
        .full       (),
        .empty      (active_delta_buffer_empty_1),
        .valid      ()
    ); // 存放更新后的next_delta[src_id]，用于比较

    active_pr_fifo_ft ACTIVE_PR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        ( pr_uram_wr_value ),
        .wr_en      ( pr_uram_wr_valid ),
        .rd_en      (active_read_signal),
        .dout       (active_pr_top),
        .full       (),
        .empty      (active_pr_buffer_empty),
        .valid      ()
    );// 这里存放从uram读取的pagerank值

    always @ (posedge clk) begin
        if (rst) begin
            backend_send <= 0;
            backend_recv <= 0;
        end else begin
            if (active_read_signal) begin
                backend_send <= backend_send + 1;
            end
            if (active_data_compare_result_valid) begin
                backend_recv <= backend_recv + 1;
            end
        end
    end
    // 这里是将M2读取的delta值返回到M4
    always @ (posedge clk) begin
        if (rst) begin
            src_recv_update_v_value <= 0;
            src_recv_update_v_valid <= 0;
        end else begin
            if (delta1_bram_data_valid && !front_iteration_id[0]) begin
                src_recv_update_v_value <= delta1_bram_data[ DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH ];
                src_recv_update_v_valid <= 1;
            end else if (delta2_uram_data_valid && front_iteration_id[0]) begin
                src_recv_update_v_value <= delta2_uram_data[ DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH ];
                src_recv_update_v_valid <= 1;
            end else begin
                src_recv_update_v_value <= 0;
                src_recv_update_v_valid <= 0;
            end
        end
    end

    integer j;
    always @ (posedge clk) begin
        if (active_read_signal) begin
            active_addr_reg[0] <= active_addr_top;
            active_addr_reg_valid[0] <= 1;
        end else begin
            active_addr_reg[0] <= 0;    
            active_addr_reg_valid[0] <= 0;
        end

        for (j = 0; j < FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY + FLOAT_SUBSTRACT_DELAY; j = j + 1) begin
            active_addr_reg[j + 1] <= active_addr_reg[j];
            active_addr_reg_valid[j + 1] <= active_addr_reg_valid[j];
        end
        if (rst) begin
            backend_active_v_id <= 0;
            backend_active_v_updated <= 0;
            backend_active_v_valid <= 0;
        end else begin
            if (active_data_compare_result_valid) begin
                backend_active_v_updated <= active_data_compare_result;
                backend_active_v_id <= (active_addr_reg[FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY + FLOAT_SUBSTRACT_DELAY ] << CORE_NUM_WIDTH) + CORE_ID;
                backend_active_v_valid <= 1;
            end else begin
                backend_active_v_id <= 0;
                backend_active_v_updated <= 0;
                backend_active_v_valid <= 0;
            end
            if (wr_valid_dst) begin
                $display("M10_vertex_bram_controller_single: Iteration_Id: %d, Core_Id: %d, active vertex, id: %d, value: %h", front_iteration_id, CORE_ID, wr_addr_dst * CORE_NUM + CORE_ID, wr_v_value_dst);
            end
            if (backend_active_v_valid) begin
                $display("M10_vertex_bram_controller_single: Iteration_Id: %d, Core_Id: %d, active vertex, id: %d, update: %d", front_iteration_id, CORE_ID, backend_active_v_id, backend_active_v_updated);
            end
        end
    end

    // send rd request
    always @ (posedge clk) begin
        if (rst) begin
            //这里注意修改bram访问时间会同时影响访问cu_delta和访问next_delta的时间，因此通过这里修改要注意修改两处的地方
            delta1_bram_rd_addr <= 0;
            delta1_bram_rd_valid <= 0;
            delta2_uram_rd_addr <= 0;
            delta2_uram_rd_valid <= 0;

            tmp_delta1_bram_rd_addr <= 0;
            tmp_delta1_bram_rd_valid <= 0;
            // tmp_delta2_uram_rd_addr <= 0;
            // tmp_delta2_uram_rd_valid <= 0;
            delta2_uram_rd_addr <= 0;
            delta2_uram_rd_valid <= 0;

            pr_uram_rd_addr <= 0;
            pr_uram_rd_valid <= 0;
            

            for (j = 0; j < FLOAT_ADD_DELAY; j = j + 1) begin
                conflict_check_buffer[j] <= 0;
                conflict_valid_buffer[j] <= 0;
            end
        end else begin
            // bram与uram相比，慢一个clk
            delta1_bram_rd_addr <= tmp_delta1_bram_rd_addr;
            delta1_bram_rd_valid <= tmp_delta1_bram_rd_valid;
            // delta2_uram_rd_addr <= tmp_delta2_uram_rd_addr;
            // delta2_uram_rd_valid <= tmp_delta2_uram_rd_valid;
            for (j = FLOAT_ADD_DELAY - 1; j > 0; j = j - 1) begin
                conflict_check_buffer[j] <= conflict_check_buffer[j - 1];
                conflict_valid_buffer[j] <= conflict_valid_buffer[j - 1];
            end
            // delta1
            if (!front_iteration_id[0] && rd_valid_src) begin
                tmp_delta1_bram_rd_addr <= rd_addr_src;
                tmp_delta1_bram_rd_valid <= 1;
            end else if (front_iteration_id[0] && !delta_buffer_empty && !conflict) begin
                tmp_delta1_bram_rd_addr <= wr_delta_addr_top;
                tmp_delta1_bram_rd_valid <= 1;
                // 更新delta1值前需要先将delta1的值读出来
            end else begin
                tmp_delta1_bram_rd_addr <= 0;
                tmp_delta1_bram_rd_valid <= 0;
            end
            // delta2
            if (front_iteration_id[0] && rd_valid_src) begin
                // tmp_delta2_uram_rd_addr <= rd_addr_src;
                // tmp_delta2_uram_rd_valid <= 1;
                delta2_uram_rd_addr <= rd_addr_src;
                delta2_uram_rd_valid <= 1;
            end else if (!front_iteration_id[0] && !delta_buffer_empty && !conflict) begin
                // tmp_delta2_uram_rd_addr <= wr_delta_addr_top;
                // tmp_delta2_uram_rd_valid <= 1;
                delta2_uram_rd_addr <= wr_delta_addr_top;
                delta2_uram_rd_valid <= 1;
            end else begin
                // tmp_delta2_uram_rd_addr <= 0;
                // tmp_delta2_uram_rd_valid <= 0;
                delta2_uram_rd_addr <= 0; 
                delta2_uram_rd_valid <= 0;
            end
            // pr
            if (!delta_buffer_empty && !conflict) begin
                pr_uram_rd_addr <= wr_delta_addr_top;
                pr_uram_rd_valid <= 1;
                // wr_delta_addr_top除了发出读取指令外，还把地址加入了检查buffer里，用于防止在ADD_DELAY个时钟内连续访问相同的地址
                conflict_check_buffer[0] <= wr_delta_addr_top;
                conflict_valid_buffer[0] <= 1;
            end else begin
                pr_uram_rd_addr <= 0;
                pr_uram_rd_valid <= 0;
                conflict_check_buffer[0] <= 0;
                conflict_valid_buffer[0] <= 0;
            end
        end
    end

    // send wr request
    always @ (posedge clk) begin
        if (rst) begin
            delta1_bram_wr_addr <= 0;
            delta1_bram_wr_value <= 0;
            delta1_bram_wr_valid <= 0;

            delta2_uram_wr_addr <= 0;
            delta2_uram_wr_value <= 0;
            delta2_uram_wr_valid <= 0;

            pr_uram_wr_addr <= 0;
            pr_uram_wr_value <= 0;
            pr_uram_wr_valid <= 0;
        end else begin
            if (!front_iteration_id[0] && wr_delta2_adder_result_valid) begin
                delta2_uram_wr_addr <= wr_pr_addr_top;
                delta2_uram_wr_value <= {wr_delta2_adder_result, front_iteration_id};
                delta2_uram_wr_valid <= 1;
                delta1_bram_wr_addr <= 0;
                delta1_bram_wr_value <= 0;
                delta1_bram_wr_valid <= 0;
            end else if (front_iteration_id[0] && wr_delta1_adder_result_valid) begin
                delta1_bram_wr_addr <= wr_pr_addr_top;
                delta1_bram_wr_value <= {wr_delta1_adder_result, front_iteration_id};
                delta1_bram_wr_valid <= 1;
                delta2_uram_wr_addr <= 0;
                delta2_uram_wr_value <= 0;
                delta2_uram_wr_valid <= 0;
            end else begin
                delta1_bram_wr_addr <= 0;
                delta1_bram_wr_value <= 0;
                delta1_bram_wr_valid <= 0;

                delta2_uram_wr_addr <= 0;
                delta2_uram_wr_value <= 0;
                delta2_uram_wr_valid <= 0;
            end
            // pr
            if (wr_pr_adder_result_valid) begin
                pr_uram_wr_addr <= wr_pr_addr_top;
                pr_uram_wr_value <= wr_pr_adder_result;  // 将相加结果写回pagerank_uram
                pr_uram_wr_valid <= 1;
            end else begin
                pr_uram_wr_addr <= 0;
                pr_uram_wr_value <= 0;
                pr_uram_wr_valid <= 0;
            end
            // debug info
            if (delta1_bram_wr_valid) begin
                $display("M10_vertex_bram_controller_single: Iteration_Id: %d, Core_Id: %d, Real_Id: %d, write delta1, addr=%d data=%h", front_iteration_id, CORE_ID, delta1_bram_wr_addr * CORE_NUM + CORE_ID, delta1_bram_wr_addr, delta1_bram_wr_value[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH]);
            end
            if (delta2_uram_wr_valid) begin
                $display("M10_vertex_bram_controller_single: Iteration_Id: %d, Core_Id: %d, Real_Id: %d, write delta2, addr=%d data=%h", front_iteration_id, CORE_ID, delta2_uram_wr_addr * CORE_NUM + CORE_ID, delta2_uram_wr_addr, delta2_uram_wr_value[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH]);
            end
            if (pr_uram_wr_valid) begin
                $display("M10_vertex_bram_controller_single: Iteration_Id: %d, Core_Id: %d, Real_Id: %d, write pr, addr=%d data=%h", front_iteration_id, CORE_ID, pr_uram_wr_addr * CORE_NUM + CORE_ID, pr_uram_wr_addr, pr_uram_wr_value);
            end
        end
    end

    always @ (posedge clk) begin
        if (rst) begin
            end_ct <= 0;
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end else begin
            if (front_iteration_end && front_iteration_end_valid && active_addr_buffer_empty  && delta_buffer_empty && pr_buffer_empty &&
                active_delta_buffer_empty_0 && active_delta_buffer_empty_1 && active_pr_buffer_empty && backend_send == backend_recv) begin
                if (end_ct >= `WAIT_END_DELAY) begin
                    // 强制等待20 clk
                    iteration_end <= 1;
                    iteration_end_valid <= 1;
                end else begin
                    end_ct <= end_ct + 1;
                    iteration_end <= 0;
                    iteration_end_valid <= 0;
                end
            end else begin
                end_ct <= 0;
                iteration_end <= 0;
                iteration_end_valid <= 0;
            end
        end
    end

endmodule

module pr_uram #(parameter
    AWIDTH = `PR_URAM_AWIDTH,
    DWIDTH = `PR_URAM_DWIDTH,
    NBPIPE = `URAM_DELAY - 2
) (
    input clk,
    input we,
    input mem_en,
    input [DWIDTH - 1 : 0] din,
    input [AWIDTH - 1 : 0] addra,
    input [AWIDTH - 1 : 0] addrb,

    output reg [DWIDTH - 1 : 0] dout
);


    //  Xilinx UltraRAM Single Port Read First Mode.  This code implements 
    //  a parameterizable UltraRAM block in read first mode. The behavior of this RAM is 
    //  when data is written, the old memory contents at the write address are 
    //  presented on the output port.
    //
    (* ram_style = "ultra" *)
    reg [DWIDTH - 1:0] mem [(1 << AWIDTH) - 1 : 0];
    reg [DWIDTH - 1:0] memreg;              
    reg [DWIDTH - 1:0] mem_pipe_reg [NBPIPE - 1 : 0];    // Pipelines for memory
    reg mem_en_pipe_reg[NBPIPE : 0];                // Pipelines for memory enable  

    integer          i;


/*
    initial begin
        for (i = 0; i < (1 << AWIDTH); i = i + 1) begin
            mem[i] <= i;
        end
    end
*/


    // RAM : Both READ and WRITE have a latency of one
    always @ (posedge clk) begin
       if(mem_en) begin
           if(we) begin
                mem[addra] <= din;
           end
           memreg <= mem[addrb];
       end
    end

    // The enable of the RAM goes through a pipeline to produce a
    // series of pipelined enable signals required to control the data
    // pipeline.
    always @ (posedge clk) begin
        mem_en_pipe_reg[0] <= mem_en;
        for (i = 0; i < NBPIPE; i = i + 1) begin
            mem_en_pipe_reg[i + 1] <= mem_en_pipe_reg[i];
        end
    end

    // RAM output data goes through a pipeline.
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[0]) begin
            mem_pipe_reg[0] <= memreg;
        end
    end    

    always @ (posedge clk) begin
        for (i = 0; i < NBPIPE - 1; i = i + 1) begin
            if (mem_en_pipe_reg[i + 1]) begin
                mem_pipe_reg[i + 1] <= mem_pipe_reg[i];
            end
        end
    end      

    // Final output register gives user the option to add a reset and
    // an additional enable signal just for the data ouptut
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[NBPIPE]) begin
            dout <= mem_pipe_reg[NBPIPE - 1];
        end
    end

endmodule

// delta存储迁移1个到uram中，缓解BRAM资源不够的问题
module delta_uram #(parameter
    AWIDTH = `DELTA_BRAM_AWIDTH,
    DWIDTH = `DELTA_BRAM_DWIDTH,
    NBPIPE = `URAM_DELAY - 2
) (
    input clk,
    input we,
    input mem_en,
    input [DWIDTH - 1 : 0] din,
    input [AWIDTH - 1 : 0] addra,
    input [AWIDTH - 1 : 0] addrb,

    output reg [DWIDTH - 1 : 0] dout
);


    //  Xilinx UltraRAM Single Port Read First Mode.  This code implements 
    //  a parameterizable UltraRAM block in read first mode. The behavior of this RAM is 
    //  when data is written, the old memory contents at the write address are 
    //  presented on the output port.
    //
    (* ram_style = "ultra" *)
    reg [DWIDTH - 1:0] mem [(1 << AWIDTH) - 1 : 0];
    reg [DWIDTH - 1:0] memreg;              
    reg [DWIDTH - 1:0] mem_pipe_reg [NBPIPE - 1 : 0];    // Pipelines for memory
    reg mem_en_pipe_reg[NBPIPE : 0];                // Pipelines for memory enable  

    integer          i;


/*
    initial begin
        for (i = 0; i < (1 << AWIDTH); i = i + 1) begin
            mem[i] <= i;
        end
    end
*/


    // RAM : Both READ and WRITE have a latency of one
    always @ (posedge clk) begin
       if(mem_en) begin
           if(we) begin
                mem[addra] <= din;
           end
           memreg <= mem[addrb];
       end
    end

    // The enable of the RAM goes through a pipeline to produce a
    // series of pipelined enable signals required to control the data
    // pipeline.
    always @ (posedge clk) begin
        mem_en_pipe_reg[0] <= mem_en;
        for (i = 0; i < NBPIPE; i = i + 1) begin
            mem_en_pipe_reg[i + 1] <= mem_en_pipe_reg[i];
        end
    end

    // RAM output data goes through a pipeline.
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[0]) begin
            mem_pipe_reg[0] <= memreg;
        end
    end    

    always @ (posedge clk) begin
        for (i = 0; i < NBPIPE - 1; i = i + 1) begin
            if (mem_en_pipe_reg[i + 1]) begin
                mem_pipe_reg[i + 1] <= mem_pipe_reg[i];
            end
        end
    end      

    // Final output register gives user the option to add a reset and
    // an additional enable signal just for the data ouptut
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[NBPIPE]) begin
            dout <= mem_pipe_reg[NBPIPE - 1];
        end
    end
						
endmodule