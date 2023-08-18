`timescale 1ns / 1ps

`include "accelerator.vh"

module hyperx_network #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    HPX_ROW_NUM = `HPX_ROW_NUM,
    HPX_COLUMN_NUM = `HPX_COLUMN_NUM
) (
    input                                       clk,
    input [CORE_NUM - 1 : 0]                    rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_update_v_id,
    input [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_update_v_value,
    input [CORE_NUM - 1 : 0]                    front_update_v_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  front_iteration_id,
    input [CORE_NUM - 1 : 0]                    dest_core_full,

    output [CORE_NUM - 1 : 0]                   next_rst,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      update_v_id,
    output [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   update_v_value,
    output [CORE_NUM - 1 : 0]                   update_v_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id,
    output [CORE_NUM - 1 : 0]                   stage_full
);

    wire [V_ID_WIDTH * HPX_ROW_NUM - 1 : 0] row_crossbar_v_id [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_ROW_NUM - 1 : 0] row_crossbar_v_valid [0 : HPX_COLUMN_NUM - 1];
    wire [V_VALUE_WIDTH * HPX_ROW_NUM - 1 : 0] row_crossbar_v_value [0 : HPX_COLUMN_NUM - 1];

    wire [V_ID_WIDTH * HPX_COLUMN_NUM - 1 : 0] column_crossbar_v_id [0 : HPX_ROW_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] column_crossbar_v_valid [0 : HPX_ROW_NUM - 1];
    wire [V_VALUE_WIDTH * HPX_COLUMN_NUM - 1 : 0] column_crossbar_v_value [0 : HPX_ROW_NUM - 1];

    wire [HPX_ROW_NUM - 1 : 0] read_row [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] read_column [0 : HPX_ROW_NUM - 1];

    wire [V_ID_WIDTH * HPX_ROW_NUM - 1 : 0] row_v_id [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_ROW_NUM - 1 : 0] row_v_valid [0 : HPX_COLUMN_NUM - 1];
    wire [V_VALUE_WIDTH * HPX_ROW_NUM - 1 : 0] row_v_value [0 : HPX_COLUMN_NUM - 1];

    wire [V_ID_WIDTH * HPX_COLUMN_NUM - 1 : 0] column_v_id [0 : HPX_ROW_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] column_v_valid [0 : HPX_ROW_NUM - 1];
    wire [V_VALUE_WIDTH * HPX_COLUMN_NUM - 1 : 0] column_v_value [0 : HPX_ROW_NUM - 1];

    wire [HPX_ROW_NUM - 1 : 0] column_buffer_full [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] local_buffer_full [0 : HPX_ROW_NUM - 1];

    generate
        genvar i, j;
        for (i = 0; i < HPX_ROW_NUM; i = i + 1) begin
            hyperx_network_column_crossbar_single HYPERX_COLUMN_CROSSBAR_SINGLE (
                .clk                (clk),
                .rst                (rst),
                .front_v_id         (column_v_id[i]),
                .front_v_value      (column_v_value[i]),
                .front_v_valid      (column_v_valid[i]),
                .local_buffer_full  (local_buffer_full[i]),

                .read_column        (read_column[i]),
                .v_id               (column_crossbar_v_id[i]),
                .v_value            (column_crossbar_v_value[i]),
                .v_valid            (column_crossbar_v_valid[i])
            );
        end
        for (i = 0; i < HPX_COLUMN_NUM; i = i + 1) begin
            hyperx_network_row_crossbar_single HYPERX_ROW_CROSSBAR_SINGLE (
                .clk                (clk),
                .rst                (rst),
                .front_v_id         (row_v_id[i]),
                .front_v_value      (row_v_value[i]),
                .front_v_valid      (row_v_valid[i]),
                .column_buffer_full (column_buffer_full[i]),

                .read_row           (read_row[i]),
                .v_id               (row_crossbar_v_id[i]),
                .v_value            (row_crossbar_v_value[i]),
                .v_valid            (row_crossbar_v_valid[i])
            );
        end
        for (i = 0; i < HPX_ROW_NUM; i = i + 1) begin
            for (j = 0; j < HPX_COLUMN_NUM; j = j + 1) begin
                hyperx_network_single HYPERX_NETWORK_SINGLE (
                    .clk                        (clk),
                    .rst                        (rst),
                    .front_v_id                 (front_update_v_id[(i * HPX_COLUMN_NUM + j + 1) * V_ID_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * V_ID_WIDTH]),
                    .front_v_value              (front_update_v_value[(i * HPX_COLUMN_NUM + j + 1) * V_VALUE_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * V_VALUE_WIDTH]),
                    .front_v_valid              (front_update_v_valid[i * HPX_COLUMN_NUM + j]),
                    .front_iteration_end        (front_iteration_end[i * HPX_COLUMN_NUM + j]),
                    .front_iteration_end_valid  (front_iteration_end_valid[i * HPX_COLUMN_NUM + j]),
                    .front_iteration_id         (front_iteration_id[(i * HPX_COLUMN_NUM + j + 1) * ITERATION_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * ITERATION_WIDTH]),
                    .next_stage_full            (dest_core_full[i * HPX_COLUMN_NUM + j]),

                    .read_row                   (read_row[j][i]),
                    .front_row_v_id             (row_crossbar_v_id[j][(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                    .front_row_v_value          (row_crossbar_v_value[j][(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                    .front_row_v_valid          (row_crossbar_v_valid[j][i]),

                    .read_column                (read_column[i][j]),
                    .front_column_v_id          (column_crossbar_v_id[i][(j + 1) * V_ID_WIDTH - 1 : j * V_ID_WIDTH]),
                    .front_column_v_value       (column_crossbar_v_value[i][(j + 1) * V_VALUE_WIDTH - 1 : j * V_VALUE_WIDTH]),
                    .front_column_v_valid       (column_crossbar_v_valid[i][j]),

                    .row_v_id                   (row_v_id[j][(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                    .row_v_value                (row_v_value[j][(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH]),
                    .row_v_valid                (row_v_valid[j][i]),
                    .row_buffer_full            (),

                    .column_v_id                (column_v_id[i][(j + 1) * V_ID_WIDTH - 1 : j * V_ID_WIDTH]),
                    .column_v_value             (column_v_value[i][(j + 1) * V_VALUE_WIDTH - 1 : j * V_VALUE_WIDTH]),
                    .column_v_valid             (column_v_valid[i][j]),
                    .column_buffer_full         (column_buffer_full[j][i]),
                    .local_buffer_full          (local_buffer_full[i][j]),

                    .next_rst                   (next_rst[i * HPX_COLUMN_NUM + j]),
                    .update_v_id                (update_v_id[(i * HPX_COLUMN_NUM + j + 1) * V_ID_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * V_ID_WIDTH]),
                    .update_v_value             (update_v_value[(i * HPX_COLUMN_NUM + j + 1) * V_VALUE_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * V_VALUE_WIDTH]),
                    .update_v_valid             (update_v_valid[i * HPX_COLUMN_NUM + j]),
                    .iteration_end              (iteration_end[i * HPX_COLUMN_NUM + j]),
                    .iteration_end_valid        (iteration_end_valid[i * HPX_COLUMN_NUM + j]),
                    .iteration_id               (iteration_id[(i * HPX_COLUMN_NUM + j + 1) * ITERATION_WIDTH - 1 : (i * HPX_COLUMN_NUM + j) * ITERATION_WIDTH]),
                    .stage_buffer_full          (stage_full[i * HPX_COLUMN_NUM + j])
                );
            end
        end
    endgenerate

endmodule

module hyperx_network_row_crossbar_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HPX_ROW_NUM = `HPX_ROW_NUM,
    HPX_COLUMN_NUM_WIDTH = `HPX_COLUMN_NUM_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH
) (
    input                                          clk,
    input                                          rst,
    input [HPX_ROW_NUM * V_ID_WIDTH - 1 : 0]       front_v_id,
    input [HPX_ROW_NUM * V_VALUE_WIDTH - 1 : 0]    front_v_value,
    input [HPX_ROW_NUM - 1 : 0]                    front_v_valid,
    input [HPX_ROW_NUM - 1 : 0]                    column_buffer_full,

    output [HPX_ROW_NUM - 1 : 0]                   read_row,
    output reg [HPX_ROW_NUM * V_ID_WIDTH - 1 : 0]      v_id,
    output reg [HPX_ROW_NUM * V_VALUE_WIDTH - 1 : 0]   v_value,
    output reg [HPX_ROW_NUM - 1 : 0]                   v_valid
);

    wire [HPX_ROW_NUM - 1 : 0] shuffle_signal [0 : HPX_ROW_NUM - 1];
    reg [HPX_ROW_NUM - 1 : 0] pe_priority;
    wire [HPX_ROW_NUM * 2 - 1 : 0] tmp_grant [0 : HPX_ROW_NUM - 1];
    wire [HPX_ROW_NUM - 1 : 0] grant [0 : HPX_ROW_NUM - 1];
    wire [HPX_ROW_NUM - 1 : 0] tmp_read_row [0 : HPX_ROW_NUM - 1];

    always @ (posedge clk) begin
        if (rst) begin
            pe_priority <= 1;
        end else begin
            pe_priority <= {pe_priority[HPX_ROW_NUM - 2 : 0], pe_priority[HPX_ROW_NUM - 1]};
        end
    end

    generate
        genvar i, j;
        for (i = 0; i < HPX_ROW_NUM; i = i + 1) begin
            for (j = 0; j < HPX_ROW_NUM; j = j + 1) begin
                // if port i recieve data from port j
                // level 1
                assign shuffle_signal[i][j] = (front_v_id[j * V_ID_WIDTH + CORE_NUM_WIDTH - 1 : j * V_ID_WIDTH + HPX_COLUMN_NUM_WIDTH] == i) && front_v_valid[j] && !column_buffer_full[i];
                // level 4
                assign tmp_read_row[j][i] = grant[i][j];
            end
            // level 5
            assign read_row[i] = tmp_read_row[i] != 0;
            // level 2
            assign tmp_grant[i] = {shuffle_signal[i], shuffle_signal[i]} & ~({shuffle_signal[i], shuffle_signal[i]} - pe_priority);
            // level 3
            assign grant[i] = tmp_grant[i][HPX_ROW_NUM * 2 - 1 : HPX_ROW_NUM] | tmp_grant[i][HPX_ROW_NUM - 1 : 0];
            always @ (*) begin
                case (1'b1)
                    grant[i][0]: begin
                        // level 4
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[V_ID_WIDTH - 1 : 0];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[V_VALUE_WIDTH - 1 : 0];
                        v_valid[i] <= 1;
                    end
                    grant[i][1]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[2 * V_ID_WIDTH - 1 : V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[2 * V_VALUE_WIDTH - 1 : V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][2]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[3 * V_ID_WIDTH - 1 : 2 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[3 * V_VALUE_WIDTH - 1 : 2 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][3]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[4 * V_ID_WIDTH - 1 : 3 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[4 * V_VALUE_WIDTH - 1 : 3 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    default: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= 0;
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= 0;
                        v_valid[i] <= 0;
                    end
                endcase
            end
        end
    endgenerate

endmodule

module hyperx_network_column_crossbar_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HPX_COLUMN_NUM = `HPX_COLUMN_NUM,
    HPX_COLUMN_NUM_WIDTH = `HPX_COLUMN_NUM_WIDTH
) (
    input                                             clk,
    input                                             rst,
    input [HPX_COLUMN_NUM * V_ID_WIDTH - 1 : 0]       front_v_id,
    input [HPX_COLUMN_NUM * V_VALUE_WIDTH - 1 : 0]    front_v_value,
    input [HPX_COLUMN_NUM - 1 : 0]                    front_v_valid,
    input [HPX_COLUMN_NUM - 1 : 0]                    local_buffer_full,

    output [HPX_COLUMN_NUM - 1 : 0]                   read_column,
    output reg [HPX_COLUMN_NUM * V_ID_WIDTH - 1 : 0]      v_id,
    output reg [HPX_COLUMN_NUM * V_VALUE_WIDTH - 1 : 0]   v_value,
    output reg [HPX_COLUMN_NUM - 1 : 0]                   v_valid
);

    wire [HPX_COLUMN_NUM - 1 : 0] shuffle_signal [0 : HPX_COLUMN_NUM - 1];
    reg [HPX_COLUMN_NUM - 1 : 0] pe_priority;
    wire [HPX_COLUMN_NUM * 2 - 1 : 0] tmp_grant [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] grant [0 : HPX_COLUMN_NUM - 1];
    wire [HPX_COLUMN_NUM - 1 : 0] tmp_read_column [0 : HPX_COLUMN_NUM - 1];

    always @ (posedge clk) begin
        if (rst) begin
            pe_priority <= 1;
        end else begin
            pe_priority <= {pe_priority[HPX_COLUMN_NUM - 2 : 0], pe_priority[HPX_COLUMN_NUM - 1]};
        end
    end

    generate
        genvar i, j;
        for (i = 0; i < HPX_COLUMN_NUM; i = i + 1) begin
            for (j = 0; j < HPX_COLUMN_NUM; j = j + 1) begin
                // if port i recieve data from port j
                // level 1
                assign shuffle_signal[i][j] = (front_v_id[j * V_ID_WIDTH + HPX_COLUMN_NUM_WIDTH - 1 : j * V_ID_WIDTH] == i) && front_v_valid[j] && !local_buffer_full[i];
                // level 4
                assign tmp_read_column[j][i] = grant[i][j];
            end
            // level 5
            assign read_column[i] = tmp_read_column[i] != 0;
            // level 2
            assign tmp_grant[i] = {shuffle_signal[i], shuffle_signal[i]} & ~({shuffle_signal[i], shuffle_signal[i]} - pe_priority);
            // level 3
            assign grant[i] = tmp_grant[i][HPX_COLUMN_NUM * 2 - 1 : HPX_COLUMN_NUM] | tmp_grant[i][HPX_COLUMN_NUM - 1 : 0];
            always @ (*) begin
                case (1'b1)
                    grant[i][0]: begin
                        // level 4
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[V_ID_WIDTH - 1 : 0];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[V_VALUE_WIDTH - 1 : 0];
                        v_valid[i] <= 1;
                    end
                    grant[i][1]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[2 * V_ID_WIDTH - 1 : V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[2 * V_VALUE_WIDTH - 1 : V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][2]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[3 * V_ID_WIDTH - 1 : 2 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[3 * V_VALUE_WIDTH - 1 : 2 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][3]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[4 * V_ID_WIDTH - 1 : 3 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[4 * V_VALUE_WIDTH - 1 : 3 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][4]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[5 * V_ID_WIDTH - 1 : 4 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[5 * V_VALUE_WIDTH - 1 : 4 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][5]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[6 * V_ID_WIDTH - 1 : 5 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[6 * V_VALUE_WIDTH - 1 : 5 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][6]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[7 * V_ID_WIDTH - 1 : 6 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[7 * V_VALUE_WIDTH - 1 : 6 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][7]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[8 * V_ID_WIDTH - 1 : 7 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[8 * V_VALUE_WIDTH - 1 : 7 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][8]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[9 * V_ID_WIDTH - 1 : 8 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[9 * V_VALUE_WIDTH - 1 : 8 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][9]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[10 * V_ID_WIDTH - 1 : 9 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[10 * V_VALUE_WIDTH - 1 : 9 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][10]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[11 * V_ID_WIDTH - 1 : 10 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[11 * V_VALUE_WIDTH - 1 : 10 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][11]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[12 * V_ID_WIDTH - 1 : 11 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[12 * V_VALUE_WIDTH - 1 : 11 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][12]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[13 * V_ID_WIDTH - 1 : 12 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[13 * V_VALUE_WIDTH - 1 : 12 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][13]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[14 * V_ID_WIDTH - 1 : 13 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[14 * V_VALUE_WIDTH - 1 : 13 * V_VALUE_WIDTH];
                        v_valid[i] = 1;
                    end
                    grant[i][14]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[15 * V_ID_WIDTH - 1 : 14 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[15 * V_VALUE_WIDTH - 1 : 14 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    grant[i][15]: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= front_v_id[16 * V_ID_WIDTH - 1 : 15 * V_ID_WIDTH];
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= front_v_value[16 * V_VALUE_WIDTH - 1 : 15 * V_VALUE_WIDTH];
                        v_valid[i] <= 1;
                    end
                    default: begin
                        v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH] <= 0;
                        v_value[(i + 1) * V_VALUE_WIDTH - 1 : i * V_VALUE_WIDTH] <= 0;
                        v_valid[i] <= 0;
                    end
                endcase
            end
        end
    endgenerate

endmodule

module hyperx_network_single #(parameter
    HPX_ROW_ID = 0,
    HPX_COLUMN_ID = 0,
    V_ID_WIDTH = `V_ID_WIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    HPX_ROW_NUM = `HPX_ROW_NUM,
    HPX_COLUMN_NUM = `HPX_COLUMN_NUM
) (
    input                           clk,
    input                           rst,
    input [V_ID_WIDTH - 1 : 0]      front_v_id,
    input [V_VALUE_WIDTH - 1 : 0]   front_v_value,
    input                           front_v_valid,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,
    input                           front_iteration_id,
    input                           next_stage_full,

    input                           read_row,
    input [V_ID_WIDTH - 1 : 0]      front_row_v_id,
    input [V_VALUE_WIDTH - 1 : 0]   front_row_v_value,
    input                           front_row_v_valid,

    input                           read_column,
    input [V_ID_WIDTH - 1 : 0]      front_column_v_id,
    input [V_VALUE_WIDTH - 1 : 0]   front_column_v_value,
    input                           front_column_v_valid,

    output [V_ID_WIDTH - 1 : 0]     row_v_id,
    output [V_VALUE_WIDTH - 1 : 0]  row_v_value,
    output                          row_v_valid,
    output                          row_buffer_full,

    output [V_ID_WIDTH - 1 : 0]     column_v_id,
    output [V_VALUE_WIDTH - 1 : 0]  column_v_value,
    output                          column_v_valid,
    output                          column_buffer_full,
    output                          local_buffer_full,

    output reg                          next_rst,
    output [V_ID_WIDTH - 1 : 0]         update_v_id,
    output [V_VALUE_WIDTH - 1 : 0]      update_v_value,
    output                              update_v_valid,
    output reg                          iteration_end,
    output reg                          iteration_end_valid,
    output reg                          iteration_id,
    output                              stage_buffer_full
);

    wire row_buffer_empty, column_buffer_empty, update_buffer_empty;

    assign stage_buffer_full = row_buffer_full;

    always @ (posedge clk) begin
        if (!rst && front_iteration_end && front_iteration_end_valid && row_buffer_empty &&
                column_buffer_empty && update_buffer_empty && !front_v_valid) begin
            iteration_end <= front_iteration_end;
            iteration_end_valid <= 1;
        end else begin
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end
        iteration_id <= front_iteration_id;
        next_rst <= rst;
    end

    active_v_id_fifo_ft ROW_ACTIVE_V_ID_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_v_id),
        .wr_en      (front_v_valid),
        .rd_en      (read_row),
        .dout       (row_v_id),
        .prog_full  (row_buffer_empty),
        .full       (),
        .empty      (row_buffer_full),
        .valid      (row_v_valid)
    );
    active_v_value_fifo_ft ROW_ACTIVE_V_VALUE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_v_value),
        .wr_en      (front_v_valid),
        .rd_en      (read_row),
        .dout       (row_v_value),
        .prog_full  (),
        .full       (),
        .empty      (),
        .valid      ()
    );
    active_v_id_fifo_ft COLUMN_ACTIVE_V_ID_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_row_v_id),
        .wr_en      (front_row_v_valid),
        .rd_en      (read_column),
        .dout       (column_v_id),
        .prog_full  (column_buffer_empty),
        .full       (),
        .empty      (column_buffer_full),
        .valid      (column_v_valid)
    );
    active_v_value_fifo_ft COLUMN_ACTIVE_V_VALUE_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (front_row_v_value),
        .wr_en      (front_row_v_valid),
        .rd_en      (read_column),
        .dout       (column_v_value),
        .prog_full  (),
        .full       (),
        .empty      (),
        .valid      ()
    );
    active_v_id_fifo UPDATE_ACTIVE_V_ID_FIFO (
        .clk        (clk),
        .srst       (rst),
        .din        (front_column_v_id),
        .wr_en      (front_column_v_valid),
        .rd_en      (!next_stage_full && !update_buffer_empty),
        .dout       (update_v_id),
        .prog_full  (local_buffer_full),
        .full       (),
        .empty      (local_buffer_empty),
        .valid      (update_v_valid)
    );
    active_v_value_fifo UPDATE_ACTIVE_V_VALUE_FIFO (
        .clk        (clk),
        .srst       (rst),
        .din        (front_column_v_value),
        .wr_en      (front_column_v_valid),
        .rd_en      (!next_stage_full && !update_buffer_empty),
        .dout       (update_v_value),
        .prog_full  (),
        .full       (),
        .empty      (),
        .valid      ()
    );

endmodule