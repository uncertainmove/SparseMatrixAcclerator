`timescale 1ns / 1ps

`include "accelerator.vh"

module M03_tb ();

    parameter integer V_ID_WIDTH = `V_ID_WIDTH;
    parameter integer V_OFF_AWIDTH = `V_OFF_AWIDTH;
    parameter integer V_OFF_DWIDTH = `V_OFF_DWIDTH;
    parameter integer V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter integer CORE_NUM = `CORE_NUM;
    parameter integer CORE_NUM_WIDTH = `CORE_NUM_WIDTH;

    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz
    
    logic ap_clk = 1;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic ap_rst = 0;

    // module input
    reg [CORE_NUM * V_OFF_AWIDTH - 1 : 0]   front_rd_active_v_offset_addr;
    reg [CORE_NUM - 1 : 0]                  front_active_v_valid;
    // module output
    wire [CORE_NUM - 1 : 0]                 next_rst;
    wire [CORE_NUM * V_OFF_DWIDTH - 1 : 0]  uram_loffset;
    wire [CORE_NUM * V_OFF_DWIDTH - 1 : 0]  uram_roffset;
    wire [CORE_NUM - 1 : 0]                 uram_dvalid;

    integer i, j;
    logic test_pass = 1;

    // task automatic SetUpTestCase();
    //     // setup uram
    //     // BUG can't find OFFSET_URAM_SINGLE under URAM!
    //     for (i = 0; i < CORE_NUM; i = i + 1) begin
    //         for (j = 0; j < (1 << 11); j = j + 1) begin // 11 refers to M03_offset_uram.v:uram
    //             // if (j == 0) begin
    //             //     inst_M03_dut.gen_URAM[i].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] = 'h0;
    //             //     inst_M03_dut.gen_URAM[i].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH - 1 : 0] = inst_M03_dut.gen_URAM[i].OFFSET_UEAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] + j * CORE_NUM + j + 1;
    //             // end
    //             // else begin
    //             //     inst_M03_dut.gen_URAM[i].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] = inst_M03_dut.gen_URAM[i].OFFSET_UEAM_SINGLE.URAM.mem[j - 1][V_OFF_DWIDTH - 1 : 0];
    //             //     inst_M03_dut.gen_URAM[i].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH - 1 : 0] = inst_M03_dut.gen_URAM[i].OFFSET_UEAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] + j * CORE_NUM + j + 1;
    //             // end
    //             inst_M03_dut.gen_URAM[0].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] = 'h0;
    //             inst_M03_dut.gen_URAM[i].OFFSET_URAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH - 1 : 0] = inst_M03_dut.gen_URAM[i].OFFSET_UEAM_SINGLE.URAM.mem[j][V_OFF_DWIDTH +: V_OFF_DWIDTH] + j * CORE_NUM + j + 1;
    //         end
    //     end
    // endtask

    task automatic init_input_flag();
        front_rd_active_v_offset_addr = 'h0;
        front_active_v_valid = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = 1;
        init_input_flag();
        // SetUpTestCase();
        repeat (10) @(posedge ap_clk);
        ap_rst = 0;
    endtask
    
    task automatic test_given_Init_then_Read_URAM();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            // read from only one pipeline at one clk
            for (j = 0; j < CORE_NUM; j = j + 1) begin
                if (j == i) front_active_v_valid[j] = 1'b1;
                else        front_active_v_valid[j] = 1'b0;
            end
            repeat (1) @(posedge ap_clk);
            @(negedge ap_clk);
            init_input_flag();
            repeat (4) @(posedge ap_clk);
            // read has a latency of 3 clk
            @(negedge ap_clk);
            init_input_flag();
            // check the according output
            for (j = 0; j < CORE_NUM; j = j + 1) begin
                if (j == i) begin
                    assert(uram_loffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH] == 'd100)
                    else begin
                        $error("uram_loffset[%3d] is 0x%0x, expect 0x%0x!", j, uram_loffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH], 'd100);
                        test_pass = 0;
                    end
                    assert(uram_roffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH] == 'd100)
                    else begin
                        $error("uram_roffset[%3d] is 0x%0x, expect 0x%0x!", j, uram_roffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH], 'd100);
                        test_pass = 0;
                    end
                    assert(uram_dvalid[j] == 1'b1)
                    else begin
                        $error("uram_dvalid[%3d] is 0x%0x, expect 0x%0x!", j, uram_dvalid[j], 1'b1);
                        test_pass = 0;
                    end
                end
                else begin
                    // assert(uram_loffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH] == 'h0)
                    // else begin
                    //     $error("uram_loffset[%3d] is 0x%0x, expect 0x%0x!", j, uram_loffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH], 'h0);
                    //     test_pass = 0;
                    // end
                    // assert(uram_roffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH] == 'h0)
                    // else begin
                    //     $error("uram_roffset[%3d] is 0x%0x, expect 0x%0x!", j, uram_roffset[j * V_OFF_DWIDTH +: V_OFF_DWIDTH], 'h0);
                    //     test_pass = 0;
                    // end
                    assert(uram_dvalid[j] == 1'b0)
                    else begin
                        $error("uram_dvalid[%3d] is 0x%0x, expect 0x%0x!", j, uram_dvalid[j], 1'b0);
                        test_pass = 0;
                    end
                end
            end
        end
        if (test_pass)  $display("[PASSED] test_given_Init_then_Read_URAM");
    endtask

    // DUT instantiation
    offset_uram #(
        .V_ID_WIDTH     (V_ID_WIDTH),
        .V_OFF_AWIDTH   (V_OFF_AWIDTH),
        .V_OFF_DWIDTH   (V_OFF_DWIDTH),
        .V_VALUE_WIDTH  (V_VALUE_WIDTH),
        .CORE_NUM       (CORE_NUM)
    )
    inst_M03_dut (
        .clk                            (ap_clk),
        .rst                            ({CORE_NUM{ap_rst}}),
        .front_rd_active_v_offset_addr  (front_rd_active_v_offset_addr),
        .front_active_v_valid           (front_active_v_valid),

        .next_rst                       (next_rst),
        .uram_loffset                   (uram_loffset),
        .uram_roffset                   (uram_roffset),
        .uram_dvalid                    (uram_dvalid)
    );

    initial begin : RD_Offset_Uram_Test
        test_given_Init_then_Read_URAM();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule