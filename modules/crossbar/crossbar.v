module top #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,

    output reg [31 : 0] out,
    output reg valid
);

    reg [32 * PIPELINE_NUM - 1 : 0] data;
    reg [PIPELINE_NUM - 1 : 0] data_valid;
    reg [PIPELINE_NUM - 1: 0] arbitration_read;
    wire [32 * PIPELINE_NUM - 1 : 0] data_out;
    wire [PIPELINE_NUM - 1 : 0] data_valid_out;
    reg [4 : 0] select;
    wire [31 : 0] fifo_out [0 : PIPELINE_NUM - 1];
    wire fifo_empty [0 : PIPELINE_NUM - 1];

    always @ (posedge clk) begin
        if (rst) begin
            out <= 0;
            valid <= 0;
        end else begin
            out <= fifo_out[select];
            valid <= !fifo_empty[select];
        end
    end

    generate
        genvar i;
        for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                if (rst) begin
                    data[(i + 1) * 32 - 1 : i * 32] <= i;
                    data_valid[i] <= 0;
                    arbitration_read[i] <= 1;
                    select <= 0;
                end else begin
                    if (arbitration_read[i]) begin
                        data[(i + 1) * 32 - 1 : i * 32] <= data[(i + 1) * 32 - 1 : i * 32] + 10;
                    end
                    data_valid[i] <= 1;
                    select <= select + 1;
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
        end
    endgenerate

    crossbar_array CROSSBAR_ARRAY (
        .data_in (data),
        .data_valid_in (data_valid),

        .data_out (data_out),
        .data_valid_out (data_valid_out)
    );

endmodule

module top3 #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,

    output reg [31 : 0] out,
    output reg valid
);



endmodule


module top2 #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,
    input [31 : 0] dividend,
    input [31 : 0] divisor,

    output reg [31 : 0] out,
    output reg valid
);
    reg [31 : 0] dividend_reg, divisor_reg;
    wire [31 : 0] data_out;
    wire valid_out;
    
    always @ (posedge clk) begin
        if (rst) begin
            out <= 0;
            valid <= 0;
            
            dividend_reg <= dividend;
            divisor_reg <= divisor;
        end else begin
            out <= data_out;
            valid <= valid_out;
        end
    end

    div_gen_0 DIVIDER (
        .aclk       (clk),
        .s_axis_dividend_tdata  (dividend_reg),
        .s_axis_dividend_tvalid (1'b1),
        .s_axis_divisor_tdata   (divisor_reg),
        .s_axis_divisor_tvalid  (1'b1),
        .m_axis_dout_tdata      (data_out),
        .m_axis_dout_tvalid     (valid_out)
    );

endmodule


module crossbar_select #(parameter
    PIPELINE_NUM = 32
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    generate
        genvar i;
        for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
            crossbar_select_node #(.NODE_ID(i)) CROSSBAR_SELECT_NODE (
                .clk (clk),
                .rst (rst),
                .data_in (data_in),
                .data_valid_in (data_valid_in),

                .data_out (data_out[(i + 1) * 32 - 1 : i * 32]),
                .data_valid_out (data_valid_out[i])
            );
        end
    endgenerate

endmodule

module crossbar_select_node #(parameter
    PIPELINE_NUM = 32,
    NODE_ID = 0,
    SELECT_WIDTH = 5
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output reg [31 : 0] data_out,
    output reg data_valid_out
);
    wire [PIPELINE_NUM - 1 : 0] select;
    generate
        genvar i;
        for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
            assign select[i] = data_valid_in[i] && (data_in[32 * i + SELECT_WIDTH - 1 : 32 * i] == NODE_ID);
        end
    endgenerate

    always @ (posedge clk) begin
        if (rst) begin
            data_out <= 0;
            data_valid_out <= 0;
        end else begin
            casex (select)
                32'b00000000000000000000000000000000: begin
                    data_out <= 32'h0;
                    data_valid_out <= 1'b0;
                end
                32'b00000000000000000000000000000001: begin
                    data_out <= data_in[32 * 1 - 1 : 32 * 0];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000000000000000000001x: begin
                    data_out <= data_in[32 * 2 - 1 : 32 * 1];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000000000000000000001xx: begin
                    data_out <= data_in[32 * 3 - 1 : 32 * 2];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000000000000000000001xxx: begin
                    data_out <= data_in[32 * 4 - 1 : 32 * 3];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000000000000000001xxxx: begin
                    data_out <= data_in[32 * 5 - 1 : 32 * 4];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000000000000000001xxxxx: begin
                    data_out <= data_in[32 * 6 - 1 : 32 * 5];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000000000000000001xxxxxx: begin
                    data_out <= data_in[32 * 7 - 1 : 32 * 6];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000000000000001xxxxxxx: begin
                    data_out <= data_in[32 * 8 - 1 : 32 * 7];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000000000000001xxxxxxxx: begin
                    data_out <= data_in[32 * 9 - 1 : 32 * 8];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000000000000001xxxxxxxxx: begin
                    data_out <= data_in[32 * 10 - 1 : 32 * 9];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000000000001xxxxxxxxxx: begin
                    data_out <= data_in[32 * 11 - 1 : 32 * 10];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000000000001xxxxxxxxxxx: begin
                    data_out <= data_in[32 * 12 - 1 : 32 * 11];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000000000001xxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 13 - 1 : 32 * 12];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000000001xxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 14 - 1 : 32 * 13];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000000001xxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 15 - 1 : 32 * 14];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000000001xxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 16 - 1 : 32 * 15];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000000001xxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 17 - 1 : 32 * 16];
                    data_valid_out <= 1'b1;
                end
                32'b000000000000001xxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 18 - 1 : 32 * 17];
                    data_valid_out <= 1'b1;
                end
                32'b00000000000001xxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 19 - 1 : 32 * 18];
                    data_valid_out <= 1'b1;
                end
                32'b0000000000001xxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 20 - 1 : 32 * 19];
                    data_valid_out <= 1'b1;
                end
                32'b000000000001xxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 21 - 1 : 32 * 20];
                    data_valid_out <= 1'b1;
                end
                32'b00000000001xxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 22 - 1 : 32 * 21];
                    data_valid_out <= 1'b1;
                end
                32'b0000000001xxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 23 - 1 : 32 * 22];
                    data_valid_out <= 1'b1;
                end
                32'b000000001xxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 24 - 1 : 32 * 23];
                    data_valid_out <= 1'b1;
                end
                32'b00000001xxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 25 - 1 : 32 * 24];
                    data_valid_out <= 1'b1;
                end
                32'b0000001xxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 26 - 1 : 32 * 25];
                    data_valid_out <= 1'b1;
                end
                32'b000001xxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 27 - 1 : 32 * 26];
                    data_valid_out <= 1'b1;
                end
                32'b00001xxxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 28 - 1 : 32 * 27];
                    data_valid_out <= 1'b1;
                end
                32'b0001xxxxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 29 - 1 : 32 * 28];
                    data_valid_out <= 1'b1;
                end
                32'b001xxxxxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 30 - 1 : 32 * 29];
                    data_valid_out <= 1'b1;
                end
                32'b01xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 31 - 1 : 32 * 30];
                    data_valid_out <= 1'b1;
                end
                32'b1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx: begin
                    data_out <= data_in[32 * 32 - 1 : 32 * 31];
                    data_valid_out <= 1'b1;
                end
            endcase
        end
    end

