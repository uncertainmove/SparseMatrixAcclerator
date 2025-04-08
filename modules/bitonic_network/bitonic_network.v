module top #(parameter
    PIPELINE_NUM = 16
) (
    input clk,
    input rst,

    output reg [31 : 0] out,
    output reg valid
);

    reg [32 * PIPELINE_NUM - 1 : 0] data;
    reg [PIPELINE_NUM - 1 : 0] data_valid;
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
                    select <= 0;
                end else begin
                    data[(i + 1) * 32 - 1 : i * 32] <= data[(i + 1) * 32 - 1 : i * 32] + 10;
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

// bitonic网络包含两层
// 1. 将无序的输入数据转化为双调序列
// 2. 将双调序列转化为升序序列
module bitonic_network #(parameter
    PIPELINE_NUM = 16
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [PIPELINE_NUM - 1 : 0] data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    wire [32 * PIPELINE_NUM - 1 : 0] data_out_level_1;
    wire [PIPELINE_NUM - 1 : 0] data_valid_out_level_1;

    bitonic_network_unorder_to_bitonic UNORDER_TO_BITONIC (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_out_level_1),
        .data_valid_out (data_valid_out_level_1)
    );

    bitonic_network_bitonic_to_ascending BITONIC_TO_ASCENDING (
        .clk        (clk),
        .rst        (rst),
        .data_in    (data_in),
        .data_valid_in  (data_valid_in),

        .data_out   (data_out),
        .data_valid_out (data_valid_out)
    );


endmodule

