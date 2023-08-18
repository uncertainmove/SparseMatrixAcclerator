`timescale 1ns / 1ps

`include "accelerator.vh"

module generate_hbm_edge_rqst #(parameter
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    GROUP_CORE_NUM = `GROUP_CORE_NUM,
    CORE_NUM = `CORE_NUM
) (
    input                               clk,
    input [CORE_NUM - 1 : 0]            rst,
    input [CORE_NUM * HBM_AWIDTH - 1 : 0]       front_rd_hbm_edge_addr,
    input [CORE_NUM * HBM_EDGE_MASK - 1 : 0]    front_rd_hbm_edge_mask,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id,
    input [CORE_NUM - 1 : 0]                    front_rd_hbm_edge_valid,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value,
    input [CORE_NUM - 1 : 0]                    front_active_v_value_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]          hbm_full,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]          next_stage_full,

    output reg [CORE_NUM - 1 : 0]             next_rst,
    output [CORE_NUM - 1 : 0]                 stage_full,
    output [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]    rd_hbm_edge_addr,
    output [PSEUDO_CHANNEL_NUM - 1 : 0]                 rd_hbm_edge_valid,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      active_v_id,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   active_v_value,
    output [CORE_NUM - 1 : 0]                   active_v_id_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id
);

    generate
        genvar i;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i * GROUP_CORE_NUM+: GROUP_CORE_NUM] <= rst[i * GROUP_CORE_NUM+: GROUP_CORE_NUM];
            end
            generate_hbm_edge_rqst_single #(.CHANNEL_ID(i)) GENERATE_HBM_EDGE_RQST_SINGLE (
                .clk                    (clk),
                .rst                    (rst[i]),
                .front_rd_hbm_edge_addr (front_rd_hbm_edge_addr[(i + 1) * GROUP_CORE_NUM * HBM_AWIDTH - 1 : i * GROUP_CORE_NUM * HBM_AWIDTH]),
                .front_rd_hbm_edge_mask (front_rd_hbm_edge_mask[(i + 1) * GROUP_CORE_NUM * HBM_EDGE_MASK - 1 : i * GROUP_CORE_NUM * HBM_EDGE_MASK]),
                .front_active_v_id      (front_active_v_id[(i + 1) * GROUP_CORE_NUM * V_ID_WIDTH - 1 : i * GROUP_CORE_NUM * V_ID_WIDTH]),
                .front_rd_hbm_edge_valid(front_rd_hbm_edge_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .front_active_v_value   (front_active_v_value[(i + 1) * GROUP_CORE_NUM * V_VALUE_WIDTH - 1 : i * GROUP_CORE_NUM * V_VALUE_WIDTH]),
                .front_active_v_value_valid   (front_active_v_value_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .front_iteration_end    (front_iteration_end[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .front_iteration_end_valid    (front_iteration_end_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .front_iteration_id     (front_iteration_id[(i + 1) * GROUP_CORE_NUM * ITERATION_WIDTH - 1 : i * GROUP_CORE_NUM * ITERATION_WIDTH]),
                .hbm_full               (hbm_full[i]),
                .next_stage_full        (next_stage_full[i]),

                .stage_full             (stage_full[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .rd_hbm_edge_addr       (rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .rd_hbm_edge_valid      (rd_hbm_edge_valid[i]),
                .active_v_id            (active_v_id[(i + 1) * GROUP_CORE_NUM * V_ID_WIDTH - 1 : i * GROUP_CORE_NUM * V_ID_WIDTH]),
                .active_v_value         (active_v_value[(i + 1) * GROUP_CORE_NUM * V_VALUE_WIDTH - 1 : i * GROUP_CORE_NUM * V_VALUE_WIDTH]),
                .active_v_id_valid      (active_v_id_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .iteration_end          (iteration_end[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .iteration_end_valid    (iteration_end_valid[(i + 1) * GROUP_CORE_NUM - 1 : i * GROUP_CORE_NUM]),
                .iteration_id           (iteration_id[(i + 1) * GROUP_CORE_NUM * ITERATION_WIDTH - 1 : i * GROUP_CORE_NUM * ITERATION_WIDTH])
            );
        end
    endgenerate

endmodule

module generate_hbm_edge_rqst_single #(parameter
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    GROUP_CORE_NUM = `GROUP_CORE_NUM,
    GROUP_CORE_NUM_WIDTH = `GROUP_CORE_NUM_WIDTH,
    FIFO_SIZE_WIDTH = `FIFO_SIZE_WIDTH,
    CHANNEL_ID = 0,
    CORE_NUM = `CORE_NUM
) (
    input                               clk,
    input                               rst,
    input [GROUP_CORE_NUM * HBM_AWIDTH - 1 : 0]         front_rd_hbm_edge_addr,
    input [GROUP_CORE_NUM * HBM_EDGE_MASK - 1 : 0]      front_rd_hbm_edge_mask,
    input [GROUP_CORE_NUM * V_ID_WIDTH - 1 : 0]         front_active_v_id,
    input [GROUP_CORE_NUM * V_VALUE_WIDTH - 1 : 0]      front_active_v_value,
    input [GROUP_CORE_NUM - 1 : 0]                      front_active_v_value_valid,
    input [GROUP_CORE_NUM - 1 : 0]                      front_rd_hbm_edge_valid,   
    input [GROUP_CORE_NUM - 1 : 0]                      front_iteration_end,
    input [GROUP_CORE_NUM - 1 : 0]                      front_iteration_end_valid,
    input [GROUP_CORE_NUM * ITERATION_WIDTH - 1 : 0]    front_iteration_id,
    input                                           hbm_full,
    input                                           next_stage_full,

    output [GROUP_CORE_NUM - 1 : 0]                         stage_full,
    output reg [HBM_AWIDTH - 1 : 0]                         rd_hbm_edge_addr,
    output reg                                              rd_hbm_edge_valid,
    output reg [GROUP_CORE_NUM * V_ID_WIDTH - 1 : 0]        active_v_id,
    output reg [GROUP_CORE_NUM * V_VALUE_WIDTH - 1 : 0]     active_v_value,
    output reg [GROUP_CORE_NUM - 1 : 0]                     active_v_id_valid,
    output reg [GROUP_CORE_NUM - 1 : 0]                     iteration_end,
    output reg [GROUP_CORE_NUM - 1 : 0]                     iteration_end_valid,
    output reg [GROUP_CORE_NUM * ITERATION_WIDTH - 1 : 0]   iteration_id
);

    wire [GROUP_CORE_NUM - 1 : 0] edge_addr_buffer_empty, edge_addr_buffer_full;
    wire [GROUP_CORE_NUM - 1 : 0] local_active_v_id_buffer_empty, local_active_v_id_buffer_full;
    wire [GROUP_CORE_NUM - 1 : 0] local_active_v_value_buffer_empty, local_active_v_value_buffer_full;
    wire [GROUP_CORE_NUM * HBM_AWIDTH - 1 : 0] local_edge_addr;
    wire [HBM_EDGE_MASK - 1 : 0] local_edge_mask [0 : GROUP_CORE_NUM - 1];
    wire [V_ID_WIDTH - 1 : 0] local_active_v_id [0 : GROUP_CORE_NUM - 1];
    wire [V_VALUE_WIDTH - 1 : 0] local_active_v_value [0 : GROUP_CORE_NUM - 1];
    wire local_active_v_id_valid [0 : GROUP_CORE_NUM - 1];
    wire [GROUP_CORE_NUM - 1 : 0] local_min_edge_addr_array;
    wire [GROUP_CORE_NUM - 1 : 0] sel_signal [0 : GROUP_CORE_NUM - 1];

    wire edge_addr_buffer_all_empty;
    assign edge_addr_buffer_all_empty = (local_active_v_value_buffer_empty == {GROUP_CORE_NUM{1'b1}});

    gen_min_addr GEN_MIN_ADDR (
        .front_edge_addr(local_edge_addr),
        .edge_addr_buffer_empty(edge_addr_buffer_empty),
        
        .min_edge_addr_array  (local_min_edge_addr_array)
    );

    always @(posedge clk) begin
        if (rst) begin
            rd_hbm_edge_addr <= 0;
            rd_hbm_edge_valid <= 0;
        end else begin
            if (hbm_full || next_stage_full || edge_addr_buffer_all_empty) begin
                rd_hbm_edge_addr <= 0;
                rd_hbm_edge_valid <= 0;
            end else begin
                casex (local_min_edge_addr_array)
                    16'b0000000000000000: begin
                        rd_hbm_edge_addr <= 0;
                        rd_hbm_edge_valid <= 0;
                    end
                    16'b???????????????1: begin
                        rd_hbm_edge_addr <= local_edge_addr[1 * HBM_AWIDTH - 1 : 0 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b??????????????10: begin
                        rd_hbm_edge_addr <= local_edge_addr[2 * HBM_AWIDTH - 1 : 1 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b?????????????100: begin
                        rd_hbm_edge_addr <= local_edge_addr[3 * HBM_AWIDTH - 1 : 2 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b????????????1000: begin
                        rd_hbm_edge_addr <= local_edge_addr[4 * HBM_AWIDTH - 1 : 3 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b???????????10000: begin
                        rd_hbm_edge_addr <= local_edge_addr[5 * HBM_AWIDTH - 1 : 4 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b??????????100000: begin
                        rd_hbm_edge_addr <= local_edge_addr[6 * HBM_AWIDTH - 1 : 5 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b?????????1000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[7 * HBM_AWIDTH - 1 : 6 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b????????10000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[8 * HBM_AWIDTH - 1 : 7 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b???????100000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[9 * HBM_AWIDTH - 1 : 8 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b??????1000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[10 * HBM_AWIDTH - 1 : 9 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b?????10000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[11 * HBM_AWIDTH - 1 : 10 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b????100000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[12 * HBM_AWIDTH - 1 : 11 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b???1000000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[13 * HBM_AWIDTH - 1 : 12 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b??10000000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[14 * HBM_AWIDTH - 1 : 13 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b?100000000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[15 * HBM_AWIDTH - 1 : 14 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                    16'b1000000000000000: begin
                        rd_hbm_edge_addr <= local_edge_addr[16 * HBM_AWIDTH - 1 : 15 * HBM_AWIDTH];
                        rd_hbm_edge_valid <= 1;
                    end
                endcase
            end
        end
    end

    always @ (posedge clk) begin
        iteration_id <= front_iteration_id;
        if (!rst && (&front_iteration_end) && (&front_iteration_end_valid) && local_active_v_id_buffer_empty) begin
            iteration_end <= {GROUP_CORE_NUM{1'b1}};
            iteration_end_valid <= {GROUP_CORE_NUM{1'b1}};
        end else begin
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end
    end

    assign stage_full = edge_addr_buffer_full;
    generate
        genvar i;
        genvar j;
        for (i = 0; i < GROUP_CORE_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assign sel_signal[i][j] = local_min_edge_addr_array[j] && local_edge_mask[j][i] && !edge_addr_buffer_empty[j];
            end
            // output
            always @ (posedge clk) begin
                if (rst) begin
                    active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= 0;
                    active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= 0;
                    active_v_id_valid[i] <= 0;
                end else begin
                    if (hbm_full || next_stage_full || edge_addr_buffer_all_empty) begin
                        active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= 0;
                        active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= 0;
                        active_v_id_valid[i] <= 0;
                    end else begin
                        case (sel_signal[i])
                            16'b0000000000000000: begin
                                // send invalid signal, check active_v_id in next stage
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= {V_ID_WIDTH{1'b1}};
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= 1;
                                active_v_id_valid[i] <= 1;
                            end
                            16'b0000000000000001: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[0];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[0];
                                active_v_id_valid[i] <= local_active_v_id_valid[0];
                            end
                            16'b0000000000000010: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[1];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[1];
                                active_v_id_valid[i] <= local_active_v_id_valid[1];
                            end
                            16'b0000000000000100: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[2];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[2];
                                active_v_id_valid[i] <= local_active_v_id_valid[2];
                            end
                            16'b0000000000001000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[3];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[3];
                                active_v_id_valid[i] <= local_active_v_id_valid[3];
                            end
                            16'b0000000000010000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[4];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[4];
                                active_v_id_valid[i] <= local_active_v_id_valid[4];
                            end
                            16'b0000000000100000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[5];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[5];
                                active_v_id_valid[i] <= local_active_v_id_valid[5];
                            end
                            16'b0000000001000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[6];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[6];
                                active_v_id_valid[i] <= local_active_v_id_valid[6];
                            end
                            16'b0000000010000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[7];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[7];
                                active_v_id_valid[i] <= local_active_v_id_valid[7];
                            end
                            16'b0000000100000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[8];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[8];
                                active_v_id_valid[i] <= local_active_v_id_valid[8];
                            end
                            16'b0000001000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[9];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[9];
                                active_v_id_valid[i] <= local_active_v_id_valid[9];
                            end
                            16'b0000010000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[10];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[10];
                                active_v_id_valid[i] <= local_active_v_id_valid[10];
                            end
                            16'b0000100000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[11];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[11];
                                active_v_id_valid[i] <= local_active_v_id_valid[11];
                            end
                            16'b0001000000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[12];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[12];
                                active_v_id_valid[i] <= local_active_v_id_valid[12];
                            end
                            16'b0010000000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[13];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[13];
                                active_v_id_valid[i] <= local_active_v_id_valid[13];
                            end
                            16'b0100000000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[14];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[14];
                                active_v_id_valid[i] <= local_active_v_id_valid[14];
                            end
                            16'b1000000000000000: begin
                                active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= local_active_v_id[15];
                                active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= local_active_v_value[15];
                                active_v_id_valid[i] <= local_active_v_id_valid[15];
                            end
                        endcase
                    end
                end
            end

            edge_addr_fifo_ft EDGE_ADDR_FIFO_FT (
                .clk        (clk),
                .srst       (rst),
                .din        (front_rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .wr_en      (front_rd_hbm_edge_valid[i]),
                .rd_en      (!hbm_full && !next_stage_full && !edge_addr_buffer_all_empty && local_min_edge_addr_array[i]),
                .dout       (local_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .prog_full    (edge_addr_buffer_full[i]),
                .full       (),
                .empty      (edge_addr_buffer_empty[i]),
                .valid      ()
            );

            edge_mask_fifo_ft EDGE_MASK_FIFO_FT (
                .clk        (clk),
                .srst       (rst),
                .din        (front_rd_hbm_edge_mask[(i + 1) * HBM_EDGE_MASK - 1 : i * HBM_EDGE_MASK]),
                .wr_en      (front_rd_hbm_edge_valid[i]),
                .rd_en      (!hbm_full && !next_stage_full && !edge_addr_buffer_all_empty && local_min_edge_addr_array[i]),
                .dout       (local_edge_mask[i]),
                .full       (),
                .empty      (),
                .valid      ()
            );

            active_v_id_fifo_ft ACTIVE_V_ID_FIFO_FT (
                .clk        (clk),
                .srst       (rst),
                .din        (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .wr_en      (front_rd_hbm_edge_valid[i]),
                .rd_en      (!hbm_full && !next_stage_full && !edge_addr_buffer_all_empty && local_min_edge_addr_array[i]),
                .dout       (local_active_v_id[i]),
                .prog_full  (local_active_v_id_buffer_full[i]),
                .full       (),
                .empty      (local_active_v_id_buffer_empty[i]),
                .valid      (local_active_v_id_valid[i])
            );

            active_v_value_fifo_ft ACTIVE_V_VALUE_FIFO_FT (
                .clk        (clk),
                .srst       (rst),
                .din        (front_active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .wr_en      (front_active_v_value_valid[i]),
                .rd_en      (!hbm_full && !next_stage_full && !edge_addr_buffer_all_empty && local_min_edge_addr_array[i]),
                .dout       (local_active_v_value[i]),
                .full       (),
                .empty      (local_active_v_value_buffer_empty),
                .valid      ()
            );
        end
    endgenerate

endmodule

module gen_min_addr #(
    HBM_AWIDTH = `HBM_AWIDTH,
    GROUP_CORE_NUM = `GROUP_CORE_NUM, // 16
    GROUP_CORE_NUM_WIDTH = `GROUP_CORE_NUM_WIDTH
) (
    input [GROUP_CORE_NUM * HBM_AWIDTH - 1 : 0] front_edge_addr,
    input [GROUP_CORE_NUM - 1 : 0]              edge_addr_buffer_empty,

    output [GROUP_CORE_NUM - 1 : 0]             min_edge_addr_array
);

    wire [HBM_AWIDTH - 1 : 0] local_min_edge_addr_level0 [0 : GROUP_CORE_NUM - 1];
    wire [GROUP_CORE_NUM - 1 : 0] compare [0 : GROUP_CORE_NUM - 1 ];
    /*
    generate
        genvar i;
        for (i = 0; i < 16; i = i + 1) begin
            assign local_min_edge_addr_level0[i] = edge_addr_buffer_empty[i] ? {HBM_AWIDTH{1'b1}} : front_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH];
        end
        for (i = 0; i < 8; i = i + 1) begin
            assign local_min_edge_addr_level1[i] = local_min_edge_addr_level0[2 * i + 1] > local_min_edge_addr_level0[2 * i] ? local_min_edge_addr_level0[2 * i] : local_min_edge_addr_level0[2 * i + 1];
        end
        for (i = 0; i < 4; i = i + 1) begin
            assign local_min_edge_addr_level2[i] = local_min_edge_addr_level1[2 * i + 1] > local_min_edge_addr_level1[2 * i] ? local_min_edge_addr_level1[2 * i] : local_min_edge_addr_level1[2 * i + 1];
        end
        for (i = 0; i < 2; i = i + 1) begin
            assign local_min_edge_addr_level3[i] = local_min_edge_addr_level2[2 * i + 1] > local_min_edge_addr_level2[2 * i] ? local_min_edge_addr_level2[2 * i] : local_min_edge_addr_level2[2 * i + 1];
        end
    endgenerate
    assign min_edge_addr = local_min_edge_addr_level3[0] > local_min_edge_addr_level3[1] ? local_min_edge_addr_level3[1] : local_min_edge_addr_level3[0];
    */
    generate
        genvar i,j;
        for (i = 0; i < GROUP_CORE_NUM; i = i + 1) begin
            assign local_min_edge_addr_level0[i] = edge_addr_buffer_empty[i] ? {HBM_AWIDTH{1'b1}} : front_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH];
        end
        for (i = 0; i < GROUP_CORE_NUM; i = i + 1) begin
            for(j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assign compare[i][j] = local_min_edge_addr_level0[i] > local_min_edge_addr_level0[j];
            end
        end
        for (i = 0; i < GROUP_CORE_NUM; i = i + 1) begin
            assign min_edge_addr_array[i] = compare[i] == 0;
        end
    endgenerate
    
endmodule