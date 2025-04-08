`timescale 1ns / 1ps

`include "accelerator.vh"

module BRAM_tb ();
    parameter DELTA_BRAM_AWIDTH = 15;
    parameter DELTA_BRAM_DWIDTH = 36;
    reg clk;
    reg [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_uram_rd_addr ;
    reg                                delta2_uram_rd_valid ;
    reg [DELTA_BRAM_AWIDTH - 1 : 0]    delta2_uram_wr_addr ;
    reg [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_uram_wr_value ;
    reg                                delta2_uram_wr_valid ;
    wire [DELTA_BRAM_DWIDTH - 1 : 0]    delta2_uram_data;
    always begin
        #5 clk = 0;
        #5 clk = 1;
    end
    initial begin
        delta2_uram_rd_valid = 1'b0;
        delta2_uram_wr_valid = 1'b0;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 0;
        delta2_uram_wr_value = 0;
        delta2_uram_rd_addr = 0;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 0;
        delta2_uram_wr_value = 0;
        delta2_uram_rd_addr = 1;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 0;
        delta2_uram_wr_value = 0;
        delta2_uram_rd_addr = 2;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 1;
        delta2_uram_wr_value = 3;
        delta2_uram_rd_addr = 3;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 1;
        delta2_uram_wr_value = 3;
        delta2_uram_rd_addr = 3;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 1;
        delta2_uram_wr_valid = 1;
        delta2_uram_wr_value = 5;
        delta2_uram_rd_addr = 3;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 0;
        delta2_uram_wr_valid = 1;
        delta2_uram_wr_value = 5;
        delta2_uram_rd_addr = 0;
        delta2_uram_rd_valid = 1;
        #10
        delta2_uram_wr_addr = 3;
        delta2_uram_wr_valid = 1;
        delta2_uram_wr_value = 8;
        delta2_uram_rd_addr = 4;
        delta2_uram_rd_valid = 1;
    end
    
    delta2_uram_ip_single DELTA2_BRAM_IP_SINGLE (
        .clka       (clk),
        .ena        (1'b1),
        .wea        (delta2_uram_wr_valid),
        .addra      (delta2_uram_wr_addr),
        .dina       (delta2_uram_wr_value),
        .clkb       (clk),
        .enb        (delta2_uram_rd_valid),
        .addrb      (delta2_uram_rd_addr),
        .doutb      (delta2_uram_data)
    );

endmodule