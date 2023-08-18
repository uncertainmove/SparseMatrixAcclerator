module top #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,

    output reg [31 : 0] out,
    output reg [31 : 0] addrout,
    output reg valid
);

    reg [32 * PIPELINE_NUM - 1 : 0] data;
    reg [32 * PIPELINE_NUM - 1 : 0] addr;
    reg [PIPELINE_NUM - 1 : 0] data_valid;
    wire [32 * PIPELINE_NUM - 1 : 0] bitonic_data_out;
    wire [32 * PIPELINE_NUM - 1 : 0] bitonic_addr_out;
    wire [PIPELINE_NUM - 1 : 0] bitonic_data_valid_out;
    wire [32 * PIPELINE_NUM - 1 : 0] data_out;
    wire [32 * PIPELINE_NUM - 1 : 0] addr_out;
    wire [PIPELINE_NUM - 1 : 0] data_valid_out;
    reg [4 : 0] select;
    wire [31 : 0] fifo_out [0 : PIPELINE_NUM - 1];
    wire [31 : 0] fifo_out_2 [0 : PIPELINE_NUM - 1];
    wire fifo_empty [0 : PIPELINE_NUM - 1];

    always @ (posedge clk) begin
        if (rst) begin
            out <= 0;
            addrout <= 0;
            valid <= 0;
            select <= 0;
        end else begin
            out <= fifo_out[select];
            addrout <= fifo_out_2[select];
            valid <= !fifo_empty[select];
            select <= select + 1;
        end
    end

    generate
        genvar i;
        for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                if (rst) begin
                    data[(i + 1) * 32 - 1 : i * 32] <= i + 1;
                    addr[(i + 1) * 32 - 1 : i * 32] <= i;
                    data_valid[i] <= 0;
                end else begin
                    data[(i + 1) * 32 - 1 : i * 32] <= data[(i + 1) * 32 - 1 : i * 32] + 1;
                    addr[(i + 1) * 32 - 1 : i * 32] <= addr[(i + 1) * 32 - 1 : i * 32] + 1;
                    data_valid[i] <= addr[(i + 1) * 32 - 1 : i * 32] > 16384 ? 1'b0 : 1'b1;
                end
            end

            fifo_out_ip OUT_FIFO (
                .clk        (clk),
                .srst       (rst),
                .din        (data_out[32 * (i + 1) - 1 : 32 * i]),
                .wr_en      (data_valid_out[i]),
                .rd_en      (select == i && !fifo_empty[i]),
                .dout       (fifo_out[i]),
                .full       (),
                .empty      (fifo_empty[i])
            );

            fifo_out_ip ADDR_OUT_FIFO (
                .clk        (clk),
                .srst       (rst),
                .din        (addr_out[32 * (i + 1) - 1 : 32 * i]),
                .wr_en      (data_valid_out[i]),
                .rd_en      (select == i && !fifo_empty[i]),
                .dout       (fifo_out_2[i]),
                .full       (),
                .empty      ()
            );
        end
    endgenerate

    bitonic_network_32 BITONIC_NEWTORK_32 (
        .clk(clk),
        .rst(rst),
        .data_in (data),
        .addr_in (addr),
        .data_valid_in (data_valid),

        .data_out (bitonic_data_out),
        .addr_out (bitonic_addr_out),
        .data_valid_out (bitonic_data_valid_out)
    );

    accumulator_32 ACCUMULATOR_32 (
        .clk(clk),
        .rst(rst),
        .data_in(bitonic_data_out),
        .addr_in(bitonic_addr_out),
        .data_valid_in(bitonic_data_valid_out),

        .data_out(data_out),
        .addr_out(addr_out),
        .data_valid_out(data_valid_out)
    );

endmodule

