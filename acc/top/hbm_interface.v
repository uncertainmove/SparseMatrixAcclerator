`timescale 1ns / 1ps

`include "accelerator.vh"

module hbm_interface #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_DWIDTH = `HBM_DWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    GROUP_CORE_NUM = `GROUP_CORE_NUM,
    CORE_NUM = `CORE_NUM
) (
    input                               clk,
    input [CORE_NUM - 1 : 0]            rst,
    input [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]     front_rd_hbm_edge_addr,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]                  front_rd_hbm_edge_valid,
    input [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0]     hbm_controller_edge,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]                  hbm_controller_valid,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]                  hbm_controller_full,

    output [PSEUDO_CHANNEL_NUM - 1 : 0]                 stage_full,
    output [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]    rd_hbm_edge_addr,
    output [PSEUDO_CHANNEL_NUM - 1 : 0]                 rd_hbm_edge_valid,
    output [CORE_NUM * HBM_AWIDTH - 1 : 0]              active_v_edge,
    output [CORE_NUM - 1 : 0]                           active_v_edge_valid
);

    generate
        genvar i;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            hbm_interface_send_rqst_single #(.PSEUDO_ID(i)) HBM_INTERFACE_SEND_RQST_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .front_rd_hbm_edge_addr (front_rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .front_rd_hbm_edge_valid(front_rd_hbm_edge_valid[i]),
                .hbm_controller_full    (hbm_controller_full[i]),

                .stage_full             (stage_full[i]),
                .rd_hbm_edge_addr       (rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .rd_hbm_edge_valid      (rd_hbm_edge_valid[i])
            );

            hbm_interface_recv_edge_single #(.PSEUDO_ID(i)) HBM_INTERFACE_RECV_EDGE_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .hbm_controller_edge    (hbm_controller_edge[(i + 1) * HBM_DWIDTH - 1 : i * HBM_DWIDTH]),
                .hbm_controller_valid   (hbm_controller_valid[i]),

                .active_v_edge          (active_v_edge[(i + 1) * GROUP_CORE_NUM * HBM_AWIDTH - 1 : i * GROUP_CORE_NUM * HBM_AWIDTH]),
                .active_v_edge_valid    (active_v_edge_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM])
            );
        end
    endgenerate

endmodule

module hbm_interface_send_rqst_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    GROUP_CORE_NUM = `GROUP_CORE_NUM,
    PSEUDO_ID = 0,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH
) (
    input                           clk,
    input                           rst,
    input [HBM_AWIDTH - 1 : 0]      front_rd_hbm_edge_addr,
    input                           front_rd_hbm_edge_valid,
    input                           hbm_controller_full,

    output                          stage_full,
    output [HBM_AWIDTH - 1 : 0]     rd_hbm_edge_addr,
    output                          rd_hbm_edge_valid
);

    wire edge_addr_buffer_empty, edge_addr_buffer_full;

    assign stage_full = edge_addr_buffer_full;
    controller_edge_addr_fifo CONTROLLER_EDGE_ADDR_FIFO (
        .clk        (clk),
        .srst       (rst),
        .din        (front_rd_hbm_edge_addr + ((1 << 15) >> CORE_NUM_WIDTH)),
        .wr_en      (front_rd_hbm_edge_valid),
        .rd_en      (!edge_addr_buffer_empty && !hbm_controller_full),
        .dout       (rd_hbm_edge_addr),
        .prog_full  (edge_addr_buffer_full),
        .full       (),
        .empty      (edge_addr_buffer_empty),
        .valid      (rd_hbm_edge_valid)
    );
/*
    always @ (posedge clk) begin
        if (rst) begin
            stage_full <= 0;
            rd_hbm_edge_addr <= 0;
            rd_hbm_edge_valid <= 0;
        end else begin
            stage_full <= hbm_controller_full;
            rd_hbm_edge_addr <= front_rd_hbm_edge_addr;
            rd_hbm_edge_valid <= front_rd_hbm_edge_valid;
        end
    end
*/

endmodule

module hbm_interface_recv_edge_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HBM_DWIDTH = `HBM_DWIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    PSEUDO_ID = 0,
    GROUP_CORE_NUM = `GROUP_CORE_NUM,
    CORE_NUM = `CORE_NUM
) (
    input                           clk,
    input                           rst,
    input [HBM_DWIDTH - 1 : 0]      hbm_controller_edge,
    input                           hbm_controller_valid,

    output [GROUP_CORE_NUM * HBM_AWIDTH - 1 : 0]    active_v_edge,
    output [GROUP_CORE_NUM - 1 : 0]                 active_v_edge_valid
);

    wire controller_edge_buffer_empty, controller_edge_buffer_full;
    wire local_valid;

    controller_edge_fifo CONTROLLER_EDGE_FIFO (
        .clk        (clk),
        .srst       (rst),
        .din        (hbm_controller_edge),
        .wr_en      (hbm_controller_valid),
        .rd_en      (!controller_edge_buffer_empty),
        .dout       (active_v_edge),
        .prog_full  (controller_edge_buffer_full),
        .full       (),
        .empty      (controller_edge_buffer_empty),
        .valid      (local_valid)
    );
    assign active_v_edge_valid = {GROUP_CORE_NUM{local_valid}};
/*
    always @ (posedge clk) begin
        if (rst) begin
            active_v_edge <= 0;
            active_v_edge_valid <= 0;
        end else begin
            active_v_edge <= hbm_controller_edge;
            active_v_edge_valid <= hbm_controller_valid;
        end
    end
*/

endmodule