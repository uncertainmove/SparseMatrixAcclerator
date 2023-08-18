`timescale 1ns / 1ps

`include "accelerator.vh"

module rd_active_vertex_offset #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    CORE_NUM = `CORE_NUM
) (
    input                           clk,
    input [CORE_NUM - 1 : 0]        rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id,
    input [CORE_NUM - 1 : 0]                    front_active_v_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,
    input [CORE_NUM - 1 : 0]                    next_stage_full,

    output reg [CORE_NUM - 1 : 0]                   next_rst,
    output [CORE_NUM - 1 : 0]                       stage_full,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]          active_v_id,
    output [CORE_NUM - 1 : 0]                       active_v_id_valid,
    output [CORE_NUM * V_OFF_AWIDTH - 1 : 0]        rd_active_v_offset_addr,
    output [CORE_NUM * DELTA_BRAM_AWIDTH - 1 : 0]   rd_active_v_value_addr,
    output [CORE_NUM - 1 : 0]                       rd_active_v_addr_valid,
    output [CORE_NUM - 1 : 0]                       iteration_end,
    output [CORE_NUM - 1 : 0]                       iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0]     iteration_id
);

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            rd_active_vertex_offset_single #(.CORE_ID(i)) RD_ACTIVE_VERTEX_OFFSET_SINGLE (
                .clk        (clk),
                .rst        (rst[i]),
                .front_active_v_id  (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .front_active_v_valid   (front_active_v_valid[i]),
                .front_iteration_end    (front_iteration_end[i]),
                .front_iteration_end_valid  (front_iteration_end_valid[i]),
                .front_iteration_id     (front_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),
                .next_stage_full        (next_stage_full[i]),

                .stage_full             (stage_full[i]),
                .active_v_id            (active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .active_v_id_valid      (active_v_id_valid[i]),
                .rd_active_v_offset_addr(rd_active_v_offset_addr[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .rd_active_v_value_addr (rd_active_v_value_addr[(i + 1) * DELTA_BRAM_AWIDTH - 1 : i * DELTA_BRAM_AWIDTH]),
                .rd_active_v_addr_valid (rd_active_v_addr_valid[i]),
                .iteration_end          (iteration_end[i]),
                .iteration_end_valid    (iteration_end_valid[i]),
                .iteration_id           (iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH])
            );
        end
    endgenerate

endmodule

module rd_active_vertex_offset_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_ID = 0,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH
) (
    input                           clk,
    input                           rst,
    input [V_ID_WIDTH - 1 : 0]      front_active_v_id,
    input                           front_active_v_valid,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] front_iteration_id,
    input                           next_stage_full,

    output                                  stage_full,
    output [V_ID_WIDTH - 1 : 0]             active_v_id,
    output                                  active_v_id_valid,
    output [V_OFF_AWIDTH - 1 : 0]           rd_active_v_offset_addr,
    output [DELTA_BRAM_AWIDTH - 1 : 0]      rd_active_v_value_addr,
    output                                  rd_active_v_addr_valid,
    output reg                              iteration_end,
    output reg                              iteration_end_valid,
    output reg [ITERATION_WIDTH - 1 : 0]    iteration_id
);

    wire active_v_id_buffer_empty, active_v_id_buffer_full;
    wire [31 : 0]   divider_dout_taddr;
    wire            divider_dout_tvalid;

    assign stage_full = active_v_id_buffer_full;
    assign rd_active_v_offset_addr = active_v_id >> CORE_NUM_WIDTH;
    assign rd_active_v_value_addr = active_v_id >> CORE_NUM_WIDTH;
    assign rd_active_v_addr_valid = active_v_id_valid;

    // note : full signal send to front stage has one cycle delay
    active_v_id_fifo ACTIVE_V_ID_FIFO (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_id),
        .wr_en      (front_active_v_valid),
        .rd_en      (!next_stage_full && !active_v_id_buffer_empty),
        .dout       (active_v_id),
        .prog_full  (active_v_id_buffer_full),
        .full       (),
        .empty      (active_v_id_buffer_empty),
        .valid      (active_v_id_valid)
    );

    always @ (posedge clk) begin
        iteration_id <= front_iteration_id;
        if (!rst && front_iteration_end && front_iteration_end_valid && active_v_id_buffer_empty) begin
            iteration_end <= 1;
            iteration_end_valid <= 1;
        end else begin
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end
    end

endmodule