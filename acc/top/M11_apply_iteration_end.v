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

    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            apply_iteration_end_single APPLY_ITERATION_END_SINGLE (
                .clk                        (clk),
                .rst                        (rst[i]),
                .front_active_v_id          (front_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .front_active_v_updated     (front_active_v_updated[i]),
                .front_active_v_valid       (front_active_v_valid[i]),
                .front_iteration_end        (front_iteration_end == {ITERATION_WIDTH{1'b1}}),
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