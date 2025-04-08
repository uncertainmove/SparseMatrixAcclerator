`include "accelerator.vh"

module accu #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HORIZONTAL_NUM = `HORIZONTAL_NUM,
    PIPELINE_NUM = 16
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data,
    input [32 * PIPELINE_NUM - 1 : 0] addr,
    input [PIPELINE_NUM - 1 : 0] data_valid,
    input [HORIZONTAL_NUM * PIPELINE_NUM - 1 : 0] select,

    output [(V_VALUE_WIDTH+2) * PIPELINE_NUM - 1 : 0] out,
    output [V_ID_WIDTH * PIPELINE_NUM - 1 : 0] addrout,
    output [PIPELINE_NUM - 1 : 0] prog_full,
    output [PIPELINE_NUM - 1 : 0] empty,
    output [PIPELINE_NUM - 1 : 0] valid
);
   wire [32 * PIPELINE_NUM - 1 : 0] bitonic_data_out;
   wire [32 * PIPELINE_NUM - 1 : 0] bitonic_addr_out;
   wire [PIPELINE_NUM - 1 : 0] bitonic_data_valid_out;
   wire [32 * PIPELINE_NUM - 1 : 0] data_out;
   wire [32 * PIPELINE_NUM - 1 : 0] addr_out;
   wire [PIPELINE_NUM - 1 : 0] data_valid_out;
   wire [PIPELINE_NUM - 1 : 0] fifo_empty;
   wire [PIPELINE_NUM - 1 : 0] fifo_prog_full;
   assign prog_full = fifo_prog_full==0 ? {PIPELINE_NUM{1'b0}} : {PIPELINE_NUM{1'b1}};
   assign empty = fifo_empty;
   
   generate
       genvar i;
       for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
           custom_fifo_ft #(.FIFO_DWIDTH(V_VALUE_WIDTH+2), .FIFO_AWIDTH(5), .PROG_FULL_NUM(20)) OUT_FIFO (
               .clk        (clk),
               .srst       (rst),
               .din        (data_out[32 * (i + 1) - 1 : 32 * i]),
               .wr_en      (data_valid_out[i]),
               .rd_en      (!fifo_empty[i] && select[(i+1)*HORIZONTAL_NUM - 1 : i*HORIZONTAL_NUM]!=0),
               .dout       (out[(i+1)*(V_VALUE_WIDTH+2)-1 : i*(V_VALUE_WIDTH+2)]),
               .prog_full  (fifo_prog_full[i]),
               .empty      (fifo_empty[i]),
               .valid       (valid[i])
           );

           custom_fifo_ft #(.FIFO_DWIDTH(V_ID_WIDTH), .FIFO_AWIDTH(5), .PROG_FULL_NUM(20)) ADDR_OUT_FIFO (
               .clk        (clk),
               .srst       (rst),
               .din        (addr_out[32 * (i + 1) - 1 : 32 * i]),
               .wr_en      (data_valid_out[i]),
               .rd_en      (!fifo_empty[i] && select[(i+1)*HORIZONTAL_NUM - 1 : i*HORIZONTAL_NUM]!=0),
               .dout       (addrout[(i+1)*V_ID_WIDTH-1 : i*V_ID_WIDTH]),
               .full       (),
               .empty      ()
           );
       end
   endgenerate

   bitonic_network_16 BITONIC_NEWTORK_16 (
       .clk(clk),
       .rst(rst),
       .data_in (data),
       .addr_in (addr),
       .data_valid_in (data_valid),

       .data_out (bitonic_data_out),
       .addr_out (bitonic_addr_out),
       .data_valid_out (bitonic_data_valid_out)
   );

   accumulator_16 ACCUMULATOR_16 (
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

module accumulator_16 #(parameter
    PIPELINE_NUM = 16,
    VERTEX_BRAM_DWIDTH = 32,
    ACC_ID_WIDTH = 32,
    EDGE_PIPE_NUM = 16,
    TOT_ACC_ID_WIDTH = 32 * 16
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
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_1;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_2;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_3;
    wire [TOT_ACC_ID_WIDTH - 1 : 0]                     acc_id_level_4;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_1_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_2_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_3_valid;
    wire [PIPELINE_NUM - 1 : 0]                         src_level_4_valid;

    // level 4
    generate
        genvar i, j;
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

    assign data_out = src_level_4;
    assign addr_out = acc_id_level_4;
    
    generate
        for (i = 0; i < EDGE_PIPE_NUM; i = i + 1) begin
            if(i < EDGE_PIPE_NUM-1)
                assign data_valid_out[i] = src_level_4_valid[i] && acc_id_level_4[(i+1)*32-1 : i*32] != acc_id_level_4[(i+2)*32-1 : (i+1)*32];
            else
                assign data_valid_out[i] = src_level_4_valid[i];
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
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
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
            if (validin_1 && validin_2 && idin_1 == idin_2) begin
                dout[V_VALUE_WIDTH-1 : 0]    <= din_1[V_VALUE_WIDTH-1 : 0] + din_2[V_VALUE_WIDTH-1 : 0];
		        dout[VERTEX_BRAM_DWIDTH -1 : V_VALUE_WIDTH]    <= din_1;
                idout   <= idin_1;
                validout <= validin_1;
            end
            else if(validin_1 && !validin_2) begin
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
// 1. 将无序的输入数据转化为双调序
// 2. 将双调序列转化为升序序列
module bitonic_network_16 #(parameter
    PIPELINE_NUM = 16
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

    bitonic_network_unorder_to_bitonic_16 UNORDER_TO_BITONIC_16 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_out_level_1),
        .addr_out   (addr_out_level_1),
        .data_valid_out (data_valid_out_level_1)
    );

    bitonic_network_bitonic_to_ascending_16 BITONIC_TO_ASCENDING_16 (
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

module bitonic_network_unorder_to_bitonic_16 #(parameter
    PIPELINE_NUM = 16
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

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0011001100110011), .STEP(1)) COMPARATOR_LEVEL_1_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_level[0]),
        .addr_out   (addr_level[0]),
        .data_valid_out (data_valid_level[0])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000111100001111), .STEP(2)) COMPARATOR_LEVEL_2_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[0]),
        .addr_in    (addr_level[0]),
        .data_valid_in  (data_valid_level[0]),

        .data_out   (data_level[1]),
        .addr_out   (addr_level[1]),
        .data_valid_out (data_valid_level[1])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000111100001111), .STEP(1)) COMPARATOR_LEVEL_2_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[1]),
        .addr_in    (addr_level[1]),
        .data_valid_in  (data_valid_level[1]),

        .data_out   (data_level[2]),
        .addr_out   (addr_level[2]),
        .data_valid_out (data_valid_level[2])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000011111111), .STEP(4)) COMPARATOR_LEVEL_3_STEP_4 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[2]),
        .addr_in    (addr_level[2]),
        .data_valid_in  (data_valid_level[2]),

        .data_out   (data_level[3]),
        .addr_out   (addr_level[3]),
        .data_valid_out (data_valid_level[3])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000011111111), .STEP(2)) COMPARATOR_LEVEL_3_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[3]),
        .addr_in    (addr_level[3]),
        .data_valid_in  (data_valid_level[3]),

        .data_out   (data_level[4]),
        .addr_out   (addr_level[4]),
        .data_valid_out (data_valid_level[4])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000011111111), .STEP(1)) COMPARATOR_LEVEL_3_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[4]),
        .addr_in    (addr_level[4]),
        .data_valid_in  (data_valid_level[4]),

        .data_out   (data_level[5]),
        .addr_out   (addr_level[5]),
        .data_valid_out (data_valid_level[5])
    );

    assign data_out = data_level[5];
    assign addr_out = addr_level[5];
    assign data_valid_out = data_valid_level[5];