endmodule

// 32*32的矩阵crossbar
module crossbar_array #(parameter
    PIPELINE_NUM = 32
) (
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out,
    output [PIPELINE_NUM * PIPELINE_NUM - 1 : 0] arbitration_read
);

    wire [31 : 0] right_out [0 : PIPELINE_NUM * PIPELINE_NUM - 1];
    wire right_out_valid [0 : PIPELINE_NUM * PIPELINE_NUM - 1];
    wire [31 : 0] down_out [0 : PIPELINE_NUM * PIPELINE_NUM - 1];
    wire down_out_valid [0 : PIPELINE_NUM * PIPELINE_NUM - 1];

    generate
        genvar i, j;
        for (i = 0; i < PIPELINE_NUM; i = i + 1) begin
            for (j = 0; j < PIPELINE_NUM; j = j + 1) begin
                crossbar_array_node #(.NODE_ID(j)) CROSSBAR_NODE (
                    .left_in (j == 32'h0 ? data_in[32 * (i + 1) - 1 : 32 * i] : right_out[i * PIPELINE_NUM + j - 1]),
                    .left_in_valid (j == 1'b0 ? data_valid_in[i] : right_out_valid[i * PIPELINE_NUM + j - 1]),
                    .up_in (i == 0 ? 32'h0 : down_out[(i - 1) * PIPELINE_NUM + j]),
                    .up_in_valid (i == 0 ? 1'b0 : down_out_valid[(i - 1) * PIPELINE_NUM + j]),

                    .right_out (right_out[i * PIPELINE_NUM + j]),
                    .right_out_valid (right_out_valid[i * PIPELINE_NUM + j]),
                    .down_out (down_out[i * PIPELINE_NUM + j]),
                    .down_out_valid (down_out_valid[i * PIPELINE_NUM + j])
                );
                assign arbitration_read[i * PIPELINE_NUM + j] = down_out_valid[i * PIPELINE_NUM + j];
            end
            assign data_out[32 * (i + 1) - 1 : 32 * i] = down_out[PIPELINE_NUM * (PIPELINE_NUM - 1) + i];
            assign data_valid_out[i] = down_out_valid[PIPELINE_NUM * (PIPELINE_NUM - 1) + i];
        end
    endgenerate

endmodule

module crossbar_array_node #(parameter
    NODE_ID = 0,
    SELECT_WIDTH = 5
) (
    input [31 : 0] left_in,
    input left_in_valid,
    input [31 : 0] up_in,
    input up_in_valid,

    output [31 : 0] right_out,
    output right_out_valid,
    output [31 : 0] down_out,
    output down_out_valid
);

    assign right_out = (left_in_valid && left_in[SELECT_WIDTH - 1 : 0] != NODE_ID) ? left_in : 0;
    assign right_out_valid = (left_in_valid && left_in[SELECT_WIDTH - 1 : 0] != NODE_ID);
    assign down_out = up_in_valid ? up_in : ((left_in_valid && left_in[SELECT_WIDTH - 1 : 0] == NODE_ID) ? left_in : 0);
    assign down_out_valid = up_in_valid || (left_in_valid && left_in[SELECT_WIDTH - 1 : 0] == NODE_ID);

endmodule