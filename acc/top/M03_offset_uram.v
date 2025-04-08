`timescale 1ns / 1ps

`include "accelerator.vh"

module offset_uram #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    HBM_DWIDTH = 1024,
    HBM_AWIDTH = `HBM_AWIDTH,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    VTX_NUM_WIDTH = `VTX_NUM_WIDTH
) (
    input                                   clk,
    input [CORE_NUM - 1 : 0]                rst,
    input [31 : 0]                          vertex_num,
    input                                   initial_uram,
    input [1023 : 0]                        front_hbm_rd_data,
    input                                   front_hbm_rd_valid,
    input                                   front_hbm_rd_ready,
    input [CORE_NUM * V_OFF_AWIDTH - 1 : 0] front_rd_active_v_offset_addr,
    input [CORE_NUM - 1 : 0]                front_active_v_valid,

    output reg [CORE_NUM - 1 : 0]           next_rst,
    output reg [HBM_AWIDTH - 1 : 0]         hbm_rd_addr,
    output reg                              hbm_rd_valid,
    output reg                              hbm_rd_complete,
    output [CORE_NUM * V_OFF_DWIDTH - 1 : 0]uram_loffset,
    output [CORE_NUM * V_OFF_DWIDTH - 1 : 0]uram_roffset,
    output [CORE_NUM - 1 : 0]               uram_dvalid 
);

    reg [31 : 0] send_ct;
    reg [31 : 0] rv_ct;
    reg initial_ch;
    wire [2047 : 0] front_hbm_rd_data_tmp;
    wire [CORE_NUM - 1 : 0] front_hbm_rd_valid_tmp;

    always @ (posedge clk) begin
        if (rst) begin
            if (!initial_uram) begin
                // 初始地址按128B对齐
                hbm_rd_addr <= {HBM_AWIDTH{1'b1}} & 64'hffffffffffffff80;
                hbm_rd_valid <= 0;
                send_ct <= 0;
                rv_ct <= 0;
                hbm_rd_complete <= 0;
                initial_ch <= 0; // 0: 初始化前16个core，1: 初始化后16个core
            end else begin
                // rst=1, initial_uram=1, 进入传输状态
                if (front_hbm_rd_valid) begin
                    rv_ct <= rv_ct + 1;
                    initial_ch <= ~initial_ch;
                end
                // 要求offset存放模式为：st -> ed -> st -> ed, 这样迭代式的存放
                // vertex_num << 3：4B data_size * 2倍的offset数据
                if (!front_hbm_rd_ready || hbm_rd_addr == ((vertex_num << 3) & 64'hffffffffffffff80)) begin
                    hbm_rd_valid <= 0;
                    // 当停止地址发送时才允许判断是否结束
                    hbm_rd_complete <= send_ct == rv_ct;
                end else begin
                    // 每次自增cacheline
                    hbm_rd_addr <= hbm_rd_addr + 64'h0000000000000080;
                    hbm_rd_valid <= 1;
                    send_ct <= send_ct + 1;
                end
            end
        end else begin
            // 传输结束，重置状态信号
            hbm_rd_addr <= {HBM_AWIDTH{1'b1}} & 64'hffffffffffffff80;
            hbm_rd_valid <= 0;
            send_ct <= 0;
            rv_ct <= 0;
            hbm_rd_complete <= 0;
            initial_ch <= 0; // 0: 初始化前16个core，1: 初始化后16个core
        end
    end

    assign front_hbm_rd_data_tmp[1023 : 0] = front_hbm_rd_data;
    assign front_hbm_rd_data_tmp[2047 : 1024] = front_hbm_rd_data;
    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assign front_hbm_rd_valid_tmp[i] = i < 16 ? (!initial_ch && front_hbm_rd_valid) : (initial_ch && front_hbm_rd_valid);
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            // 由于每次只能取上来 32*4B 的数据，因此需要 16 个core轮流写入
            offset_uram_single #(.CORE_ID(i)) OFFSET_URAM_SINGLE (
                .clk                            (clk),
                .rst                            (rst),
                .initial_uram                   (initial_uram),
                .hbm_data                       (front_hbm_rd_data_tmp[(i + 1) * 64 - 1 : i * 64]),
                .hbm_valid                      (front_hbm_rd_valid_tmp[i]),
                .front_rd_active_v_offset_addr  (front_rd_active_v_offset_addr[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .front_active_v_valid           (front_active_v_valid[i]),
                
                .uram_loffset                   (uram_loffset[(i + 1) * V_OFF_DWIDTH - 1 : i * V_OFF_DWIDTH]),
                .uram_roffset                   (uram_roffset[(i + 1) * V_OFF_DWIDTH - 1 : i * V_OFF_DWIDTH]),
                .uram_dvalid                    (uram_dvalid[i])
            );
        end
    endgenerate

endmodule

module offset_uram_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    V_VALUE_WIDTH = `V_VALUE_WIDTH,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    URAM_DELAY = `URAM_DELAY,
    CORE_ID = 0
) (
    input                           clk,
    input                           rst,
    input                           initial_uram,
    input [64 - 1 : 0]              hbm_data,
    input                           hbm_valid,
    input [V_OFF_AWIDTH - 1 : 0]    front_rd_active_v_offset_addr,
    input                           front_active_v_valid,

    output [V_OFF_DWIDTH - 1 : 0]   uram_loffset,
    output [V_OFF_DWIDTH - 1 : 0]   uram_roffset,
    output                          uram_dvalid
);

    reg local_uram_valid [0 : URAM_DELAY - 1];
    reg [HBM_AWIDTH - 1 : 0]    local_uram_addr;

    integer i;
    always @ (posedge clk) begin
        local_uram_valid[0] <= front_active_v_valid;
        for (i = 0; i < URAM_DELAY - 1; i = i + 1) begin
            local_uram_valid[i + 1] <= local_uram_valid[i];
        end
    end
    assign uram_dvalid = local_uram_valid[URAM_DELAY - 1];

    always @ (posedge clk) begin
        if (rst) begin
            if (!initial_uram) begin
                local_uram_addr <= 0;
            end else begin
                if (hbm_valid) begin
                    local_uram_addr <= local_uram_addr + 1;
                end
            end
        end else begin
            local_uram_addr <= 0;
        end
    end

    off_uram OFF_URAM (
        .clk    (clk),
        .we     (hbm_valid),
        .mem_en (1'b1),
        .din    (hbm_data),
        .addr   (hbm_valid ? local_uram_addr : front_rd_active_v_offset_addr),

        .dout   ({uram_roffset, uram_loffset})
    );

endmodule

module off_uram #(parameter
    AWIDTH = `V_OFF_AWIDTH,
    DWIDTH = 2 * `V_OFF_DWIDTH,
    NBPIPE = `URAM_DELAY - 2
) (
    input clk,
    input we,
    input mem_en,
    input [DWIDTH - 1 : 0] din,
    input [AWIDTH - 1 : 0] addr,

    output reg [DWIDTH - 1 : 0] dout
);


    //  Xilinx UltraRAM Single Port Read First Mode.  This code implements 
    //  a parameterizable UltraRAM block in read first mode. The behavior of this RAM is 
    //  when data is written, the old memory contents at the write address are 
    //  presented on the output port.
    //
    (* ram_style = "ultra" *)
    reg [DWIDTH - 1:0] mem [(1 << AWIDTH) - 1 : 0];        // Memory Declaration
    reg [DWIDTH - 1:0] memreg;              
    reg [DWIDTH - 1:0] mem_pipe_reg [NBPIPE - 1 : 0];    // Pipelines for memory
    reg mem_en_pipe_reg[NBPIPE : 0];                // Pipelines for memory enable  

    integer          i;


/*
    initial begin
        for (i = 0; i < (1 << AWIDTH); i = i + 1) begin
            mem[i] <= i;
        end
    end
*/


    // RAM : Both READ and WRITE have a latency of one
    always @ (posedge clk) begin
       if(mem_en) begin
           if(we) begin
                mem[addr] <= din;
           end
           memreg <= mem[addr];
       end
    end

    // The enable of the RAM goes through a pipeline to produce a
    // series of pipelined enable signals required to control the data
    // pipeline.
    always @ (posedge clk) begin
        mem_en_pipe_reg[0] <= mem_en;
        for (i = 0; i < NBPIPE; i = i + 1) begin
            mem_en_pipe_reg[i + 1] <= mem_en_pipe_reg[i];
        end
    end

    // RAM output data goes through a pipeline.
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[0]) begin
            mem_pipe_reg[0] <= memreg;
        end
    end    

    always @ (posedge clk) begin
        for (i = 0; i < NBPIPE - 1; i = i + 1) begin
            if (mem_en_pipe_reg[i + 1]) begin
                mem_pipe_reg[i + 1] <= mem_pipe_reg[i];
            end
        end
    end      

    // Final output register gives user the option to add a reset and
    // an additional enable signal just for the data output
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[NBPIPE]) begin
            dout <= mem_pipe_reg[NBPIPE - 1];
        end
    end
						
endmodule