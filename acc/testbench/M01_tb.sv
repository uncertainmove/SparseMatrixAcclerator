`timescale 1ns / 1ps

`include "accelerator.vh"

module M01_tb ();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter BITMAP_COMPRESSED_LENGTH = `BITMAP_COMPRESSED_LENGTH;
    parameter BITMAP_COMPRESSED_LENGTH_WIDTH = `BITMAP_COMPRESSED_LENGTH_WIDTH;
    parameter VTX_NUM = `VTX_NUM;
    parameter BITMAP_COMPRESSED_NUM = `BITMAP_COMPRESSED_NUM;
    parameter BITMAP_COMPRESSED_NUM_WIDTH = `BITMAP_COMPRESSED_NUM_WIDTH;
    parameter ITERATION_DWIDTH = `ITERATION_WIDTH;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [V_ID_WIDTH - 1 : 0]              root_id;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]   backend_active_v_id;
    reg [CORE_NUM - 1 : 0]                backend_active_v_updated;
    reg [CORE_NUM - 1 : 0]                backend_active_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                backend_active_v_id_valid;
    reg [CORE_NUM - 1 : 0]                backend_iteration_end;
    reg [CORE_NUM - 1 : 0]                backend_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                next_stage_full;
    
    // module output
    wire [CORE_NUM - 1 : 0]                     next_rst;
    wire [CORE_NUM - 1 : 0]                     push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]        active_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]     active_v_value;
    wire [CORE_NUM - 1 : 0]                     active_v_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                     active_v_id_valid;
    wire [CORE_NUM - 1 : 0]                     iteration_end;
    wire [CORE_NUM - 1 : 0]                     iteration_end_valid;
    wire [CORE_NUM * ITERATION_DWIDTH - 1 : 0]  global_iteration_id;
    
    task automatic init_input_flag();
        backend_active_v_id = 'h0;
        backend_active_v_updated = 'h0;
        backend_active_v_pull_first_flag = 'h0;
        backend_active_v_id_valid = 'h0;
        backend_iteration_end = 'h0;
        backend_iteration_end_valid = 'h0;
        next_stage_full = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        @(negedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j,k,t;
    logic test_pass;
    
    task automatic test_Push_given_TrueRst_then_ActiveVidQueueNotEmpty();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id init error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value init error");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid init error");
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
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_TrueRst_then_ActiveVidQueueNotEmpty");
    endtask
    
    task automatic test_Pull_given_TrueRst_then_BitmapInitial();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id init error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value init error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid init error");
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
        
        init_input_flag();
        repeat (1) @(posedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id init error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value init error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid init error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_TrueRst_then_BitmapInitial");
    endtask
    
    task automatic test_Pull_given_InitBitmap_then_OutputNormal();
        root_id = 'h0;
        reset_module();
        init_input_flag();
        repeat (BITMAP_COMPRESSED_NUM+1) @(posedge ap_clk);
        test_pass = 1;
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id init error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value init error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag init error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid init error");
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
        
        init_input_flag();
        repeat (7) @(posedge ap_clk);
        @(negedge ap_clk)
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag output error!");
                test_pass = 0;
            end
            if(root_id % CORE_NUM == i)begin
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i+CORE_NUM)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
            end
            else begin
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
            else begin
                $error("active_v_value output error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b1)
            else begin
                $error("active_v_pull_first_flag output error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b1)
            else begin
                $error("active_v_id_valid output error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_InitBitmap_then_OutputNormal");
    endtask
    
    task automatic test_Pull_given_InitBitmap_then_VisIndexChanged();
        root_id = 'h0;
        reset_module();
        init_input_flag();
        repeat (BITMAP_COMPRESSED_NUM+1) @(posedge ap_clk);
        init_input_flag();
        repeat (6) @(posedge ap_clk);
        test_pass = 1;
        
        for(k = 0; k < BITMAP_COMPRESSED_LENGTH - 1; k = k + 1)begin
            @(posedge ap_clk);
            @(negedge ap_clk)
            //check
            for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                if(root_id % CORE_NUM == i)begin
                    assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i + CORE_NUM * (k+1))
                    else begin
                        $error("active_v_id output error!");
                        test_pass = 0;
                    end
                end
                else begin
                    assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i + CORE_NUM * k)
                    else begin
                        $error("active_v_id output error!");
                        test_pass = 0;
                    end
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_pull_first_flag[i] == 1'b1)
                else begin
                    $error("active_v_pull_first_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b1)
                else begin
                    $error("active_v_id_valid output error");
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
        
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk)
        i = root_id % CORE_NUM;
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag output error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id output error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value output error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag output error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid output error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_InitBitmap_then_VisIndexChanged");
    endtask
    
    task automatic test_Pull_given_VisIndexChanged_then_NormalOutput();
        root_id = 'h0;
        reset_module();
        init_input_flag();
        repeat (BITMAP_COMPRESSED_NUM+1) @(posedge ap_clk);
        init_input_flag();
        repeat (BITMAP_COMPRESSED_LENGTH+6) @(posedge ap_clk);
        test_pass = 1;
        
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk)
        i = root_id % CORE_NUM;
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag output error!");
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i + CORE_NUM * BITMAP_COMPRESSED_LENGTH)
            else begin
                $error("active_v_id output error!");
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h1)
            else begin
                $error("active_v_value output error");
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b1)
            else begin
                $error("active_v_pull_first_flag output error!");
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b1)
            else begin
                $error("active_v_id_valid output error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_VisIndexChanged_then_NormalOutput");
    endtask
    
    task automatic test_Push_given_FalseRst_then_InitBitmap();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        for(j = 0;j < BITMAP_COMPRESSED_NUM;j = j + 1)begin
            repeat (1) @(posedge ap_clk);
            @(negedge ap_clk);
            for(i = 0;i < CORE_NUM;i = i + 1)begin
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag init error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id init error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value init error");
                    test_pass = 0;
                end
                assert(active_v_pull_first_flag[i] == 1'b0)
                else begin
                    $error("active_v_pull_first_flag init error!");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b0)
                else begin
                    $error("active_v_id_valid init error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_FalseRst_then_InitBitmap");
    endtask
    
    task automatic test_Push_given_FalseRst_then_ActiveVidValid();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        repeat (BITMAP_COMPRESSED_NUM+1) @(posedge ap_clk);
        @(negedge ap_clk);
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            if(root_id == i)begin
                assert(push_flag[i] == 1'b1)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b1)
                else begin
                    $error("active_v_id_valid output error");
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
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b0)
                else begin
                    $error("active_v_id_valid output error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b1)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b1)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_FalseRst_then_ActiveVidValid");
    endtask
    
    task automatic test_Push_given_BackendActiveId_then_WriteQueueSuccess();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        repeat (BITMAP_COMPRESSED_NUM) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            if(i%2==1)begin
                backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
                backend_active_v_id_valid[i] = 1;
                backend_active_v_updated[i] = 1;
            end
        end 
        repeat (3) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            if(i%2==0)begin
                backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
                backend_active_v_id_valid[i] = 1;
                backend_active_v_updated[i] = 1;
            end
        end 
        repeat (1) @(posedge ap_clk);
        //check
        @(negedge ap_clk);
        for(i = 0;i < CORE_NUM;i = i + 1)begin
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b0)
                else begin
                    $error("active_v_id_valid output error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b1)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b1)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_BackendActiveId_then_WriteQueueSuccess");
    endtask
    
    task automatic test_Push_given_IterationEnd_then_SwitchIteration();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        repeat (BITMAP_COMPRESSED_NUM) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            backend_active_v_id_valid[i] = 1;
            backend_active_v_updated[i] = 1;
        end 
        repeat (3) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            backend_iteration_end[i] = 1;
            backend_iteration_end_valid[i] = 1;
        end 
        repeat (1) @(posedge ap_clk);
        //check
        @(negedge ap_clk);
        for(i = 0;i < CORE_NUM;i = i + 1)begin
                assert(push_flag[i] == 1'b1)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 2)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b1)
                else begin
                    $error("active_v_id_valid output error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_IterationEnd_then_SwitchIteration");
    endtask
    
    task automatic test_Push_given_BackStageFull_then_PartialOutput();
        root_id = 'h0;
        reset_module();
        test_pass = 1;
        repeat (BITMAP_COMPRESSED_NUM) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            backend_active_v_id_valid[i] = 1;
            backend_active_v_updated[i] = 1;
        end 
        repeat (3) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            backend_iteration_end[i] = 1;
            backend_iteration_end_valid[i] = 1;
        end 
        repeat (1) @(posedge ap_clk);
        init_input_flag();
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            if(i%2 == 0)begin
                next_stage_full[i] = 1;
            end
        end 
        repeat (1) @(posedge ap_clk);
        //check
        @(negedge ap_clk);
        for(i = 0;i < CORE_NUM;i = i + 1)begin
            if(i%2 == 0)begin
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b0)
                else begin
                    $error("active_v_id_valid output error");
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
                assert(push_flag[i] == 1'b1)
                else begin
                    $error("push_flag output error!");
                    test_pass = 0;
                end
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                else begin
                    $error("active_v_id output error!");
                    test_pass = 0;
                end
                assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 2)
                else begin
                    $error("active_v_value output error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b1)
                else begin
                    $error("active_v_id_valid output error");
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
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_BackStageFull_then_PartialOutput");
    endtask
    
    rd_active_vertex inst_M01_dut (
        .clk                                    (ap_clk),
        .rst                                    (ap_rst),
        .root_id                                (root_id),
        .backend_active_v_id                    (backend_active_v_id),
        .backend_active_v_updated               (backend_active_v_updated),
        .backend_active_v_pull_first_flag       (backend_active_v_pull_first_flag),
        .backend_active_v_id_valid              (backend_active_v_id_valid),
        .backend_iteration_end                  (backend_iteration_end),
        .backend_iteration_end_valid            (backend_iteration_end_valid),
        .next_stage_full                        (next_stage_full),

        .next_rst                               (next_rst),
        .push_flag                              (push_flag),
        .active_v_id                            (active_v_id),
        .active_v_value                         (active_v_value),
        .active_v_pull_first_flag               (active_v_pull_first_flag),
        .active_v_id_valid                      (active_v_id_valid),
        .iteration_end                          (iteration_end),
        .iteration_end_valid                    (iteration_end_valid),
        .global_iteration_id                    (global_iteration_id)
    );

    initial begin : rd_active_vertex_Test
    //change the MAX_TASK_NUM more than 32 before test
        test_Push_given_TrueRst_then_ActiveVidQueueNotEmpty();
        //test_Pull_given_TrueRst_then_BitmapInitial();
        //test_Pull_given_InitBitmap_then_OutputNormal();
        //test_Pull_given_InitBitmap_then_VisIndexChanged();
        //test_Pull_given_VisIndexChanged_then_NormalOutput();
        test_Push_given_FalseRst_then_InitBitmap();
        test_Push_given_FalseRst_then_ActiveVidValid();
        test_Push_given_BackendActiveId_then_WriteQueueSuccess();
        test_Push_given_IterationEnd_then_SwitchIteration();
        test_Push_given_BackStageFull_then_PartialOutput();
        repeat (10) @(posedge ap_clk);
        $finish;
    end
    
endmodule
