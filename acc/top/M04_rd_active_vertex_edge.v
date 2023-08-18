`timescale 1ns / 1ps

`include "accelerator.vh"

module rd_active_vertex_edge #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    CORE_NUM = `CORE_NUM
) (
    input                               clk,
    input [CORE_NUM - 1 : 0]            rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id,
    input [CORE_NUM - 1 : 0]                    front_active_v_valid,
    input [CORE_NUM * V_OFF_DWIDTH - 1 : 0]     front_active_v_loffset,
    input [CORE_NUM * V_OFF_DWIDTH - 1 : 0]     front_active_v_roffset,
    input [CORE_NUM - 1 : 0]                    front_offset_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value,
    input [CORE_NUM - 1 : 0]                    front_active_v_value_valid,
    input [CORE_NUM - 1 : 0]                    combine_stage_full,

    output reg [CORE_NUM - 1 : 0]       next_rst,
    output [CORE_NUM - 1 : 0]           stage_full,
    output [CORE_NUM * HBM_AWIDTH - 1 : 0]      rd_hbm_edge_addr,
    output [CORE_NUM * HBM_EDGE_MASK - 1 : 0]   rd_hbm_edge_mask,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      hbm_active_v_id,
    output [CORE_NUM - 1 : 0]                   rd_hbm_edge_valid,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   hbm_active_v_value,
    output [CORE_NUM - 1 : 0]                   hbm_active_v_value_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id
);

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            rd_active_vertex_edge_single #(.CORE_ID(i)) RD_ACTIVE_VERTEX_EDGE_SINGLE (
                .clk                            (clk),
                .rst                            (rst[i]),
                .front_active_v_id              (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .front_active_v_valid           (front_active_v_valid[i]),
                .front_active_v_loffset         (front_active_v_loffset[(i + 1) * V_OFF_DWIDTH - 1 : i * V_OFF_DWIDTH]),
                .front_active_v_roffset         (front_active_v_roffset[(i + 1) * V_OFF_DWIDTH - 1 : i * V_OFF_DWIDTH]),
                .front_offset_valid             (front_offset_valid[i]),
                .front_iteration_end            (front_iteration_end[i]),
                .front_iteration_end_valid      (front_iteration_end_valid[i]),
                .front_iteration_id             (front_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),
                .front_active_v_value           (front_active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .front_active_v_value_valid     (front_active_v_value_valid[i]),
                .combine_stage_full             (combine_stage_full[i]),

                .stage_full                     (stage_full[i]),
                .rd_hbm_edge_addr               (rd_hbm_edge_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH]),
                .rd_hbm_edge_mask               (rd_hbm_edge_mask[(i + 1) * HBM_EDGE_MASK - 1 : i * HBM_EDGE_MASK]),
                .hbm_active_v_id                (hbm_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .rd_hbm_edge_valid              (rd_hbm_edge_valid[i]),
                .hbm_active_v_value             (hbm_active_v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                .hbm_active_v_value_valid       (hbm_active_v_value_valid[i]),
                .iteration_end                  (iteration_end[i]),
                .iteration_end_valid            (iteration_end_valid[i]),
                .iteration_id                   (iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH])
            );
        end
    endgenerate

endmodule

module rd_active_vertex_edge_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_EDGE_MASK = `HBM_EDGE_MASK,
    CORE_ID = 0,
    CACHELINE_LEN_WIDTH = `CACHELINE_LEN_WIDTH,
    CACHELINE_LEN = `CACHELINE_LEN,
    FIFO_SIZE_WIDTH = `FIFO_SIZE_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    DAMPING = `DAMPING
) (
    input                           clk,
    input                           rst,
    input [V_ID_WIDTH - 1 : 0]      front_active_v_id,
    input                           front_active_v_valid,
    input [V_OFF_DWIDTH - 1 : 0]    front_active_v_loffset,
    input [V_OFF_DWIDTH - 1 : 0]    front_active_v_roffset,
    input                           front_offset_valid,
    input [V_VALUE_WIDTH - 1 : 0]   front_active_v_value,
    input                           front_active_v_value_valid,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] front_iteration_id,
    input                           combine_stage_full,

    output                          stage_full,
    output reg [HBM_AWIDTH - 1 : 0]     rd_hbm_edge_addr,
    output reg [HBM_EDGE_MASK - 1 : 0]  rd_hbm_edge_mask,
    output reg [V_ID_WIDTH - 1 : 0]     hbm_active_v_id,
    output reg                          rd_hbm_edge_valid,
    output [V_VALUE_WIDTH - 1 : 0]      hbm_active_v_value,
    output                              hbm_active_v_value_valid,
    output reg                          iteration_end,
    output reg                          iteration_end_valid,
    output reg                          iteration_id
);

    wire active_v_id_buffer_empty, active_v_id_buffer_full;
    wire active_v_value_buffer_empty, active_v_value_buffer_full;
    wire active_v_offset_buffer_empty, active_v_offset_buffer_full;
    reg [V_OFF_DWIDTH - 1 : 0] now_loffset;
    wire [V_OFF_DWIDTH - 1 : 0] active_v_loffset_top, active_v_roffset_top;
    wire [V_ID_WIDTH - 1 : 0] active_v_id_top;
    wire [V_VALUE_WIDTH - 1 : 0] active_v_value_top;
    wire read_signal;

    reg [V_VALUE_WIDTH - 1 : 0] tmp_v_degree [0 : 8]; // delay 2 cycle
    reg tmp_v_degree_valid [0 : 1];
    wire [V_VALUE_WIDTH - 1 : 0] tmp_v_value;
    wire tmp_v_value_valid;

    assign stage_full = active_v_id_buffer_full;
    assign read_signal = !active_v_offset_buffer_empty && !active_v_value_buffer_empty && !combine_stage_full &&
        ((active_v_loffset_top == active_v_roffset_top + 1) ||
        ((now_loffset >> CACHELINE_LEN_WIDTH > active_v_roffset_top >> CACHELINE_LEN_WIDTH) && (active_v_loffset_top >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH)) ||
        (now_loffset >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH));

    active_v_id_fifo_ft ACTIVE_V_ID_FIFO_FT (
    // custom_fifo_ft #(.FIFO_DWIDTH(20), .FIFO_AWIDTH(2)) ACTIVE_V_ID_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_id),
        .wr_en      (front_active_v_valid),
        .rd_en      (read_signal),
        .dout       (active_v_id_top),
        .prog_full  (active_v_id_buffer_full),
        .full       (),
        .empty      (active_v_id_buffer_empty),
        .valid      ()
    );

    integer i;
    always @ (posedge clk) begin
        tmp_v_degree[0] <= active_v_roffset_top - active_v_loffset_top + 1;
        tmp_v_degree_valid[0] = read_signal;
        for (i = 0; i < 7; i = i + 1) begin
            tmp_v_degree[i + 1] <= tmp_v_degree[i];
            tmp_v_degree_valid[i + 1] <= tmp_v_degree_valid[i];
        end
    end

    float_multiply VALUE_MULTIPLY (
        .aclk       (clk),
        .s_axis_a_tdata         (active_v_value_top),
        .s_axis_a_tvalid        (read_signal),
        .s_axis_b_tdata         (DAMPING),
        .s_axis_b_tvalid        (read_signal),
        .m_axis_result_tdata    (tmp_v_value),
        .m_axis_result_tvalid   (tmp_v_value_valid)
    );

    float_divider VALUE_DIVIDER (
        .aclk       (clk),
        .s_axis_a_tdata         (tmp_v_value),
        .s_axis_a_tvalid        (tmp_v_value_valid),
        .s_axis_b_tdata         (tmp_v_degree[8]),
        .s_axis_b_tvalid        (tmp_v_degree_valid[8]),
        .m_axis_result_tdata    (hbm_active_v_value),
        .m_axis_result_tvalid   (hbm_active_v_value_valid)
    );

    active_v_value_fifo_ft ACTIVE_V_VALUE_FIFO_FT (
    // custom_fifo_ft #(.FIFO_DWIDTH(20), .FIFO_AWIDTH(2)) ACTIVE_V_VALUE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_value),
        .wr_en      (front_active_v_value_valid),
        .rd_en      (read_signal),
        .dout       (active_v_value_top),
        .full       (),
        .empty      (active_v_value_buffer_empty),
        .valid      ()
    );

    active_v_offset_fifo_ft ACTIVE_V_LOFFSET_FIFO_FT (
    // custom_fifo_ft #(.FIFO_DWIDTH(22), .FIFO_AWIDTH(2)) ACTIVE_V_LOFFSET_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_loffset),
        .wr_en      (front_offset_valid),
        .rd_en      (read_signal),
        .dout       (active_v_loffset_top),
        .prog_full  (active_v_offset_buffer_full),
        .full       (),
        .empty      (active_v_offset_buffer_empty),
        .valid      ()
    );

    active_v_offset_fifo_ft ACTIVE_V_ROFFSET_FIFO_FT (
    // custom_fifo_ft #(.FIFO_DWIDTH(22), .FIFO_AWIDTH(2)) ACTIVE_V_ROFFSET_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_active_v_roffset),
        .wr_en      (front_offset_valid),
        .rd_en      (read_signal),
        .dout       (active_v_roffset_top),
        .full       (),
        .empty      (),
        .valid      ()
    );

    always @ (posedge clk) begin
        if (rst) begin
            now_loffset <= {V_OFF_DWIDTH{1'b1}};

            rd_hbm_edge_addr <= 0;
            rd_hbm_edge_mask <= 0;
            hbm_active_v_id <= 0;
            rd_hbm_edge_valid <= 0;
        end else begin
            if (active_v_offset_buffer_empty || active_v_value_buffer_empty) begin
                now_loffset <= {V_OFF_DWIDTH{1'b1}};

                rd_hbm_edge_addr <= 0;
                rd_hbm_edge_mask <= 0;
                hbm_active_v_id <= 0;
                rd_hbm_edge_valid <= 0;
            end else begin
                if (combine_stage_full) begin
                    rd_hbm_edge_addr <= 0;
                    rd_hbm_edge_mask <= 0;
                    hbm_active_v_id <= 0;
                    rd_hbm_edge_valid <= 0;
                end else begin
                    if (active_v_loffset_top == active_v_roffset_top + 1) begin
                        rd_hbm_edge_addr <= 0;
                        rd_hbm_edge_mask <= 0;
                        hbm_active_v_id <= 0;
                        rd_hbm_edge_valid <= 0;
                        now_loffset <= {V_OFF_DWIDTH{1'b1}};
                    end else if (now_loffset >> CACHELINE_LEN_WIDTH > active_v_roffset_top >> CACHELINE_LEN_WIDTH) begin
                        rd_hbm_edge_addr <= active_v_loffset_top >> CACHELINE_LEN_WIDTH;
                        for (i = 0; i < CACHELINE_LEN; i = i + 1) begin
                            if (i < active_v_loffset_top[CACHELINE_LEN_WIDTH - 1 : 0] || (i > active_v_roffset_top[CACHELINE_LEN_WIDTH - 1 : 0] && active_v_loffset_top >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH)) begin
                                rd_hbm_edge_mask[i] <= 0;
                            end else begin
                                rd_hbm_edge_mask[i] <= 1;
                            end
                        end
                        hbm_active_v_id <= active_v_id_top;
                        rd_hbm_edge_valid <= active_v_loffset_top < active_v_roffset_top;

                        if (active_v_loffset_top >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH) begin
                            now_loffset <= {V_OFF_DWIDTH{1'b1}};
                        end else begin
                            now_loffset <= ((active_v_loffset_top >> CACHELINE_LEN_WIDTH) + 1) << CACHELINE_LEN_WIDTH;
                        end
                    end else begin
                        rd_hbm_edge_addr <= now_loffset >> CACHELINE_LEN_WIDTH;
                        for (i = 0; i < CACHELINE_LEN; i = i + 1) begin
                            if (i < now_loffset[CACHELINE_LEN_WIDTH - 1 : 0] || (i > active_v_roffset_top[CACHELINE_LEN_WIDTH - 1 : 0] && now_loffset >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH)) begin
                                rd_hbm_edge_mask[i] <= 0;
                            end else begin
                                rd_hbm_edge_mask[i] <= 1;
                            end
                        end
                        hbm_active_v_id <= active_v_id_top;
                        rd_hbm_edge_valid <= now_loffset < active_v_roffset_top;

                        if (now_loffset >> CACHELINE_LEN_WIDTH == active_v_roffset_top >> CACHELINE_LEN_WIDTH) begin
                            now_loffset <= {V_OFF_DWIDTH{1'b1}};
                        end else begin
                            now_loffset <= ((now_loffset >> CACHELINE_LEN_WIDTH) + 1) << CACHELINE_LEN_WIDTH;
                        end
                    end
                end
            end
        end
    end

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