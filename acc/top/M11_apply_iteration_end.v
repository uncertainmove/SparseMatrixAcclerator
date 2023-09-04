`timescale 1ns / 1ps

`include "accelerator.vh"

module apply_iteration_end #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    CORE_NUM = `CORE_NUM,
    ITERATION_WIDTH = `ITERATION_WIDTH
) (
    input                                       clk,
    input [CORE_NUM - 1 : 0]                    rst,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id,
    input [CORE_NUM - 1 : 0]                    front_active_v_updated,
    input [CORE_NUM - 1 : 0]                    front_active_v_valid,
    input [CORE_NUM - 1 : 0]                    front_iteration_end,
    input [CORE_NUM - 1 : 0]                    front_iteration_end_valid,

    output [CORE_NUM * V_ID_WIDTH - 1 : 0]      active_v_id,
    output [CORE_NUM - 1 : 0]                   active_v_updated,
    output [CORE_NUM - 1 : 0]                   active_v_valid,
    output [CORE_NUM - 1 : 0]                   iteration_end,
    output [CORE_NUM - 1 : 0]                   iteration_end_valid
);

    wire iteration_end_combined;

    btree_combine_iteration_end BTREE_COMBINE_ITERATION_END (
        .front_iteration_end        (front_iteration_end),

        .iteration_end_combined     (iteration_end_combined)
    );

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            apply_iteration_end_single APPLY_ITERATION_END_SINGLE (
                .clk                        (clk),
                .rst                        (rst[i]),
                .front_active_v_id          (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .front_active_v_updated     (front_active_v_updated[i]),
                .front_active_v_valid       (front_active_v_valid[i]),
                .front_iteration_end        (iteration_end_combined),
                .front_iteration_end_valid  (front_iteration_end_valid[i]),

                .active_v_id                (active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .active_v_updated           (active_v_updated[i]),
                .active_v_valid             (active_v_valid[i]),
                .iteration_end              (iteration_end[i]),
                .iteration_end_valid        (iteration_end_valid[i])
            );
        end
    endgenerate

endmodule

module apply_iteration_end_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    CORE_NUM = `CORE_NUM
) (
    input                           clk,
    input                           rst,
    input [V_ID_WIDTH - 1 : 0]      front_active_v_id,
    input                           front_active_v_updated,
    input                           front_active_v_valid,
    input                           front_iteration_end,
    input                           front_iteration_end_valid,

    output reg [V_ID_WIDTH - 1 : 0] active_v_id,
    output reg                      active_v_updated,
    output reg                      active_v_valid,
    output reg                      iteration_end,
    output reg                      iteration_end_valid
);

    always @ (posedge clk) begin
        if (rst || !front_active_v_valid) begin
            active_v_id <= 0;
            active_v_updated <= 0;
            active_v_valid <= 0;
        end else begin
            active_v_id <= front_active_v_id;
            active_v_updated <= front_active_v_updated;
            active_v_valid <= 1;
        end
    end
    
    // 这里Iteration是异步还是同步？
    always @ (posedge clk) begin
        if (rst || !front_iteration_end_valid) begin
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end else begin
            iteration_end <= front_iteration_end;
            iteration_end_valid <= 1;
        end
    end

endmodule

module btree_combine_iteration_end #(parameter
    CORE_NUM = `CORE_NUM
) (
    input [CORE_NUM - 1 : 0] front_iteration_end,

    output iteration_end_combined
);

    // wire [63 : 0] iteration_end_level3;
    wire [31 : 0] iteration_end_level4;
    wire [15 : 0] iteration_end_level5;
    wire [7 : 0] iteration_end_level6;
    wire [3 : 0] iteration_end_level7;
    wire [1 : 0] iteration_end_level8;
    
    
    assign iteration_end_combined = iteration_end_level8[0] && iteration_end_level8[1];
    generate
        genvar i;
        
        for (i = 0; i < 32; i = i + 1) begin
            assign iteration_end_level4[i] = front_iteration_end[i];
        end
        for (i = 0; i < 16; i = i + 1) begin
            assign iteration_end_level5[i] = iteration_end_level4[2 * i] & iteration_end_level4[2 * i + 1];
        end
        for (i = 0; i < 8; i = i + 1) begin
            assign iteration_end_level6[i] = iteration_end_level5[2 * i] & iteration_end_level5[2 * i + 1];
        end
        for (i = 0; i < 4; i = i + 1) begin
            assign iteration_end_level7[i] = iteration_end_level6[2 * i] & iteration_end_level6[2 * i + 1];
        end
        for (i = 0; i < 2; i = i + 1) begin
            assign iteration_end_level8[i] = iteration_end_level7[2 * i] & iteration_end_level7[2 * i + 1];
        end
    endgenerate
    

endmodule