module accumulator_32 #(parameter
    PIPELINE_NUM = 32,
    VERTEX_BRAM_DWIDTH = 32,
    ACC_ID_WIDTH = 32,
    EDGE_PIPE_NUM = 32,
    TOT_ACC_ID_WIDTH = 32 * 32
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [32 * PIPELINE_NUM - 1 : 0] addr_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [32 * PIPELINE_NUM - 1 : 0] addr_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    wire [VERTEX_BRAM_DWIDTH * EDGE_PIPE_NUM - 1 : 0]   src_level_1;
    wire [VERTEX_BRAM_DWIDTH * EDGE_PIPE_NUM - 1 : 0]   src_level_2;
    wire [VERTEX_BRAM_DWIDTH * EDGE_PIPE_NUM - 1 : 0]   src_level_3;
    wire [VERTEX_BRAM_DWIDTH * EDGE_PIPE_NUM - 1 : 0]   src_level_4;
    wire [VERTEX_BRAM_DWIDTH * EDGE_PIPE_NUM - 1 : 0]   src_level_5;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_1;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_2;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_3;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_4;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_5;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_1_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_2_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_3_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_4_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_5_valid;

    assign data_out = src_level_5;
    assign addr_out = acc_id_level_5;
    assign data_valid_out = src_level_5_valid;

    // level 5
    generate
        genvar i;
        genvar j;

        for (i = 0; i < EDGE_PIPE_NUM; i = i + 32) begin : M14_BLOCK_6
            for (j = 0; j < 16; j = j + 1) begin : M14_BLOCK_7
                parallel_accumulator_update_reg L5_R (
                    .clk(clk), .rst(rst),
                    .din(src_level_4[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin(acc_id_level_4[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin(src_level_4_valid[i + j]),

                    .dout(src_level_5[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_5[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_5_valid[i + j])
                );
            end
        end

        for (i = 16; i < EDGE_PIPE_NUM; i = i + 32) begin : M14_BLOCK_8
            for (j = 0; j < 16; j = j + 1) begin : M14_BLOCK_9
                parallel_accumulator_update_addr L5_A (
                    .clk(clk), .rst(rst),
                    .din_1(src_level_4[(i - 1 + 1) * VERTEX_BRAM_DWIDTH - 1 : (i - 1) * VERTEX_BRAM_DWIDTH]), .idin_1(acc_id_level_4[(i - 1 + 1) * ACC_ID_WIDTH - 1 : (i - 1) * ACC_ID_WIDTH]), .validin_1(src_level_4_valid[i - 1]),
                    .din_2(src_level_4[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin_2(acc_id_level_4[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin_2(src_level_4_valid[i + j]),

                    .dout(src_level_5[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_5[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_5_valid[i + j])
                );
            end
        end

    endgenerate

    // level 4
    generate
        for (i = 0; i < EDGE_PIPE_NUM; i = i + 16) begin : M14_BLOCK_10
            for (j = 0; j < 8; j = j + 1) begin : M14_BLOCK_11
                parallel_accumulator_update_reg L4_R (
                    .clk(clk), .rst(rst),
                    .din(src_level_3[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin(acc_id_level_3[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin(src_level_3_valid[i + j]),

                    .dout(src_level_4[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_4[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_4_valid[i + j])
                );
            end
        end

        for (i = 8; i < EDGE_PIPE_NUM; i = i + 16) begin : M14_BLOCK_12
            for (j = 0; j < 8; j = j + 1) begin : M14_BLOCK_13
                parallel_accumulator_update_addr L4_A (
                    .clk(clk), .rst(rst),
                    .din_1(src_level_3[(i - 1 + 1) * VERTEX_BRAM_DWIDTH - 1 : (i - 1) * VERTEX_BRAM_DWIDTH]), .idin_1(acc_id_level_3[(i - 1 + 1) * ACC_ID_WIDTH - 1 : (i - 1) * ACC_ID_WIDTH]), .validin_1(src_level_3_valid[i - 1]),
                    .din_2(src_level_3[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin_2(acc_id_level_3[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin_2(src_level_3_valid[i + j]),

                    .dout(src_level_4[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_4[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_4_valid[i + j])
                );
            end
        end

    endgenerate

    // level 3
    generate
        for (i = 0; i < EDGE_PIPE_NUM; i = i + 8) begin : M14_BLOCK_14
            for (j = 0; j < 4; j = j + 1) begin : M14_BLOCK_15
                parallel_accumulator_update_reg L3_R (
                    .clk(clk), .rst(rst),
                    .din(src_level_2[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin(acc_id_level_2[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin(src_level_2_valid[i + j]),

                    .dout(src_level_3[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_3[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_3_valid[i + j])
                );
            end
        end

        for (i = 4; i < EDGE_PIPE_NUM; i = i + 8) begin : M14_BLOCK_16
            for (j = 0; j < 4; j = j + 1) begin : M14_BLOCK_17
                parallel_accumulator_update_addr L3_A (
                    .clk(clk), .rst(rst),
                    .din_1(src_level_2[(i - 1 + 1) * VERTEX_BRAM_DWIDTH - 1 : (i - 1) * VERTEX_BRAM_DWIDTH]), .idin_1(acc_id_level_2[(i - 1 + 1) * ACC_ID_WIDTH - 1 : (i - 1) * ACC_ID_WIDTH]), .validin_1(src_level_2_valid[i - 1]),
                    .din_2(src_level_2[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idin_2(acc_id_level_2[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]), .validin_2(src_level_2_valid[i + j]),

                    .dout(src_level_3[(i + j + 1) * VERTEX_BRAM_DWIDTH - 1 : (i + j) * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_3[(i + j + 1) * ACC_ID_WIDTH - 1 : (i + j) * ACC_ID_WIDTH]),
                    .validout(src_level_3_valid[i + j])
                );
            end
        end

    endgenerate

    // level 2
    generate
        for (i = 0; i < EDGE_PIPE_NUM; i = i + 4) begin: M14_BLOCK_18
            parallel_accumulator_update_reg L2_R (
                .clk(clk), .rst(rst),
                .din(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin(src_level_1_valid[i]),

                .dout(src_level_2[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_2[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_2_valid[i])
            );
        end
        for (i = 1; i < EDGE_PIPE_NUM; i = i + 4) begin: M14_BLOCK_19
            parallel_accumulator_update_reg L2_R (
                .clk(clk), .rst(rst),
                .din(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin(src_level_1_valid[i]),

                .dout(src_level_2[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_2[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_2_valid[i])
            );
        end
    endgenerate

    generate
        for (i = 2; i < EDGE_PIPE_NUM; i = i + 4) begin: M14_BLOCK_20
            parallel_accumulator_update_addr L2_A (
                .clk(clk), .rst(rst),
                .din_1(src_level_1[i * VERTEX_BRAM_DWIDTH - 1 : (i - 1) * VERTEX_BRAM_DWIDTH]), .idin_1(acc_id_level_1[i * ACC_ID_WIDTH - 1 : (i - 1) * ACC_ID_WIDTH]), .validin_1(src_level_1_valid[i - 1]),
                .din_2(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin_2(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin_2(src_level_1_valid[i]),

                .dout(src_level_2[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_2[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_2_valid[i])
            );
        end
        for (i = 3; i < EDGE_PIPE_NUM; i = i + 4) begin: M14_BLOCK_21
            parallel_accumulator_update_addr L2_A (
                .clk(clk), .rst(rst),
                .din_1(src_level_1[(i - 1) * VERTEX_BRAM_DWIDTH - 1 : (i - 2) * VERTEX_BRAM_DWIDTH]), .idin_1(acc_id_level_1[(i - 1) * ACC_ID_WIDTH - 1 : (i - 2) * ACC_ID_WIDTH]), .validin_1(src_level_1_valid[i - 2]),
                .din_2(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin_2(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin_2(src_level_1_valid[i]),

                .dout(src_level_2[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_2[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_2_valid[i])
            );
        end
    endgenerate

    // level 1

    generate
        for (i = 0; i < EDGE_PIPE_NUM; i = i + 2) begin: M14_BLOCK_22
            parallel_accumulator_update_reg L1_R (
                .clk(clk), .rst(rst),
                .din(data_in[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin(addr_in[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin(data_valid_in[i]),

                .dout(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_1_valid[i])
            );
        end
    endgenerate

    generate
        for (i = 1; i < EDGE_PIPE_NUM; i = i + 2) begin: M14_BLOCKl_23
            parallel_accumulator_update_addr L2_A (
                .clk(clk), .rst(rst),
                .din_1(data_in[i * VERTEX_BRAM_DWIDTH - 1 : (i - 1) * VERTEX_BRAM_DWIDTH]), .idin_1(addr_in[i * ACC_ID_WIDTH - 1 : (i - 1) * ACC_ID_WIDTH]), .validin_1(data_valid_in[i - 1]),
                .din_2(data_in[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idin_2(addr_in[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]), .validin_2(data_valid_in[i]),

                .dout(src_level_1[(i + 1) * VERTEX_BRAM_DWIDTH - 1 : i * VERTEX_BRAM_DWIDTH]), .idout(acc_id_level_1[(i + 1) * ACC_ID_WIDTH - 1 : i * ACC_ID_WIDTH]),
                .validout(src_level_1_valid[i])
            );
        end
    endgenerate

endmodule

module parallel_accumulator_update_reg #(parameter
    VERTEX_BRAM_DWIDTH = 32,
    ACC_ID_WIDTH = 32
    ) (
    input                                   clk,
    input                                   rst,
    input [VERTEX_BRAM_DWIDTH - 1 : 0]      din,
    input [ACC_ID_WIDTH - 1 : 0]            idin,
    input                                   validin,

    output reg [VERTEX_BRAM_DWIDTH - 1 : 0] dout,
    output reg [ACC_ID_WIDTH - 1 : 0]       idout,
    output reg                              validout);

    always @ (posedge clk) begin
        if (rst) begin
            dout    <= 0;
            idout   <= 0;
            validout <= 0;
        end
        else begin
            dout    <= din;
            idout   <= idin;
            validout <= validin;
        end
    end
endmodule

module parallel_accumulator_update_addr #(parameter
    VERTEX_BRAM_DWIDTH = 32,
    ACC_ID_WIDTH = 32
) (
    input                                   clk,
    input                                   rst,
    input [VERTEX_BRAM_DWIDTH - 1 : 0]      din_1,
    input [ACC_ID_WIDTH - 1 : 0]            idin_1,
    input                                   validin_1,
    input [VERTEX_BRAM_DWIDTH - 1 : 0]      din_2,
    input [ACC_ID_WIDTH - 1 : 0]            idin_2,
    input                                   validin_2,

    output reg [VERTEX_BRAM_DWIDTH - 1 : 0] dout,
    output reg [ACC_ID_WIDTH - 1 : 0]       idout,
    output reg                              validout);

    always @ (posedge clk) begin
        if (rst) begin
            dout    <= 0;
            idout   <= 0;
            validout <= 0;
        end
        else begin
            if ((validin_1 && validin_2 && idin_1 == idin_2 && din_1 < din_2) || (validin_1 && !validin_2)) begin
                dout    <= din_1;
                idout   <= idin_1;
                validout <= validin_1;
            end
            else begin
                dout    <= din_2;
                idout   <= idin_2;
                validout <= validin_2;
            end
        end
    end

endmodule

// bitonic网络包含两层
// 1. 将无序的输入数据转化为双调序列
// 2. 将双调序列转化为升序序列
module bitonic_network_32 #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [32 * PIPELINE_NUM - 1 : 0] addr_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [32 * PIPELINE_NUM - 1 : 0] addr_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    wire [32 * PIPELINE_NUM - 1 : 0] data_out_level_1;
    wire [32 * PIPELINE_NUM - 1 : 0] addr_out_level_1;
    wire [PIPELINE_NUM - 1 : 0] data_valid_out_level_1;

    bitonic_network_unorder_to_bitonic_32 UNORDER_TO_BITONIC_32 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_out_level_1),
        .addr_out   (addr_out_level_1),
        .data_valid_out (data_valid_out_level_1)
    );

    bitonic_network_bitonic_to_ascending_32 BITONIC_TO_ASCENDING_32 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_out_level_1),
        .addr_in    (addr_out_level_1),
        .data_valid_in  (data_valid_out_level_1),

        .data_out   (data_out),
        .addr_out   (addr_out),
        .data_valid_out (data_valid_out)
    );


endmodule

module bitonic_network_unorder_to_bitonic_32 #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [32 * PIPELINE_NUM - 1 : 0] addr_in,
    input [PIPELINE_NUM - 1 : 0]    data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [32 * PIPELINE_NUM - 1 : 0] addr_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    wire [32 * PIPELINE_NUM - 1 : 0] data_level [0 : 9];
    wire [32 * PIPELINE_NUM - 1 : 0] addr_level [0 : 9];
    wire [PIPELINE_NUM - 1 : 0] data_valid_level [0 : 9];

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00110011001100110011001100110011), .STEP(1)) COMPARATOR_LEVEL_1_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_level[0]),
        .addr_out   (addr_level[0]),
        .data_valid_out (data_valid_level[0])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00001111000011110000111100001111), .STEP(2)) COMPARATOR_LEVEL_2_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[0]),
        .addr_in    (addr_level[0]),
        .data_valid_in  (data_valid_level[0]),

        .data_out   (data_level[1]),
        .addr_out   (addr_level[1]),
        .data_valid_out (data_valid_level[1])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00001111000011110000111100001111), .STEP(1)) COMPARATOR_LEVEL_2_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[1]),
        .addr_in    (addr_level[1]),
        .data_valid_in  (data_valid_level[1]),

        .data_out   (data_level[2]),
        .addr_out   (addr_level[2]),
        .data_valid_out (data_valid_level[2])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000111111110000000011111111), .STEP(4)) COMPARATOR_LEVEL_3_STEP_4 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[2]),
        .addr_in    (addr_level[2]),
        .data_valid_in  (data_valid_level[2]),

        .data_out   (data_level[3]),
        .addr_out   (addr_level[3]),
        .data_valid_out (data_valid_level[3])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000111111110000000011111111), .STEP(2)) COMPARATOR_LEVEL_3_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[3]),
        .addr_in    (addr_level[3]),
        .data_valid_in  (data_valid_level[3]),

        .data_out   (data_level[4]),
        .addr_out   (addr_level[4]),
        .data_valid_out (data_valid_level[4])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000111111110000000011111111), .STEP(1)) COMPARATOR_LEVEL_3_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[4]),
        .addr_in    (addr_level[4]),
        .data_valid_in  (data_valid_level[4]),

        .data_out   (data_level[5]),
        .addr_out   (addr_level[5]),
        .data_valid_out (data_valid_level[5])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000001111111111111111), .STEP(8)) COMPARATOR_LEVEL_4_STEP_8 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[5]),
        .addr_in    (addr_level[5]),
        .data_valid_in  (data_valid_level[5]),

        .data_out   (data_level[6]),
        .addr_out   (addr_level[6]),
        .data_valid_out (data_valid_level[6])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000001111111111111111), .STEP(4)) COMPARATOR_LEVEL_4_STEP_4 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[6]),
        .addr_in    (addr_level[6]),
        .data_valid_in  (data_valid_level[6]),

        .data_out   (data_level[7]),
        .addr_out   (addr_level[7]),
        .data_valid_out (data_valid_level[7])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000001111111111111111), .STEP(2)) COMPARATOR_LEVEL_4_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[7]),
        .addr_in    (addr_level[7]),
        .data_valid_in  (data_valid_level[7]),

        .data_out   (data_level[8]),
        .addr_out   (addr_level[8]),
        .data_valid_out (data_valid_level[8])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000001111111111111111), .STEP(1)) COMPARATOR_LEVEL_4_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[8]),
        .addr_in    (addr_level[8]),
        .data_valid_in  (data_valid_level[8]),

        .data_out   (data_level[9]),
        .addr_out   (addr_level[9]),
        .data_valid_out (data_valid_level[9])
    );

    assign data_out = data_level[9];
    assign addr_out = addr_level[9];
    assign data_valid_out = data_valid_level[9];

endmodule

module bitonic_network_bitonic_to_ascending_32 #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [32 * PIPELINE_NUM - 1 : 0] addr_in,
    input [PIPELINE_NUM - 1 : 0]    data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [32 * PIPELINE_NUM - 1 : 0] addr_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    wire [32 * PIPELINE_NUM - 1 : 0] data_level [0 : 7 - 1];
    wire [32 * PIPELINE_NUM - 1 : 0] addr_level [0 : 7 - 1];
    wire [PIPELINE_NUM - 1 : 0] data_valid_level [0 : 7 - 1];

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000000000000000000000), .STEP(16)) COMPARATOR_LEVEL_1_STEP_16 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_level[0]),
        .addr_out   (addr_level[0]),
        .data_valid_out (data_valid_level[0])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(32'b00000000000000000000000000000000), .STEP(8)) COMPARATOR_LEVEL_1_STEP_8 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[0]),
        .addr_in    (addr_level[0]),
        .data_valid_in  (data_valid_level[0]),

        .data_out   (data_level[1]),
        .addr_out   (addr_level[1]),
        .data_valid_out (data_valid_level[1])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b00000000000000000000000000000000), .STEP(4)) COMPARATOR_LEVEL_2_STEP_4 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[1]),
        .addr_in    (addr_level[1]),
        .data_valid_in  (data_valid_level[1]),

        .data_out   (data_level[2]),
        .addr_out   (addr_level[2]),
        .data_valid_out (data_valid_level[2])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b00000000000000000000000000000000), .STEP(2)) COMPARATOR_LEVEL_3_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[2]),
        .addr_in    (addr_level[2]),
        .data_valid_in  (data_valid_level[2]),

        .data_out   (data_level[3]),
        .addr_out   (addr_level[3]),
        .data_valid_out (data_valid_level[3])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b00000000000000000000000000000000), .STEP(1)) COMPARATOR_LEVEL_4_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[3]),
        .addr_in    (addr_level[3]),
        .data_valid_in  (data_valid_level[3]),

        .data_out   (data_level[4]),
        .addr_out   (addr_level[4]),
        .data_valid_out (data_valid_level[4])
    );

    assign data_out = data_level[4];
    assign addr_out = addr_level[4];
    assign data_valid_out = data_valid_level[4];

endmodule

module comparator_level_step_x #(parameter
    PIPELINE_NUM = 32,
    COMPARATOR_CH = {PIPELINE_NUM{1'b0}},
    STEP = 2
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [32 * PIPELINE_NUM - 1 : 0] addr_in,
    input [PIPELINE_NUM - 1 : 0]    data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [32 * PIPELINE_NUM - 1 : 0] addr_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    generate
        genvar i, j;
        for (i = 0; i < PIPELINE_NUM; i = i + 2 * STEP) begin
            for (j = 0; j < STEP; j = j + 1) begin
                comparator #(.CH(COMPARATOR_CH[i + j])) COMPARATOR_STEP_X (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .addr_in_1          (addr_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + STEP + 1) * 32 - 1 : (i + j + STEP) * 32]),
                    .addr_in_2          (addr_in[(i + j + STEP + 1) * 32 - 1 : (i + j + STEP) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + STEP]),

                    .data_out_1         (data_out[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .addr_out_1         (addr_out[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_out[i + j]),
                    .data_out_2         (data_out[(i + j + STEP + 1) * 32 - 1 : (i + j + STEP) * 32]),
                    .addr_out_2         (addr_out[(i + j + STEP + 1) * 32 - 1 : (i + j + STEP) * 32]),
                    .data_valid_out_2   (data_valid_out[i + j + STEP])
                );
            end
        end
    endgenerate

endmodule

// 无效信号默认最小
module comparator #(parameter
    DATA_WIDTH = 32,
    CH = 1'b0 // 0: ascending, 1: descending
) (
    input clk,
    input rst,
    input [DATA_WIDTH - 1 : 0] data_in_1,
    input [DATA_WIDTH - 1 : 0] addr_in_1,
    input data_valid_in_1,
    input [DATA_WIDTH - 1 : 0] data_in_2,
    input [DATA_WIDTH - 1 : 0] addr_in_2,
    input data_valid_in_2,

    output reg [DATA_WIDTH - 1 : 0] data_out_1,
    output reg [DATA_WIDTH - 1 : 0] addr_out_1,
    output reg data_valid_out_1,
    output reg [DATA_WIDTH - 1 : 0] data_out_2,
    output reg [DATA_WIDTH - 1 : 0] addr_out_2,
    output reg data_valid_out_2
);

    always @ (posedge clk) begin
        if (rst) begin
            data_out_1 <= 0;
            addr_out_1 <= 0;
            data_valid_out_1 <= 0;
            data_out_2 <= 0;
            addr_out_2 <= 0;
            data_valid_out_2 <= 0;
        end else begin
            if ((data_valid_in_1 && data_valid_in_2 && addr_in_2 >= addr_in_1 && !CH) || (data_valid_in_1 && !data_valid_in_2 && CH) || (!data_valid_in_1 && data_valid_in_2 && !CH)) begin
                data_out_1 <= data_in_1;
                addr_out_1 <= addr_in_1;
                data_valid_out_1 <= data_valid_in_1;
                data_out_2 <= data_in_2;
                addr_out_2 <= addr_in_2;
                data_valid_out_2 <= data_valid_in_2;
            end else begin
                data_out_1 <= data_in_2;
                addr_out_1 <= addr_in_2;
                data_valid_out_1 <= data_valid_in_2;
                data_out_2 <= data_in_1;
                addr_out_2 <= addr_in_1;
                data_valid_out_2 <= data_valid_in_1;
            end
            /*
            if (data_valid_in_1 && data_valid_in_2) begin
                if (data_in_2 >= data_in_1 && !CH) begin
                    data_out_1 <= data_in_1;
                    data_out_2 <= data_in_2;
                end else begin
                    data_out_1 <= data_in_2;
                    data_out_2 <= data_in_1;
                end
                data_valid_out_1 <= 1;
                data_valid_out_2 <= 1;
            end else if (data_valid_in_1) begin
                if (!CH) begin
                    data_out_1 <= data_in_2;
                    data_valid_out_1 <= data_valid_in_2;
                    data_out_2 <= data_in_1;
                    data_valid_out_2 <= data_valid_in_1;
                end else begin
                    data_out_1 <= data_in_1;
                    data_valid_out_1 <= data_valid_in_1;
                    data_out_2 <= data_in_2;
                    data_valid_out_2 <= data_valid_in_2;
                end
            end else if (data_valid_in_2) begin
                if (!CH) begin
                    data_out_1 <= data_in_1;
                    data_valid_out_1 <= data_valid_in_1;
                    data_out_2 <= data_in_2;
                    data_valid_out_2 <= data_valid_in_2;
                end else begin
                    data_out_1 <= data_in_2;
                    data_valid_out_1 <= data_valid_in_2;
                    data_out_2 <= data_in_1;
                    data_valid_out_2 <= data_valid_in_1;
                end
            end else begin
                data_out_1 <= 0;
                data_valid_out_1 <= 0;
                data_out_2 <= 0;
                data_valid_out_2 <= 0;
            end
            */
        end
    end

endmodule