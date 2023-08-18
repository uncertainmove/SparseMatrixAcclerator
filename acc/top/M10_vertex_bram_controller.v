`timescale 1ns / 1ps

`include "accelerator.vh"

module vtx_ram_controller #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    DELTA_BRAM_DWIDTH = `DELTA_BRAM_DWIDTH,
    PR_URAM_AWIDTH = `PR_URAM_AWIDTH,
    PR_URAM_DWIDTH = `PR_URAM_DWIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM
) (
    input                                       clk,
    input [CORE_NUM - 1 : 0]                    rst,
    input                                       transfer_bram,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]          hbm_write_full,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       rd_addr_src,
    input [CORE_NUM - 1 : 0]                    rd_valid_src,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       wr_addr_dst,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    wr_v_value_dst,
    input [CORE_NUM - 1 : 0]                    wr_valid_dst,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0] front_iteration_id,

    // to hbm
    output reg [CORE_NUM - 1 : 0]               next_rst,
    output                                      transfer_done,
    output [CORE_NUM * HBM_DWIDTH_PER_CORE - 1 : 0] wb_bfs_data,
    output [PSEUDO_CHANNEL_NUM - 1 : 0] wb_bfs_valid,

    output [CORE_NUM - 1 : 0]                   stage_full,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   src_recv_update_v_value,
    output [CORE_NUM - 1 : 0]                   src_recv_update_v_valid,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      backend_active_v_id,
    output [CORE_NUM - 1 : 0]                   backend_active_v_updated,
    output [CORE_NUM - 1 : 0]                   backend_active_v_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid
);

    // from ram
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta1_bram_data [0 : CORE_NUM - 1];
    reg [2 : 0]                         delta1_bram_data_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_bram_data [0 : CORE_NUM - 1];
    reg [2 : 0]                         delta2_bram_data_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_DWIDTH - 1 : 0]       pr_uram_data [0 : CORE_NUM - 1];
    reg [2 : 0]                         pr_uram_data_valid [0 : CORE_NUM - 1];

    // to ram
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta1_bram_rd_addr [0 : CORE_NUM - 1];
    wire                                delta1_bram_rd_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_bram_rd_addr [0 : CORE_NUM - 1];
    wire                                delta2_bram_rd_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_AWIDTH - 1 : 0]       pr_uram_rd_addr [0 : CORE_NUM - 1];
    wire                                pr_uram_rd_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta1_bram_wr_addr [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta1_bram_wr_value [0 : CORE_NUM - 1];
    wire                                delta1_bram_wr_valid [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_bram_wr_addr [0 : CORE_NUM - 1];
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_bram_wr_value [0 : CORE_NUM - 1];
    wire                                delta2_bram_wr_valid [0 : CORE_NUM - 1];
    wire [PR_URAM_AWIDTH - 1 : 0]       pr_uram_wr_addr [0 : CORE_NUM - 1];
    wire [PR_URAM_DWIDTH - 1 : 0]       pr_uram_wr_value [0 : CORE_NUM - 1];
    wire                                pr_uram_wr_valid [0 : CORE_NUM - 1];

    reg [PR_URAM_AWIDTH - 1 : 0] local_transfer_addr [0 : 2];
    wire [PR_URAM_DWIDTH - 1 : 0] tmp_uram_data;
    reg local_transfer_valid [0 : 2];
    reg local_lock;
    
    assign transfer_done = (local_transfer_addr[2] == {PR_URAM_AWIDTH{1'b1}});
    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assign wb_bfs_data[(i + 1) * HBM_DWIDTH_PER_CORE - 1 : i * HBM_DWIDTH_PER_CORE] =
                {tmp_uram_data[i], local_transfer_addr[2], {CORE_NUM_WIDTH{1'b0}}} + i;
        end
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            assign wb_bfs_valid[i] = local_transfer_valid[2];
        end
    endgenerate

    always @ (posedge clk) begin
        next_rst <= rst;
    end
    
    always @ (posedge clk) begin
        if (rst) begin
            local_transfer_addr[0] <= 0;
            local_transfer_valid[0] <= 0;
            local_lock <= 1;
        end else begin
            if (transfer_bram && local_lock) begin
                local_transfer_addr[0] <= 0;
                local_transfer_valid[0] <= 1'b1;
                local_lock <= 0;
            end
            else if (transfer_bram && local_transfer_addr[0] < {PR_URAM_AWIDTH{1'b1}} && !hbm_write_full[0] && !hbm_write_full[1]) begin
                local_transfer_addr[0] <= local_transfer_addr[0] + 1;
                local_transfer_valid[0] <= 1'b1;
            end else begin
                local_transfer_valid[0] <= 0;
            end
        end
        local_transfer_addr[2] <= local_transfer_addr[1];
        local_transfer_addr[1] <= local_transfer_addr[0];
        local_transfer_valid[2] <= local_transfer_valid[1];
        local_transfer_valid[1] <= local_transfer_valid[0];
    end

    generate
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            vtx_ram_controller_single VTX_RAM_CONTROLLER_SINGLE (
                .clk                (clk),
                .rst                (rst[i]),
                .rd_addr_src        (rd_addr_src[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .rd_valid_src       (rd_valid_src[i]),
                .wr_addr_dst        (wr_addr_dst[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .wr_v_value_dst     (wr_v_value_dst[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .wr_valid_dst       (wr_valid_dst[i]),
                .front_iteration_end(front_iteration_end[i]),
                .front_iteration_end_valid  (front_iteration_end_valid[i]),
                .front_iteration_id (front_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),

                .delta1_bram_data               (delta1_bram_data[i]),
                .delta1_bram_data_valid         (delta1_bram_data_valid[i][2]),
                .delta2_bram_data               (delta2_bram_data[i]),
                .delta2_bram_data_valid         (delta2_bram_data_valid[i][2]),
                .pr_uram_data                   (pr_uram_data[i]),
                .pr_uram_data_valid             (pr_uram_data_valid[i][2]),
                .delta1_bram_rd_addr            (delta1_bram_rd_addr[i]),
                .delta1_bram_rd_valid           (delta1_bram_rd_valid[i]),
                .delta2_bram_rd_addr            (delta2_bram_rd_addr[i]),
                .delta2_bram_rd_valid           (delta2_bram_rd_valid[i]),
                .pr_uram_rd_addr                (pr_uram_rd_addr[i]),
                .pr_uram_rd_valid               (pr_uram_rd_valid[i]),
                .delta1_bram_wr_addr            (delta1_bram_wr_addr[i]),
                .delta1_bram_wr_value           (delta1_bram_wr_value[i]),
                .delta1_bram_wr_valid           (delta1_bram_wr_valid[i]),
                .delta2_bram_wr_addr            (delta2_bram_wr_addr[i]),
                .delta2_bram_wr_value           (delta2_bram_wr_value[i]),
                .delta2_bram_wr_valid           (delta2_bram_wr_valid[i]),
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
                .ena        (!rst[i]),
                .wea        (delta1_bram_wr_valid[i]),
                .addra      (delta1_bram_wr_addr[i]),
                .dina       (delta1_bram_wr_value[i]),
                .clkb       (clk),
                .enb        (!rst[i]),
                .addrb      (delta1_bram_rd_addr[i]),
                .doutb      (delta1_bram_data[i])
            );

            delta2_bram_ip_single DELTA2_BRAM_IP_SINGLE (
                .clka       (clk),
                .ena        (!rst[i]),
                .wea        (delta2_bram_wr_valid[i]),
                .addra      (delta2_bram_wr_addr[i]),
                .dina       (delta2_bram_wr_value[i]),
                .clkb       (clk),
                .enb        (!rst[i]),
                .addrb      (delta2_bram_rd_addr[i]),
                .doutb      (delta2_bram_data[i])
            );

            pr_uram PR_URAM (
                .clk    (clk),
                .we     (pr_uram_wr_valid[i]),
                .mem_en (1'b1),
                .din    (pr_uram_wr_value[i]),
                .addra  (pr_uram_wr_addr[i]),
                .addrb  (pr_uram_rd_addr[i]),

                .dout   (pr_uram_data[i])
            );

            always @ (posedge clk) begin
                delta1_bram_data_valid[i][0] <= delta1_bram_rd_valid[i];
                delta1_bram_data_valid[i][1] <= delta1_bram_data_valid[i][0];
                delta1_bram_data_valid[i][2] <= delta1_bram_data_valid[i][1];
                delta2_bram_data_valid[i][0] <= delta2_bram_rd_valid[i];
                delta2_bram_data_valid[i][1] <= delta2_bram_data_valid[i][0];
                delta2_bram_data_valid[i][2] <= delta2_bram_data_valid[i][1];
                pr_uram_data_valid[i][0] <= pr_uram_rd_valid[i];
                pr_uram_data_valid[i][1] <= pr_uram_data_valid[i][0];
                pr_uram_data_valid[i][2] <= pr_uram_data_valid[i][1];
            end
        end
    endgenerate
    
endmodule

module vtx_ram_controller_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    DELTA_BRAM_DWIDTH = `DELTA_BRAM_DWIDTH,
    PR_URAM_AWIDTH = `PR_URAM_AWIDTH,
    PR_URAM_DWIDTH = `PR_URAM_DWIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    FIFO_SIZE_WIDTH = `FIFO_SIZE_WIDTH,
    CORE_NUM = `CORE_NUM,
    E2 = `E2,
    FLOAT_ADD_DELAY = 12,
    FLOAT_MULTIPLY_DELAY = 9,
    FLOAT_COMPARE_DELAY = 2
) (
    input                   clk,
    input                   rst,
    input [V_ID_WIDTH - 1 : 0]      rd_addr_src,
    input                           rd_valid_src,
    input [V_ID_WIDTH - 1 : 0]      wr_addr_dst,
    input [V_VALUE_WIDTH - 1 : 0]   wr_v_value_dst,
    input                           wr_valid_dst,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] front_iteration_id,

    // from ram
    input [DELTA_BRAM_DWIDTH - 1 : 0]   delta1_bram_data,
    input                               delta1_bram_data_valid,
    input [DELTA_BRAM_DWIDTH - 1 : 0]   delta2_bram_data,
    input                               delta2_bram_data_valid,
    input [PR_URAM_DWIDTH - 1 : 0]      pr_uram_data,
    input                               pr_uram_data_valid,

    // to ram
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta1_bram_rd_addr,
    output reg                              delta1_bram_rd_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta2_bram_rd_addr,
    output reg                              delta2_bram_rd_valid,
    output reg [PR_URAM_AWIDTH - 1 : 0]     pr_uram_rd_addr,
    output reg                              pr_uram_rd_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta1_bram_wr_addr,
    output reg [DELTA_BRAM_DWIDTH - 1 : 0]  delta1_bram_wr_value,
    output reg                              delta1_bram_wr_valid,
    output reg [DELTA_BRAM_AWIDTH - 1 : 0]  delta2_bram_wr_addr,
    output reg [DELTA_BRAM_DWIDTH - 1 : 0]  delta2_bram_wr_value,
    output reg                              delta2_bram_wr_valid,
    output reg [PR_URAM_AWIDTH - 1 : 0]     pr_uram_wr_addr,
    output reg [PR_URAM_DWIDTH - 1 : 0]     pr_uram_wr_value,
    output reg                              pr_uram_wr_valid,

    output                                  stage_full,
    output reg [DELTA_BRAM_DWIDTH - 1 : 0]  src_recv_update_v_value,
    output reg                              src_recv_update_v_valid,
    output reg [V_ID_WIDTH - 1 : 0]         backend_active_v_id,
    output reg                              backend_active_v_updated,
    output reg                              backend_active_v_valid,
    output reg                              iteration_end,
    output reg                              iteration_end_valid
);
    wire [V_ID_WIDTH - 1 : 0] wr_delta_addr_top;
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta_value_top;
    wire [V_ID_WIDTH - 1 : 0] wr_pr_addr_top;
    wire [V_VALUE_WIDTH - 1 : 0] wr_pr_value_top;
    wire [V_ID_WIDTH - 1 : 0] active_addr_top;
    wire [V_VALUE_WIDTH - 1 : 0] active_delta_top, active_pr_top;
    wire delta_buffer_empty, pr_buffer_empty;
    wire delta_buffer_full, pr_buffer_full;
    wire active_addr_buffer_empty, active_delta_buffer_empty, active_pr_buffer_empty;
    reg [V_ID_WIDTH - 1 : 0] conflict_check_buffer [0 : FLOAT_ADD_DELAY - 1];
    reg conflict_valid_buffer [0 : FLOAT_ADD_DELAY - 1];
    wire [0 : FLOAT_ADD_DELAY - 1] conflict_array;
    wire conflict;
    wire active_read_signal;
    wire [V_VALUE_WIDTH - 1 : 0] active_data_multiply_result;
    wire active_data_multiply_result_valid;
    wire active_data_compare_result, active_data_compare_result_valid;
    reg [V_ID_WIDTH - 1 : 0] active_addr_reg [0 : FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY];
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta1_adder_result;
    wire wr_delta1_adder_result_valid;
    wire [V_VALUE_WIDTH - 1 : 0] wr_delta2_adder_result;
    wire wr_delta2_adder_result_valid;
    wire [V_VALUE_WIDTH - 1 : 0] wr_pr_adder_result;
    wire wr_pr_adder_result_valid;

    reg [15 : 0] backend_send, backend_recv;

    generate
        genvar i;
        for (i = 0; i < FLOAT_ADD_DELAY; i = i + 1) begin
            assign conflict_array[i] = (conflict_check_buffer[i] == wr_delta_addr_top && conflict_valid_buffer[i]);
        end
    endgenerate
    assign conflict = (conflict_array == {FLOAT_ADD_DELAY{1'b0}});
    assign active_read_signal = !active_addr_buffer_empty && !active_delta_buffer_empty && !active_pr_buffer_empty;

    // Note: (front_iteration_id[0] && delta1_bram_data_valid) == (!front_iteration_id[0] && delta2_bram_data_valid) == pr_uram_data_valid
    float_adder WR_DELTA1_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (front_iteration_id[0] && delta1_bram_data_valid),
        .s_axis_b_tdata         (delta1_bram_data[ITERATION_WIDTH - 1 : 0] == front_iteration_id ? delta1_bram_data[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH] : 0),
        .s_axis_b_tvalid        (front_iteration_id[0] && delta1_bram_data_valid),
        .m_axis_result_tdata    (wr_delta1_adder_result),
        .m_axis_result_tvalid   (wr_delta1_adder_result_valid)
    );

    float_adder WR_DELTA2_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (!front_iteration_id[0] && delta2_bram_data_valid),
        .s_axis_b_tdata         (delta2_bram_data[ITERATION_WIDTH - 1 : 0] == front_iteration_id ? delta2_bram_data[DELTA_BRAM_DWIDTH - 1 : ITERATION_WIDTH] : 0),
        .s_axis_b_tvalid        (!front_iteration_id[0] && delta2_bram_data_valid),
        .m_axis_result_tdata    (wr_delta2_adder_result),
        .m_axis_result_tvalid   (wr_delta2_adder_result_valid)
    );

    float_adder WR_PR_ADDER (
        .aclk       (clk),
        .s_axis_a_tdata         (wr_delta_value_top),
        .s_axis_a_tvalid        (pr_uram_data_valid),
        .s_axis_b_tdata         (pr_uram_data),
        .s_axis_b_tvalid        (pr_uram_data_valid),
        .m_axis_result_tdata    (wr_pr_adder_result),
        .m_axis_result_tvalid   (wr_pr_adder_result_valid)
    );

    wr_delta_addr_fifo_ft WR_DELTA_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_addr_dst),
        .wr_en      (wr_valid_dst),
        .rd_en      (!delta_buffer_empty && !conflict),
        .dout       (wr_delta_addr_top),
        .prog_full  (delta_buffer_full),
        .full       (),
        .empty      (delta_buffer_empty),
        .valid      ()
    );

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
    );

    wr_pr_addr_fifo_ft WR_PR_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_addr_dst),
        .wr_en      (wr_valid_dst),
        .rd_en      (wr_pr_adder_result_valid && (wr_delta1_adder_result_valid || wr_delta2_adder_result_valid)),
        .dout       (wr_pr_addr_top),
        .full       (pr_buffer_full),
        .empty      (pr_buffer_empty),
        .valid      ()
    );

    float_multiply ACTIVE_DATA_MULTIPLY (
        .aclk                   (clk),
        .s_axis_a_tdata         (active_pr_top),
        .s_axis_a_tvalid        (active_read_signal),
        .s_axis_b_tdata         (E2),
        .s_axis_b_tvalid        (active_read_signal),
        .m_axis_result_tdata    (active_data_multiply_result),
        .m_axis_result_tvalid   (active_data_multiply_result_valid)
    );

    float_compare ACTIVE_DATA_COMPARE (
        .aclk                   (clk),
        .s_axis_a_tdata         (active_delta_top),
        .s_axis_a_tvalid        (active_data_multiply_result_valid),
        .s_axis_b_tdata         (active_data_multiply_result),
        .s_axis_b_tvalid        (active_data_multiply_result_valid),
        .m_axis_result_tdata    (active_data_compare_result),
        .m_axis_result_tvalid   (active_data_compare_result_valid)
    );

    active_addr_fifo_ft ACTIVE_ADDR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (wr_pr_addr_top),
        .wr_en      (delta1_bram_wr_valid || delta2_bram_wr_valid),
        .rd_en      (active_read_signal),
        .dout       (active_addr_top),
        .full       (),
        .empty      (active_addr_buffer_empty),
        .valid      ()
    );

    active_delta_fifo_ft ACTIVE_DELTA_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (delta2_bram_wr_valid ? delta2_bram_wr_value : delta1_bram_wr_value),
        .wr_en      (delta2_bram_wr_valid || delta1_bram_wr_valid),
        .rd_en      (active_data_multiply_result_valid),
        .dout       (active_delta_top),
        .full       (),
        .empty      (active_delta_buffer_empty),
        .valid      ()
    );

    active_pr_fifo_ft ACTIVE_PR_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_iteration_id == 0 ? pr_uram_wr_value : pr_uram_data),
        .wr_en      (front_iteration_id == 0 ? pr_uram_wr_valid : pr_uram_data_valid),
        .rd_en      (active_read_signal),
        .dout       (active_pr_top),
        .full       (),
        .empty      (active_pr_buffer_empty),
        .valid      ()
    );

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

    always @ (posedge clk) begin
        if (rst) begin
            src_recv_update_v_value <= 0;
            src_recv_update_v_valid <= 0;
        end else begin
            if (delta1_bram_data_valid && !front_iteration_id[0]) begin
                src_recv_update_v_value <= delta1_bram_data;
                src_recv_update_v_valid <= 1;
            end else if (delta2_bram_data_valid && front_iteration_id[0]) begin
                src_recv_update_v_value <= delta2_bram_data;
                src_recv_update_v_valid <= 1;
            end else begin
                src_recv_update_v_value <= 0;
                src_recv_update_v_valid <= 0;
            end
        end
    end

    integer j;
    always @ (posedge clk) begin
        active_addr_reg[0] <= active_addr_top;
        for (j = 0; j < FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY; j = j + 1) begin
            active_addr_reg[j + 1] <= active_addr_reg[j];
        end
        if (rst) begin
            backend_active_v_id <= 0;
            backend_active_v_updated <= 0;
            backend_active_v_valid <= 0;
        end else begin
            if (active_data_compare_result_valid) begin
                backend_active_v_updated <= active_data_compare_result;
                backend_active_v_id <= active_addr_reg[FLOAT_MULTIPLY_DELAY + FLOAT_COMPARE_DELAY - 1];
                backend_active_v_valid <= 1;
            end else begin
                backend_active_v_id <= 0;
                backend_active_v_updated <= 0;
                backend_active_v_valid <= 0;
            end
        end
    end

    // send rd request
    always @ (posedge clk) begin
        if (rst) begin
            delta1_bram_rd_addr <= 0;
            delta1_bram_rd_valid <= 0;

            delta2_bram_rd_addr <= 0;
            delta2_bram_rd_valid <= 0;

            pr_uram_rd_addr <= 0;
            pr_uram_rd_valid <= 0;

            for (j = 0; j < FLOAT_ADD_DELAY; j = j + 1) begin
                conflict_check_buffer[j] <= 0;
                conflict_valid_buffer[j] <= 0;
            end
        end else begin
            for (j = FLOAT_ADD_DELAY - 1; j > 0; j = j - 1) begin
                conflict_check_buffer[j] <= conflict_check_buffer[j - 1];
                conflict_valid_buffer[j] <= conflict_valid_buffer[j - 1];
            end
            // delta1
            if (!front_iteration_id[0] && rd_valid_src) begin
                delta1_bram_rd_addr <= rd_addr_src;
                delta1_bram_rd_valid <= 1;
            end else if (front_iteration_id[0] && !delta_buffer_empty && !conflict) begin
                delta1_bram_rd_addr <= wr_delta_addr_top;
                delta1_bram_rd_valid <= 1;
            end else begin
                delta1_bram_rd_addr <= 0;
                delta1_bram_rd_valid <= 0;
            end
            // delta2
            if (front_iteration_id[0] && rd_valid_src) begin
                delta2_bram_rd_addr <= rd_addr_src;
                delta2_bram_rd_valid <= 1;
            end else if (!front_iteration_id[0] && !delta_buffer_empty && !conflict) begin
                delta2_bram_rd_addr <= wr_delta_addr_top;
                delta2_bram_rd_valid <= 1;
            end else begin
                delta2_bram_rd_addr <= 0;
                delta2_bram_rd_valid <= 0;
            end
            // pr
            if (!delta_buffer_empty && !conflict) begin
                pr_uram_rd_addr <= wr_delta_addr_top;
                pr_uram_rd_valid <= 1;

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

            delta2_bram_wr_addr <= 0;
            delta2_bram_wr_value <= 0;
            delta2_bram_wr_valid <= 0;

            pr_uram_wr_addr <= 0;
            pr_uram_wr_value <= 0;
            pr_uram_wr_valid <= 0;
        end else begin
            if (!front_iteration_id[0] && wr_delta2_adder_result_valid) begin
                delta2_bram_wr_addr <= wr_pr_addr_top;
                delta2_bram_wr_value <= {wr_delta2_adder_result, front_iteration_id};
                delta2_bram_wr_valid <= 1;

                delta1_bram_wr_addr <= 0;
                delta1_bram_wr_value <= 0;
                delta1_bram_wr_valid <= 0;
            end else if (front_iteration_id[0] && wr_delta1_adder_result_valid) begin
                delta1_bram_wr_addr <= wr_pr_addr_top;
                delta1_bram_wr_value <= {wr_delta1_adder_result, front_iteration_id};
                delta1_bram_wr_valid <= 1;

                delta2_bram_wr_addr <= 0;
                delta2_bram_wr_value <= 0;
                delta2_bram_wr_valid <= 0;
            end else begin
                delta1_bram_wr_addr <= 0;
                delta1_bram_wr_value <= 0;
                delta1_bram_wr_valid <= 0;

                delta2_bram_wr_addr <= 0;
                delta2_bram_wr_value <= 0;
                delta2_bram_wr_valid <= 0;
            end
            // pr
            if (wr_pr_adder_result_valid) begin
                pr_uram_wr_addr <= wr_pr_addr_top;
                pr_uram_wr_value <= wr_pr_adder_result;
                pr_uram_wr_valid <= 1;
            end else begin
                pr_uram_wr_addr <= 0;
                pr_uram_wr_value <= 0;
                pr_uram_wr_valid <= 0;
            end
        end
    end

    always @ (posedge clk) begin
        if (!rst && front_iteration_end && front_iteration_end_valid && active_addr_buffer_empty &&
                active_delta_buffer_empty && active_pr_buffer_empty && backend_send == backend_recv) begin
            iteration_end <= 1;
            iteration_end_valid <= 1;
        end else begin
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end
    end

endmodule

module pr_uram #(parameter
    AWIDTH = `PR_URAM_AWIDTH,
    DWIDTH = `PR_URAM_DWIDTH,
    NBPIPE = 3
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
    reg [DWIDTH - 1:0] mem [(1 << AWIDTH) - 1 : 0];        // Memory Declaration
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