endmodule

module bitonic_network_bitonic_to_ascending_16 #(parameter
    PIPELINE_NUM = 16
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

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000000000000), .STEP(8)) COMPARATOR_LEVEL_1_STEP_8 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .addr_in    (addr_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_level[0]),
        .addr_out   (addr_level[0]),
        .data_valid_out (data_valid_level[0])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000000000000), .STEP(4)) COMPARATOR_LEVEL_2_STEP_4 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[0]),
        .addr_in    (addr_level[0]),
        .data_valid_in  (data_valid_level[0]),

        .data_out   (data_level[1]),
        .addr_out   (addr_level[1]),
        .data_valid_out (data_valid_level[1])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000000000000), .STEP(2)) COMPARATOR_LEVEL_3_STEP_2 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[1]),
        .addr_in    (addr_level[1]),
        .data_valid_in  (data_valid_level[1]),

        .data_out   (data_level[2]),
        .addr_out   (addr_level[2]),
        .data_valid_out (data_valid_level[2])
    );

    comparator_level_step_x #(.PIPELINE_NUM(PIPELINE_NUM), .COMPARATOR_CH(16'b0000000000000000), .STEP(1)) COMPARATOR_LEVEL_4_STEP_1 (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_level[2]),
        .addr_in    (addr_level[2]),
        .data_valid_in  (data_valid_level[2]),

        .data_out   (data_level[3]),
        .addr_out   (addr_level[3]),
        .data_valid_out (data_valid_level[3])
    );

    assign data_out = data_level[3];
    assign addr_out = addr_level[3];
    assign data_valid_out = data_valid_level[3];

endmodule

module comparator_level_step_x #(parameter
    PIPELINE_NUM = 16,
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

// 无效信号默认
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
            if ((data_valid_in_1 && data_valid_in_2 && ((addr_in_2 >= addr_in_1 && !CH) || (addr_in_2 <= addr_in_1 && CH))) ||
                    (data_valid_in_1 && !data_valid_in_2 && CH) || (!data_valid_in_1 && data_valid_in_2 && !CH)) begin
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
        end
    end

endmodule