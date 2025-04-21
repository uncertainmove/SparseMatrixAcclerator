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
    CORE_NUM = `CORE_NUM,
    PORT_NUM = 4
) (
    input                               clk,
    input [CORE_NUM - 1 : 0]            rst,
    input [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]     front_rd_hbm_edge_addr,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]                  front_rd_hbm_edge_valid,
    input [PSEUDO_CHANNEL_NUM * PORT_NUM * HBM_DWIDTH - 1 : 0]  hbm_controller_edge,
    input [PSEUDO_CHANNEL_NUM * PORT_NUM - 1 : 0]               hbm_controller_valid,
    input [PSEUDO_CHANNEL_NUM * PORT_NUM - 1 : 0]               hbm_controller_full,

    output [PSEUDO_CHANNEL_NUM - 1 : 0]                 stage_full,
    output [PSEUDO_CHANNEL_NUM * PORT_NUM * HBM_AWIDTH - 1 : 0] rd_hbm_edge_addr,
    output [PSEUDO_CHANNEL_NUM * PORT_NUM - 1 : 0]               rd_hbm_edge_valid,
    output [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0]    active_v_edge,
    output [CORE_NUM - 1 : 0]                           active_v_edge_valid
);

    generate
        genvar i;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            hbm_interface_send_rqst_single #(.PSEUDO_ID(i), .PORT_NUM(PORT_NUM)) HBM_INTERFACE_SEND_RQST_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .front_rd_hbm_edge_addr (front_rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .front_rd_hbm_edge_valid(front_rd_hbm_edge_valid[i]),
                .hbm_controller_full    (hbm_controller_full[(i + 1) * PORT_NUM - 1 : i * PORT_NUM]),

                .stage_full             (stage_full[i]),
                .rd_hbm_edge_addr       (rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH * PORT_NUM - 1 : i * HBM_AWIDTH * PORT_NUM]),
                .rd_hbm_edge_valid      (rd_hbm_edge_valid[(i + 1) * PORT_NUM - 1 : i * PORT_NUM])
            );

            hbm_interface_recv_edge_single #(.PSEUDO_ID(i), .PORT_NUM(PORT_NUM)) HBM_INTERFACE_RECV_EDGE_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .hbm_controller_edge    (hbm_controller_edge[(i + 1) * HBM_DWIDTH * PORT_NUM - 1 : i * HBM_DWIDTH * PORT_NUM]),
                .hbm_controller_valid   (hbm_controller_valid[(i + 1) * PORT_NUM - 1 : i * PORT_NUM]),

                .active_v_edge          (active_v_edge[(i + 1) * HBM_DWIDTH - 1 : i * HBM_DWIDTH]),
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
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    PORT_NUM = 4
) (
    input                           clk,
    input                           rst,
    input [HBM_AWIDTH - 1 : 0]      front_rd_hbm_edge_addr,
    input                           front_rd_hbm_edge_valid,
    input [PORT_NUM - 1 : 0]        hbm_controller_full,

    output                          stage_full,
    output [PORT_NUM * HBM_AWIDTH - 1 : 0]  rd_hbm_edge_addr,
    output [PORT_NUM - 1 : 0]               rd_hbm_edge_valid
);

    wire edge_addr_buffer_empty [0 : PORT_NUM - 1];
    wire edge_addr_buffer_full [0 : PORT_NUM - 1];
    reg [1 : 0] ch;

    always @ (posedge clk) begin
        if (rst) begin
            ch <= 0;
        end else begin
            if (front_rd_hbm_edge_valid) begin
                ch <= ch + 1;
            end
        end
    end

    assign stage_full = edge_addr_buffer_full[0] || edge_addr_buffer_full[1] || edge_addr_buffer_full[2] || edge_addr_buffer_full[3];

    generate
        genvar i;
        for (i = 0; i < PORT_NUM; i = i + 1) begin
            controller_edge_addr_fifo CONTROLLER_EDGE_ADDR_FIFO (
                .clk        (clk),
                .srst       (rst),
                .din        (front_rd_hbm_edge_addr),
                .wr_en      (front_rd_hbm_edge_valid && ch == i),
                .rd_en      (!edge_addr_buffer_empty[i] && !hbm_controller_full[i]),
                .dout       (rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .prog_full  (edge_addr_buffer_full[i]),
                .full       (),
                .empty      (edge_addr_buffer_empty[i]),
                .valid      (rd_hbm_edge_valid[i])
            );
        end
    endgenerate

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
    CORE_NUM = `CORE_NUM,
    PORT_NUM = 4
) (
    input                           clk,
    input                           rst,
    input [HBM_DWIDTH * PORT_NUM - 1 : 0]   hbm_controller_edge,
    input [PORT_NUM - 1 : 0]                hbm_controller_valid,

    output [HBM_DWIDTH - 1 : 0]     active_v_edge,
    output [GROUP_CORE_NUM - 1 : 0] active_v_edge_valid
);

    wire controller_edge_buffer_empty [0 : PORT_NUM - 1];
    wire controller_edge_buffer_full [0 : PORT_NUM - 1];
    wire local_valid [0 : PORT_NUM - 1];
    wire [HBM_DWIDTH - 1 : 0] active_v_edge_tmp [0 : PORT_NUM - 1];
    reg [1 : 0] ch;
    
    always @ (posedge clk) begin
        if (rst) begin
            ch <= 0;
        end else begin
            if ((!controller_edge_buffer_empty[0] && ch == 0) || (!controller_edge_buffer_empty[1] && ch == 1) || (!controller_edge_buffer_empty[2] && ch == 2) || (!controller_edge_buffer_empty[3] && ch == 3)) begin
                ch <= ch + 1;
            end
        end
    end

    generate
        genvar i;
        for (i = 0; i < PORT_NUM; i = i + 1) begin
            controller_edge_fifo CONTROLLER_EDGE_FIFO (
                .clk        (clk),
                .srst       (rst),
                .din        (hbm_controller_edge[(i + 1) * HBM_DWIDTH - 1 : i * HBM_DWIDTH]),
                .wr_en      (hbm_controller_valid[i]),
                .rd_en      (!controller_edge_buffer_empty[i] && ch == i),
                .dout       (active_v_edge_tmp[i]),
                .prog_full  (controller_edge_buffer_full[i]),
                .full       (),
                .empty      (controller_edge_buffer_empty[i]),
                .valid      (local_valid[i])
            );
        end
    endgenerate

    assign active_v_edge_valid = {GROUP_CORE_NUM{local_valid[0] || local_valid[1] || local_valid[2] || local_valid[3]}};
    assign active_v_edge = local_valid[0] ? active_v_edge_tmp[0] : local_valid[1] ? active_v_edge_tmp[1] : local_valid[2] ? active_v_edge_tmp[2] : active_v_edge_tmp[3];

endmodule