module bitonic_network_unorder_to_bitonic #(parameter
    PIPELINE_NUM = 16,
    PIPELINE_NUM_WIDTH = 4
) (
    input clk,
    input rst,
    input [32 * PIPELINE_NUM - 1 : 0] data_in,
    input [PIPELINE_NUM - 1 : 0]    data_valid_in,

    output [32 * PIPELINE_NUM - 1 : 0] data_out,
    output [PIPELINE_NUM - 1 : 0] data_valid_out
);

    reg [32 * PIPELINE_NUM - 1 : 0] data_level [0 : PIPELINE_NUM_WIDTH - 1];
    reg [PIPELINE_NUM - 1 : 0] data_valid_level [0 : PIPELINE_NUM_WIDTH - 1];

    generate
        genvar i, j;
        // level 0
        for (i = 0; i < PIPELINE_NUM; i = i + 4) begin
            // ascending
            ascending_comparator ASCENDING_COMPARATOR_LEVEL_0 (
                .clk                (clk),
                .rst                (rst),
                .data_in_1          (data_in[(i + 1) * 32 - 1 : i * 32]),
                .data_valid_in_1    (data_valid_in[i]),
                .data_in_2          (data_in[(i + 2) * 32 - 1 : (i + 1) * 32]),
                .data_valid_in_2    (data_valid_in[i + 1]),

                .data_out_1         (data_level[0][(i + 1) * 32 - 1 : i * 32]),
                .data_valid_out_1   (data_valid_level[0][i]),
                .data_out_2         (data_level[0][(i + 2) * 32 - 1 : (i + 1) * 32]),
                .data_valid_out_2   (data_valid_level[0][i + 1])
            );
        end
        for (i = 2; i < PIPELINE_NUM; i = i + 4) begin
            // descending
            descending_comparator DESCENDING_COMPARATOR_LEVEL_0 (
                .clk                (clk),
                .rst                (rst),
                .data_in_1          (data_in[(i + 1) * 32 - 1 : i * 32]),
                .data_valid_in_1    (data_valid_in[i]),
                .data_in_2          (data_in[(i + 2) * 32 - 1 : (i + 1) * 32]),
                .data_valid_in_2    (data_valid_in[i + 1]),

                .data_out_1         (data_level[0][(i + 1) * 32 - 1 : i * 32]),
                .data_valid_out_1   (data_valid_level[0][i]),
                .data_out_2         (data_level[0][(i + 2) * 32 - 1 : (i + 1) * 32]),
                .data_valid_out_2   (data_valid_level[0][i + 1])
            );
        end
        // level 1 - 1
        for (i = 0; i < PIPELINE_NUM; i = i + 8) begin
            for (j = 0; j < 2; j = j + 1) begin
                // ascending
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_1_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[1][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[1][i + j]),
                    .data_out_2         (data_level[1][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[1][i + j + 2])
                );
            end
        end
        for (i = 1; i < PIPELINE_NUM; i = i + 8) begin
            for (j = 0; j < 2; j = j + 1) begin
                // descending
                descending_comparator DESCENDING_COMPARATOR_LEVEL_1_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j+ 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[1][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[1][i + j]),
                    .data_out_2         (data_level[1][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[1][i + j + 2])
                );
            end
        end
        // level 1 - 2
        for (i = 0; i < PIPELINE_NUM; i = i + 8) begin
            for (j = 0; j < 2; j = j + 1) begin
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_1_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + 2 * j]),
                    .data_in_2          (data_in[(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_in_2    (data_valid_in[i + 2 * j + 1]),

                    .data_out_1         (data_level[2][(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_out_1   (data_valid_level[2][i + 2 * j]),
                    .data_out_2         (data_level[2][(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_out_2   (data_valid_level[2][i + 2 * j + 1])
                );
            end
        end
        for (i = 4; i < PIPELINE_NUM; i = i + 8) begin
            for (j = 0; j < 2; j = j + 1) begin
                descending_comparator DESCENDING_COMPARATOR_LEVEL_1_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + 2 * j]),
                    .data_in_2          (data_in[(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_in_2    (data_valid_in[i + 2 * j + 1]),

                    .data_out_1         (data_level[2][(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_out_1   (data_valid_level[2][i + 2 * j]),
                    .data_out_2         (data_level[2][(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_out_2   (data_valid_level[2][i + 2 * j + 1])
                );
            end
        end
        // level 2 - 1
        for (i = 0; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 4; j = j + 1) begin
                // ascending
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_2_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 5) * 32 - 1 : (i + j + 4) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 4]),

                    .data_out_1         (data_level[3][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[3][i + j]),
                    .data_out_2         (data_level[3][(i + j + 5) * 32 - 1 : (i + j + 4) * 32]),
                    .data_valid_out_2   (data_valid_level[3][i + j + 4])
                );
            end
        end
        for (i = 8; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 4; j = j + 1) begin
                // descending
                descending_comparator DESCENDING_COMPARATOR_LEVEL_2_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 5) * 32 - 1 : (i + j + 4) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 4]),

                    .data_out_1         (data_level[3][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[3][i + j]),
                    .data_out_2         (data_level[3][(i + j + 5) * 32 - 1 : (i + j + 4) * 32]),
                    .data_valid_out_2   (data_valid_level[3][i + j + 4])
                );
            end
        end
        // level 2 - 2
        for (i = 0; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 2; j = j + 1) begin
                // ascending
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_2_2_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[4][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[4][i + j]),
                    .data_out_2         (data_level[4][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[4][i + j + 2])
                );
            end
            for (j = 4; j < 6; j = j + 1) begin
                // ascending
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_2_2_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[4][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[4][i + j]),
                    .data_out_2         (data_level[4][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[4][i + j + 2])
                );
            end
        end
        for (i = 8; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 2; j = j + 1) begin
                // descending
                descending_comparator DESCENDING_COMPARATOR_LEVEL_2_2_1 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[4][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[4][i + j]),
                    .data_out_2         (data_level[4][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[4][i + j + 2])
                );
            end
            for (j = 4; j < 6; j = j + 1) begin
                // descending
                descending_comparator DESCENDING_COMPARATOR_LEVEL_2_2_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + j]),
                    .data_in_2          (data_in[(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_in_2    (data_valid_in[i + j + 2]),

                    .data_out_1         (data_level[4][(i + j + 1) * 32 - 1 : (i + j) * 32]),
                    .data_valid_out_1   (data_valid_level[4][i + j]),
                    .data_out_2         (data_level[4][(i + j + 3) * 32 - 1 : (i + j + 2) * 32]),
                    .data_valid_out_2   (data_valid_level[4][i + j + 2])
                );
            end
        end
        // level 2 - 3
        for (i = 0; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 4; j = j + 1) begin
                ascending_comparator ASCENDING_COMPARATOR_LEVEL_1_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + 2 * j]),
                    .data_in_2          (data_in[(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_in_2    (data_valid_in[i + 2 * j + 1]),

                    .data_out_1         (data_level[5][(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_out_1   (data_valid_level[5][i + 2 * j]),
                    .data_out_2         (data_level[5][(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_out_2   (data_valid_level[5][i + 2 * j + 1])
                );
            end
        end
        for (i = 8; i < PIPELINE_NUM; i = i + 16) begin
            for (j = 0; j < 4; j = j + 1) begin
                descending_comparator DESCENDING_COMPARATOR_LEVEL_1_2 (
                    .clk                (clk),
                    .rst                (rst),
                    .data_in_1          (data_in[(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_in_1    (data_valid_in[i + 2 * j]),
                    .data_in_2          (data_in[(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_in_2    (data_valid_in[i + 2 * j + 1]),

                    .data_out_1         (data_level[5][(i + 2 * j + 1) * 32 - 1 : (i + 2 * j) * 32]),
                    .data_valid_out_1   (data_valid_level[5][i + 2 * j]),
                    .data_out_2         (data_level[5][(i + 2 * j + 2) * 32 - 1 : (i + 2 * j + 1) * 32]),
                    .data_valid_out_2   (data_valid_level[5][i + 2 * j + 1])
                );
            end
        end
        // level 3
    endgenerate

endmodule

// 无效信号默认最小
module ascending_comparator #(parameter
    DATA_WIDTH = 32
) (
    input clk,
    input rst,
    input [DATA_WIDTH - 1 : 0] data_in_1,
    input data_valid_in_1,
    input [DATA_WIDTH - 1 : 0] data_in_2,
    input data_valid_in_2,

    output reg [DATA_WIDTH - 1 : 0] data_out_1,
    output reg data_valid_out_1,
    output reg [DATA_WIDTH - 1 : 0] data_out_2,
    output reg data_valid_out_2
);

    always @ (posedge clk) begin
        if (rst) begin
            data_out_1 <= 0;
            data_valid_out_1 <= 0;
            data_out_2 <= 0;
            data_valid_out_2 <= 0;
        end else begin
            if (data_valid_in_1 && data_valid_in_2) begin
                if (data_in_2 >= data_in_1) begin
                    data_out_1 <= data_in_1;
                    data_out_2 <= data_in_2;
                end else begin
                    data_out_1 <= data_in_2;
                    data_out_2 <= data_in_1;
                end
                data_valid_out_1 <= 1;
                data_valid_out_2 <= 1;
            end else if (data_valid_in_1) begin
                data_out_1 <= 0;
                data_valid_out_1 <= 0;
                data_out_2 <= data_in_1;
                data_valid_out_2 <= 1;
            end else if (data_valid_in_2) begin
                data_out_1 <= 0;
                data_valid_out_1 <= 0;
                data_out_2 <= data_in_2;
                data_valid_out_2 <= 1;
            end else begin
                data_out_1 <= 0;
                data_valid_out_1 <= 0;
                data_out_2 <= 0;
                data_valid_out_2 <= 0;
            end
        end
    end

endmodule

module descending_comparator #(parameter
    DATA_WIDTH = 32
) (
    input clk,
    input rst,
    input [DATA_WIDTH - 1 : 0] data_in_1,
    input data_valid_in_1,
    input [DATA_WIDTH - 1 : 0] data_in_2,
    input data_valid_in_2,

    output reg [DATA_WIDTH - 1 : 0] data_out_1,
    output reg data_valid_out_1,
    output reg [DATA_WIDTH - 1 : 0] data_out_2,
    output reg data_valid_out_2
);

    always @ (posedge clk) begin
        if (rst) begin
            data_out_1 <= 0;
            data_valid_out_1 <= 0;
            data_out_2 <= 0;
            data_valid_out_2 <= 0;
        end else begin
            if (data_valid_in_1 && data_valid_in_2) begin
                if (data_in_2 >= data_in_1) begin
                    data_out_1 <= data_in_2;
                    data_out_2 <= data_in_1;
                end else begin
                    data_out_1 <= data_in_1;
                    data_out_2 <= data_in_2;
                end
                data_valid_out_1 <= 1;
                data_valid_out_2 <= 1;
            end else if (data_valid_in_1) begin
                data_out_1 <= data_in_1;
                data_valid_out_1 <= 1;
                data_out_2 <= 0;
                data_valid_out_2 <= 0;
            end else if (data_valid_in_2) begin
                data_out_1 <= data_in_2;
                data_valid_out_1 <= 1;
                data_out_2 <= 0;
                data_valid_out_2 <= 0;
            end else begin
                data_out_1 <= 0;
                data_valid_out_1 <= 0;
                data_out_2 <= 0;
                data_valid_out_2 <= 0;
            end
        end
    end

endmodule