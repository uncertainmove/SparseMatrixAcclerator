`timescale 1ns / 1ps

`include "accelerator.vh"

module custom_fifo #(parameter
    FIFO_DWIDTH = 32,
    FIFO_AWIDTH = 2
) (
    input clk,
    input srst,
    input [FIFO_DWIDTH - 1 : 0] din,
    input wr_en,
    input rd_en,
    
    output reg [FIFO_DWIDTH - 1 : 0] dout,
    output full,
    output prog_full,
    output [FIFO_AWIDTH : 0] data_count,
    output empty,
    output reg valid
);

    reg [FIFO_AWIDTH - 1 : 0] rd_idx, wr_idx;
    reg [FIFO_DWIDTH - 1 : 0] mem [0 : (1 << FIFO_AWIDTH)];

    assign full = srst || (wr_idx + 1 == rd_idx) || ((wr_idx + 1)-(1 << FIFO_AWIDTH) == rd_idx);
    assign prog_full = srst || data_count[FIFO_AWIDTH - 1 : 0] >= (1 << FIFO_AWIDTH) - 4;    
    assign empty = srst || (wr_idx == rd_idx);
    assign data_count = wr_idx - rd_idx;
    
    always @ (posedge clk) begin
        if (srst) begin
            rd_idx <= 0;
            wr_idx <= 0;
        end else begin
            if (wr_en && !full) begin
                mem[wr_idx] <= din;
                wr_idx <= wr_idx + 1;
            end
            if (rd_en && !empty) begin
                rd_idx <= rd_idx + 1;
            end
        end
    end

    always @ (posedge clk) begin
        if (srst) begin
            dout <= 0;
            valid <= 0;
        end else begin
            if (rd_en && !empty) begin
                dout <= mem[rd_idx];
                valid <= 1;
            end else begin
                dout <= 0;
                valid <= 0;
            end
        end
    end

endmodule

module custom_fifo_ft #(parameter
    FIFO_DWIDTH = 32,
    FIFO_AWIDTH = 2
) (
    input clk,
    input srst,
    input [FIFO_DWIDTH - 1 : 0] din,
    input wr_en,
    input rd_en,
    
    output [FIFO_DWIDTH - 1 : 0] dout,
    output full,
    output prog_full,
    output [FIFO_AWIDTH : 0] data_count,
    output empty,
    output valid
);

    reg [FIFO_AWIDTH - 1 : 0] rd_idx, wr_idx;
    reg [FIFO_DWIDTH - 1 : 0] mem [0 : (1 << FIFO_AWIDTH)];

    assign full = srst || (wr_idx + 1 == rd_idx) || ((wr_idx + 1)-(1 << FIFO_AWIDTH) == rd_idx);
    assign prog_full = srst || data_count[FIFO_AWIDTH - 1 : 0] >= (1 << FIFO_AWIDTH) - 4;
    assign empty = srst || (wr_idx == rd_idx);
    assign dout = mem[rd_idx];
    assign valid = !empty;
    assign data_count = wr_idx - rd_idx;

    always @ (posedge clk) begin
        if (srst) begin
            rd_idx <= 0;
            wr_idx <= 0;
        end else begin
            if (wr_en && !full) begin
                mem[wr_idx] <= din;
                wr_idx <= wr_idx + 1;
            end
            if (rd_en && !empty) begin
                rd_idx <= rd_idx + 1;
            end
        end
    end

endmodule