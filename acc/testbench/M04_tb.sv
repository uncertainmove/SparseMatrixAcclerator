`timescale 1ns / 1ps

`include "accelerator.vh"

module M04_tb ();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_OFF_AWIDTH = `V_OFF_AWIDTH;
    parameter V_OFF_DWIDTH = `V_OFF_DWIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter HBM_AWIDTH = `HBM_AWIDTH;
    parameter HBM_EDGE_MASK = `HBM_EDGE_MASK;
    parameter FIRST_EDGE_BRAM_AWIDTH = `FIRST_EDGE_BRAM_AWIDTH;
    parameter CACHELINE_LEN_WIDTH = `CACHELINE_LEN_WIDTH;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [CORE_NUM - 1 : 0]                    front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value;
    reg [CORE_NUM - 1 : 0]                    front_active_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    front_active_v_valid;
    reg [CORE_NUM * V_OFF_DWIDTH - 1 : 0]     front_active_v_loffset;
    reg [CORE_NUM * V_OFF_DWIDTH - 1 : 0]     front_active_v_roffset;
    reg [CORE_NUM - 1 : 0]                    front_offset_valid;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                    combine_stage_full;
    reg [CORE_NUM - 1 : 0]                    bram_stage_full;
    
    // module output
    wire [CORE_NUM - 1 : 0]                             next_rst;
    wire [CORE_NUM - 1 : 0]                             stage_full;
    wire [CORE_NUM * HBM_AWIDTH - 1 : 0]                rd_hbm_edge_addr;
    wire [CORE_NUM * HBM_EDGE_MASK - 1 : 0]             rd_hbm_edge_mask;
    wire [CORE_NUM - 1 : 0]                             hbm_push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]                hbm_active_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]             hbm_active_v_value;
    wire [CORE_NUM - 1 : 0]                             rd_hbm_edge_valid;
    wire [CORE_NUM * FIRST_EDGE_BRAM_AWIDTH - 1 : 0]    rd_bram_edge_addr;
    wire [CORE_NUM - 1 : 0]                             bram_push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]                bram_active_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]             bram_active_v_value;
    wire [CORE_NUM - 1 : 0]                             rd_bram_edge_valid;
    wire [CORE_NUM - 1 : 0]                             iteration_end;
    wire [CORE_NUM - 1 : 0]                             iteration_end_valid;

    task automatic init_input_flag();
        front_push_flag = 'h0;
        front_active_v_id = 'h0;
        front_active_v_value = 'h0;
        front_active_v_pull_first_flag = 'h0;
        front_active_v_valid = 'h0;
        front_active_v_loffset = 'h0;
        front_active_v_roffset = 'h0;
        front_offset_valid = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
        combine_stage_full = 'h0;
        bram_stage_full = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j;
    logic test_pass;
    logic [31:0]front_sum[511:0];
    reg [15:0] bv [511:0];

    task automatic test_PushPull_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full init error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr init error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
            else begin
                $error("rd_hbm_edge_mask init error!");
                test_pass = 0;
            end
            assert(hbm_push_flag[i] == 1'b0)
            else begin
                $error("hbm_push_flag init error!");
                test_pass = 0;
            end
            assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_id init error!");
                test_pass = 0;
            end
            assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_value init error");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b0)
            else begin
                $error("rd_hbm_edge_valid init error");
                test_pass = 0;
            end
            assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_bram_edge_addr init error!");
                test_pass = 0;
            end
            assert(bram_push_flag[i] == 1'b0)
            else begin
                $error("bram_push_flag init error!");
                test_pass = 0;
            end
            assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_id init error!");
                test_pass = 0;
            end
            assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_value init error");
                test_pass = 0;
            end
            assert(rd_bram_edge_valid[i] == 1'b0)
            else begin
                $error("rd_bram_edge_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_PushPull_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_Push_given_InputActiveId_then_OutputId();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_sum[i] = i*(i+1)/2;
            if(i % 2 == 0)begin
                front_push_flag[i] = 1;
                front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
                front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
                front_active_v_valid[i] = 1;
                front_active_v_loffset[i * V_OFF_DWIDTH +: V_OFF_DWIDTH] = front_sum[i];
                front_active_v_roffset[i * V_OFF_DWIDTH +: V_OFF_DWIDTH] = front_sum[i] + i + 1;
                front_offset_valid[i] = 1;
            end
        end
        
        // run once to write active id
        repeat (1) @(posedge ap_clk);
        
        // CHECK
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if (i % 2 == 0) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
            else begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        
        // run three clk to output addr
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        // check
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_sum[i] = i*(i+1)/2;
            if (i % 2 == 0) begin
                if(front_sum[i] >> CACHELINE_LEN_WIDTH == (front_sum[i]+i+1) >> CACHELINE_LEN_WIDTH) begin
                    assert(stage_full[i] == 1'b0)
                    else begin
                        $error("stage_full inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == front_sum[i] >> CACHELINE_LEN_WIDTH)
                    else begin
                        $error("rd_hbm_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    bv[i]='h0;
                    for(j = front_sum[i] % 16; j < (front_sum[i]+i+1) % 16; j = j + 1)begin
                        bv[i][j]=1;
                    end
                    assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == bv[i])
                    else begin
                        $error("rd_hbm_edge_mask inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_push_flag[i] == 1'b1)
                    else begin
                        $error("hbm_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                    else begin
                        $error("hbm_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                    else begin
                        $error("hbm_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_valid[i] == !(front_sum[i] == (front_sum[i]+i+1)))
                    else begin
                        $error("rd_hbm_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                    else begin
                        $error("rd_bram_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_push_flag[i] == 1'b0)
                    else begin
                        $error("bram_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                    else begin
                        $error("bram_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                    else begin
                        $error("bram_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_valid[i] == 1'b0)
                    else begin
                        $error("rd_bram_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(iteration_end[i] == 1'b0)
                    else begin
                        $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                    end
                    assert(iteration_end_valid[i] == 1'b0)
                    else begin
                        $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                        test_pass = 0;
                    end
                    
                end
                else begin
                    assert(stage_full[i] == 1'b0)
                    else begin
                        $error("stage_full inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == front_sum[i] >> CACHELINE_LEN_WIDTH)
                    else begin
                        $error("rd_hbm_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    bv[i]='h0;
                    for(j = front_sum[i] % 16; j < 16; j = j + 1)begin
                        bv[i][j]=1;
                    end
                    assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == bv[i])
                    else begin
                        $error("rd_hbm_edge_mask inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_push_flag[i] == 1'b1)
                    else begin
                        $error("hbm_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                    else begin
                        $error("hbm_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                    else begin
                        $error("hbm_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_valid[i] == !(front_sum[i] == (front_sum[i]+i+1)))
                    else begin
                        $error("rd_hbm_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                    else begin
                        $error("rd_bram_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_push_flag[i] == 1'b0)
                    else begin
                        $error("bram_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                    else begin
                        $error("bram_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                    else begin
                        $error("bram_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_valid[i] == 1'b0)
                    else begin
                        $error("rd_bram_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(iteration_end[i] == 1'b0)
                    else begin
                        $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                    end
                    assert(iteration_end_valid[i] == 1'b0)
                    else begin
                        $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                        test_pass = 0;
                    end
                end
                
            end
            else begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_InputActiveId_then_OutputId");
    endtask
    
    task automatic test_Push_given_OutputSingal_while_LocalOffsetValid_then_LocalOffsetIncrease();
        reset_module();
        init_input_flag();
        test_pass = 1;
        front_push_flag[0] = 1;
        front_active_v_id[0 +: V_ID_WIDTH] = 0;
        front_active_v_value[0 +: V_VALUE_WIDTH] = 1;
        front_active_v_valid[0] = 1;
        front_active_v_loffset[0 +: V_OFF_DWIDTH] = 0;
        front_active_v_roffset[0 +: V_OFF_DWIDTH] = 46;
        front_offset_valid[0] = 1;
        
        // run once to write active id
        repeat (1) @(posedge ap_clk);
        
        // CHECK
            assert(stage_full[0] == 1'b0)
            else begin
                $error("stage_full write error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_addr[0+: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr write error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_mask[0+: HBM_EDGE_MASK] == 'h0)
            else begin
                $error("rd_hbm_edge_mask write error!");
                test_pass = 0;
            end
            assert(hbm_push_flag[0] == 1'b0)
            else begin
                $error("hbm_push_flag write error!");
                test_pass = 0;
            end
            assert(hbm_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_id write error!");
                test_pass = 0;
            end
            assert(hbm_active_v_value[0+: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_value write error");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[0] == 1'b0)
            else begin
                $error("rd_hbm_edge_valid write error");
                test_pass = 0;
            end
            assert(rd_bram_edge_addr[0+: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_bram_edge_addr write error!");
                test_pass = 0;
            end
            assert(bram_push_flag[0] == 1'b0)
            else begin
                $error("bram_push_flag write error!");
                test_pass = 0;
            end
            assert(bram_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_id write error!");
                test_pass = 0;
            end
            assert(bram_active_v_value[0+: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_value write error");
                test_pass = 0;
            end
            assert(rd_bram_edge_valid[0] == 1'b0)
            else begin
                $error("rd_bram_edge_valid write error");
                test_pass = 0;
            end
            assert(iteration_end[0] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end[0], 1'b0);
            end
            assert(iteration_end_valid[0] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end_valid[0], 1'b0);
                test_pass = 0;
            end
        
        // run three clk to output
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        // check
            assert(stage_full[0] == 1'b0)
            else begin
                $error("stage_full output_once error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_addr[0+: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr output_once error!");
                test_pass = 0;
            end
            bv[0] = 'h0;
            for(j = 0; j < 16; j = j + 1)begin
                bv[0][j] = 1;
            end
            assert(rd_hbm_edge_mask[0+: HBM_EDGE_MASK] == bv[0])
            else begin
                $error("rd_hbm_edge_mask output_once error!");
                test_pass = 0;
            end
            assert(hbm_push_flag[0] == 1'b1)
            else begin
                $error("hbm_push_flag output_once error!");
                test_pass = 0;
            end
            assert(hbm_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_id output_once error!");
                test_pass = 0;
            end
            assert(hbm_active_v_value[0+: V_VALUE_WIDTH] == 1)
            else begin
                $error("hbm_active_v_value output_once error");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[0] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output_once error");
                test_pass = 0;
            end
            assert(rd_bram_edge_addr[0+: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_bram_edge_addr output_once error!");
                test_pass = 0;
            end
            assert(bram_push_flag[0] == 1'b0)
            else begin
                $error("bram_push_flag output_once error!");
                test_pass = 0;
            end
            assert(bram_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_id output_once error!");
                test_pass = 0;
            end
            assert(bram_active_v_value[0+: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_value output_once error");
                test_pass = 0;
            end
            assert(rd_bram_edge_valid[0] == 1'b0)
            else begin
                $error("rd_bram_edge_valid output_once error");
                test_pass = 0;
            end
            assert(iteration_end[0] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end[0], 1'b0);
            end
            assert(iteration_end_valid[0] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end_valid[0], 1'b0);
                test_pass = 0;
            end
        
        //output twice
        @(posedge ap_clk);
        @(negedge ap_clk);
        // check
            assert(stage_full[0] == 1'b0)
            else begin
                $error("stage_full output_twice error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_addr[0+: HBM_AWIDTH] == 1)
            else begin
                $error("rd_hbm_edge_addr output_twice error!");
                test_pass = 0;
            end
            bv[0] = 'h0;
            for(j = 0; j < 16; j = j + 1)begin
                bv[0][j] = 1;
            end
            assert(rd_hbm_edge_mask[0+: HBM_EDGE_MASK] == bv[0])
            else begin
                $error("rd_hbm_edge_mask output_twice error!");
                test_pass = 0;
            end
            assert(hbm_push_flag[0] == 1'b1)
            else begin
                $error("hbm_push_flag output_twice error!");
                test_pass = 0;
            end
            assert(hbm_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_id output_twice error!");
                test_pass = 0;
            end
            assert(hbm_active_v_value[0+: V_VALUE_WIDTH] == 1)
            else begin
                $error("hbm_active_v_value output_twice error");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[0] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output_twice error");
                test_pass = 0;
            end
            assert(rd_bram_edge_addr[0+: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_bram_edge_addr output_twice error!");
                test_pass = 0;
            end
            assert(bram_push_flag[0] == 1'b0)
            else begin
                $error("bram_push_flag output_twice error!");
                test_pass = 0;
            end
            assert(bram_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_id output_twice error!");
                test_pass = 0;
            end
            assert(bram_active_v_value[0+: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_value output_twice error");
                test_pass = 0;
            end
            assert(rd_bram_edge_valid[0] == 1'b0)
            else begin
                $error("rd_bram_edge_valid output_twice error");
                test_pass = 0;
            end
            assert(iteration_end[0] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end[0], 1'b0);
            end
            assert(iteration_end_valid[0] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end_valid[0], 1'b0);
                test_pass = 0;
            end
            
        //output three times
        @(posedge ap_clk);
        @(negedge ap_clk);
        // check
            assert(stage_full[0] == 1'b0)
            else begin
                $error("stage_full output_three_times error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_addr[0+: HBM_AWIDTH] == 2)
            else begin
                $error("rd_hbm_edge_addr output_three_times error!");
                test_pass = 0;
            end
            bv[0] = 'h0;
            for(j = 0; j < 46 % 16; j = j + 1)begin
                bv[0][j] = 1;
            end
            assert(rd_hbm_edge_mask[0+: HBM_EDGE_MASK] == bv[0])
            else begin
                $error("rd_hbm_edge_mask output_three_times error! ");
                test_pass = 0;
            end
            assert(hbm_push_flag[0] == 1'b1)
            else begin
                $error("hbm_push_flag output_three_times error!");
                test_pass = 0;
            end
            assert(hbm_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("hbm_active_v_id output_three_times error!");
                test_pass = 0;
            end
            assert(hbm_active_v_value[0+: V_VALUE_WIDTH] == 1)
            else begin
                $error("hbm_active_v_value output_three_times error");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[0] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output_three_times error");
                test_pass = 0;
            end
            assert(rd_bram_edge_addr[0+: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_bram_edge_addr output_three_times error!");
                test_pass = 0;
            end
            assert(bram_push_flag[0] == 1'b0)
            else begin
                $error("bram_push_flag output_three_times error!");
                test_pass = 0;
            end
            assert(bram_active_v_id[0+: V_ID_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_id output_three_times error!");
                test_pass = 0;
            end
            assert(bram_active_v_value[0+: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("bram_active_v_value output_three_times error");
                test_pass = 0;
            end
            assert(rd_bram_edge_valid[0] == 1'b0)
            else begin
                $error("rd_bram_edge_valid output_three_times error");
                test_pass = 0;
            end
            assert(iteration_end[0] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end[0], 1'b0);
            end
            assert(iteration_end_valid[0] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", 0, iteration_end_valid[0], 1'b0);
                test_pass = 0;
            end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_OutputSingal_while_LocalOffsetValid_then_LocalOffsetIncrease");
    endtask
    
    task automatic test_PullFirstEdge_given_InputActiveId_then_OutputId();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if(i % 2 == 0)begin
                front_push_flag[i] = 0;
                front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
                front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
                front_active_v_pull_first_flag[i] = 1;
                front_active_v_valid[i] = 1;
                front_active_v_loffset[i * V_OFF_DWIDTH +: V_OFF_DWIDTH] = 0;
                front_active_v_roffset[i * V_OFF_DWIDTH +: V_OFF_DWIDTH] = i;
                front_offset_valid[i] = 1;
            end
        end
        
        // run once to write active id
        repeat (1) @(posedge ap_clk);
        
        // CHECK
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if (i % 2 == 0) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
            else begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        
        // run three clk to output
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        // check
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if (i % 2 == 0) begin
                    assert(stage_full[i] == 1'b0)
                    else begin
                        $error("stage_full inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                    else begin
                        $error("rd_hbm_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                    else begin
                        $error("rd_hbm_edge_mask inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_push_flag[i] == 1'b0)
                    else begin
                        $error("hbm_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                    else begin
                        $error("hbm_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                    else begin
                        $error("hbm_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_hbm_edge_valid[i] == 1'b0)
                    else begin
                        $error("rd_hbm_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == i/CORE_NUM)
                    else begin
                        $error("rd_bram_edge_addr inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_push_flag[i] == 1'b0)
                    else begin
                        $error("bram_push_flag inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                    else begin
                        $error("bram_active_v_id inputActiveId output error!");
                        test_pass = 0;
                    end
                    assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                    else begin
                        $error("bram_active_v_value inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(rd_bram_edge_valid[i] == 1'b1)
                    else begin
                        $error("rd_bram_edge_valid inputActiveId output error");
                        test_pass = 0;
                    end
                    assert(iteration_end[i] == 1'b0)
                    else begin
                        $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                    end
                    assert(iteration_end_valid[i] == 1'b0)
                    else begin
                        $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                        test_pass = 0;
                    end
            end
            else begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_mask[i * HBM_EDGE_MASK +: HBM_EDGE_MASK] == 'h0)
                else begin
                    $error("rd_hbm_edge_mask inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_push_flag[i] == 1'b0)
                else begin
                    $error("hbm_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(hbm_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("hbm_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_addr[i * FIRST_EDGE_BRAM_AWIDTH +: FIRST_EDGE_BRAM_AWIDTH] == 'h0)
                else begin
                    $error("rd_bram_edge_addr inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_push_flag[i] == 1'b0)
                else begin
                    $error("bram_push_flag inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_id inputActiveId write error!");
                    test_pass = 0;
                end
                assert(bram_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("bram_active_v_value inputActiveId write error");
                    test_pass = 0;
                end
                assert(rd_bram_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_bram_edge_valid inputActiveId write error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_PullFirstEdge_given_InputActiveId_then_OutputId");
    endtask
    
    rd_active_vertex_edge inst_M04_dut (
        .clk                            (ap_clk),
        .rst                            (ap_rst),
        .front_push_flag                (front_push_flag),
        .front_active_v_id              (front_active_v_id),
        .front_active_v_value           (front_active_v_value),
        .front_active_v_pull_first_flag (front_active_v_pull_first_flag),
        .front_active_v_valid           (front_active_v_valid),
        .front_active_v_loffset         (front_active_v_loffset),
        .front_active_v_roffset         (front_active_v_roffset),
        .front_offset_valid             (front_offset_valid),
        .front_iteration_end            (front_iteration_end),
        .front_iteration_end_valid      (front_iteration_end_valid),
        .combine_stage_full             (combine_stage_full),
        .bram_stage_full                (bram_stage_full),

        .next_rst                       (next_rst),
        .stage_full                     (stage_full),
        .rd_hbm_edge_addr               (rd_hbm_edge_addr),
        .rd_hbm_edge_mask               (rd_hbm_edge_mask),
        .hbm_push_flag                  (hbm_push_flag),
        .hbm_active_v_id                (hbm_active_v_id),
        .hbm_active_v_value             (hbm_active_v_value),
        .rd_hbm_edge_valid              (rd_hbm_edge_valid),
        .rd_bram_edge_addr              (rd_bram_edge_addr),
        .bram_push_flag                 (bram_push_flag),
        .bram_active_v_id               (bram_active_v_id),
        .bram_active_v_value            (bram_active_v_value),
        .rd_bram_edge_valid             (rd_bram_edge_valid),
        .iteration_end                  (iteration_end),
        .iteration_end_valid            (iteration_end_valid)
    );

    initial begin : Read_Active_Vertex_Edge_Test
        test_PushPull_given_Rst_then_LocalBufferInit();
        test_Push_given_InputActiveId_then_OutputId();
        test_Push_given_OutputSingal_while_LocalOffsetValid_then_LocalOffsetIncrease();
        test_PullFirstEdge_given_InputActiveId_then_OutputId();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule