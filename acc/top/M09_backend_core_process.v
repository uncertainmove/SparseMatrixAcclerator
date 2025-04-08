`timescale 1ns / 1ps

`include "accelerator.vh"

module backend_core #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    CORE_NUM = `CORE_NUM
) (
    input                                       clk,
    input [CORE_NUM - 1 : 0]                    rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       recv_update_v_id,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    recv_update_v_value,
    input [CORE_NUM - 1 : 0]                    recv_update_v_valid,
    input [CORE_NUM - 1 : 0]                    recv_iteration_end,
    input [CORE_NUM - 1 : 0]                    recv_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  recv_iteration_id,
    input [CORE_NUM - 1 : 0]                    next_stage_full,

    output reg [CORE_NUM - 1 : 0]               next_rst,
    output [CORE_NUM - 1 : 0]                   dest_core_full,
    output [CORE_NUM * V_OFF_AWIDTH - 1 : 0]    wr_vertex_bram_addr,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   wr_vertex_bram_data,
    output [CORE_NUM - 1 : 0]                   wr_vertex_bram_valid,
    output [CORE_NUM - 1 : 0]                   wr_vertex_bram_iteration_end,
    output [CORE_NUM - 1 : 0]                   wr_vertex_bram_iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] wr_vertex_bram_iteration_id
);

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            backend_destination_core_single #(.CORE_ID(i)) BACKEND_DESTINATION_CORE_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .recv_update_v_id   (recv_update_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .recv_update_v_value(recv_update_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .recv_update_v_valid(recv_update_v_valid[i]),
                .recv_iteration_end (recv_iteration_end[i]),
                .recv_iteration_end_valid (recv_iteration_end_valid[i]),
                .recv_iteration_id  (recv_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),
                .next_stage_full        (next_stage_full[i]),

                .dest_core_full         (dest_core_full[i]),
                .wr_vertex_bram_addr    (wr_vertex_bram_addr[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .wr_vertex_bram_data    (wr_vertex_bram_data[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .wr_vertex_bram_valid   (wr_vertex_bram_valid[i]),
                .wr_vertex_bram_iteration_end   (wr_vertex_bram_iteration_end[i]),
                .wr_vertex_bram_iteration_end_valid   (wr_vertex_bram_iteration_end_valid[i]),
                .wr_vertex_bram_iteration_id    (wr_vertex_bram_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH])
            );
        end
    endgenerate

endmodule

module backend_destination_core_single #(parameter
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    CORE_ID = 0,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH
) (
    input                   clk,
    input                   rst,
    input [V_ID_WIDTH - 1 : 0]      recv_update_v_id,
    input [V_VALUE_WIDTH - 1 : 0]   recv_update_v_value,
    input                           recv_update_v_valid,
    input                           recv_iteration_end,
    input                           recv_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] recv_iteration_id,
    input                           next_stage_full,

    output reg                          dest_core_full,
    output reg [V_OFF_AWIDTH - 1 : 0]     wr_vertex_bram_addr,
    output reg [V_VALUE_WIDTH - 1 : 0]  wr_vertex_bram_data,
    output reg                          wr_vertex_bram_valid,
    output reg                          wr_vertex_bram_iteration_end,
    output reg                          wr_vertex_bram_iteration_end_valid,
    output reg [ITERATION_WIDTH - 1 : 0]wr_vertex_bram_iteration_id
);

    integer i;
    always @ (posedge clk) begin
        if (rst) begin
            wr_vertex_bram_addr <= 0;
            wr_vertex_bram_data <= 0;
            wr_vertex_bram_valid <= 0;

            dest_core_full <= 0;
        end else begin
            dest_core_full <= next_stage_full;
            if (recv_update_v_valid) begin
                wr_vertex_bram_addr <= recv_update_v_id >> CORE_NUM_WIDTH;
                wr_vertex_bram_data <= recv_update_v_value;
                wr_vertex_bram_valid <= 1;
            end else begin
                wr_vertex_bram_addr <= 0;
                wr_vertex_bram_data <= 0;
                wr_vertex_bram_valid <= 0;
            end
        end
    end

    always @ (posedge clk) begin
        wr_vertex_bram_iteration_id <= recv_iteration_id;
        if (!rst && recv_iteration_end && recv_iteration_end_valid && !recv_update_v_valid) begin
            wr_vertex_bram_iteration_end <= 1;
            wr_vertex_bram_iteration_end_valid <= 1;
        end else begin
            wr_vertex_bram_iteration_end <= 0;
            wr_vertex_bram_iteration_end_valid <= 0;
        end
    end
    
endmodule