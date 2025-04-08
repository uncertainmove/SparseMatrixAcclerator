`timescale 1ns / 1ps

`include "accelerator.vh"

module scheduler #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    CORE_NUM = `CORE_NUM,
    HBM_DWIDTH = `HBM_DWIDTH,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    GROUP_CORE_NUM = `GROUP_CORE_NUM
) (
    input                               clk,
    input [CORE_NUM - 1 : 0]            rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value,
    input [CORE_NUM - 1 : 0]                    front_active_v_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,
    input [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0] hbm_interface_active_v_edge,
    input [CORE_NUM - 1 : 0]                        hbm_interface_active_v_edge_valid,
    input [CORE_NUM - 1 : 0]                        next_stage_full,

    output reg [CORE_NUM - 1 : 0]               next_rst,
    output [PSEUDO_CHANNEL_NUM - 1 : 0]         stage_full,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      update_v_id,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   update_v_value,
    output [CORE_NUM - 1 : 0]                   update_v_valid
);

    wire [CORE_NUM - 1 : 0] local_stage_full;

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            scheduler_single #(.CORE_ID(i)) SCHEDULER_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .front_active_v_id      (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .front_active_v_value   (front_active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .front_active_v_valid   (front_active_v_valid[i]),
                .front_iteration_end    (front_iteration_end[i]),
                .front_iteration_end_valid    (front_iteration_end_valid[i]),
                .front_iteration_id     (front_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),
                .hbm_interface_active_v_edge    (hbm_interface_active_v_edge[(i + 1) * HBM_DWIDTH_PER_CORE - 1 : i * HBM_DWIDTH_PER_CORE]),
                .hbm_interface_active_v_edge_valid  (hbm_interface_active_v_edge_valid[i]),
                .next_stage_full        (next_stage_full[i]),

                .stage_full             (local_stage_full[i]),
                .update_v_id            (update_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .update_v_value         (update_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .update_v_valid         (update_v_valid[i]),
                .iteration_end          (iteration_end[i]),
                .iteration_end_valid    (iteration_end_valid[i]),
                .iteration_id           (iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH])
            );
        end
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            assign stage_full[i] = |local_stage_full[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM];
        end
   endgenerate

endmodule

module scheduler_single #(parameter
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    CORE_ID = 0,
    CORE_NUM = `CORE_NUM,
    FIFO_SIZE_WIDTH = `FIFO_SIZE_WIDTH
) (
    input                               clk,
    input                               rst,
    input [V_ID_WIDTH - 1 : 0]          front_active_v_id,
    input [V_VALUE_WIDTH - 1 : 0]       front_active_v_value,
    input                               front_active_v_valid,
    input                               front_iteration_end,
    input                               front_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0]     front_iteration_id,
    input [V_ID_WIDTH - 1 : 0]          hbm_interface_active_v_edge,
    input                               hbm_interface_active_v_edge_valid,
    input                               next_stage_full,

    output                                  stage_full,
    output reg [V_ID_WIDTH - 1 : 0]         update_v_id,
    output reg [V_VALUE_WIDTH - 1 : 0]      update_v_value,
    output reg                              update_v_valid,
    output reg                              iteration_end,
    output reg                              iteration_end_valid,
    output reg [ITERATION_WIDTH - 1 : 0]    iteration_id
);

    wire active_v_edge_buffer_full, active_v_edge_buffer_empty;
    wire active_v_id_buffer_full, active_v_id_buffer_empty;
    wire [V_ID_WIDTH - 1 : 0] active_v_id_top;
    wire [V_VALUE_WIDTH - 1 : 0] active_v_value_top;
    wire [V_ID_WIDTH - 1 : 0] active_v_edge_top;
    wire buffer_read_signal;
    reg [31 : 0] end_ct;

    assign stage_full = active_v_id_buffer_full;
    assign buffer_read_signal = !next_stage_full && !active_v_id_buffer_empty && !active_v_edge_buffer_empty;

    active_v_edge_fifo_ft ACTIVE_V_EDGE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (hbm_interface_active_v_edge),
        .wr_en      (hbm_interface_active_v_edge_valid),
        .rd_en      (buffer_read_signal),
        .dout       (active_v_edge_top),
        .prog_full  (active_v_edge_buffer_full),
        .full       (),
        .empty      (active_v_edge_buffer_empty),
        .valid      ()
    );

    active_v_id_fifo_ft ACTIVE_V_ID_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_id),
        .wr_en      (front_active_v_valid),
        .rd_en      (buffer_read_signal),
        .dout       (active_v_id_top),
        .prog_full  (active_v_id_buffer_full),
        .full       (),
        .empty      (active_v_id_buffer_empty),
        .valid      ()
    );

    active_v_value_fifo_ft ACTIVE_V_VALUE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_value),
        .wr_en      (front_active_v_valid),
        .rd_en      (buffer_read_signal),
        .dout       (active_v_value_top),
        .full       (),
        .empty      (),
        .valid      ()
    );

    always @ (posedge clk) begin
        if (rst) begin
            update_v_id <= 0;
            update_v_value <= 0;
            update_v_valid <= 0;
        end else begin
            if (buffer_read_signal) begin
                if (active_v_id_top != {V_ID_WIDTH{1'b1}}) begin
                    update_v_id <= active_v_edge_top;
                    update_v_value <= active_v_value_top;
                    update_v_valid <= 1;
                    if(active_v_edge_top == 89564) begin
                        $display("id 89564 edge %d value %h",active_v_id_top,active_v_value_top);
                    end

                    if(active_v_edge_top == 35090) begin
                        $display("id 35090 edge %d value %h",active_v_id_top,active_v_value_top);
                    end // 6.96374016229e-08 => -6.92087e-09

                    

                end else begin
                    update_v_id <= 0;
                    update_v_value <= 0;
                    update_v_valid <= 0;
                end
            end else begin
                update_v_id <= 0;
                update_v_value <= 0;
                update_v_valid <= 0;
            end
        end
    end

    always @ (posedge clk) begin
        iteration_id <= front_iteration_id;
        if (rst) begin
            end_ct <= 0;
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end else begin
            if (front_iteration_end && front_iteration_end_valid && active_v_id_buffer_empty) begin
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