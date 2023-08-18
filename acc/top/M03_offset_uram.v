`timescale 1ns / 1ps

`include "accelerator.vh"

module offset_uram #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    V_OFF_AWIDTH = `V_OFF_AWIDTH,
    V_OFF_DWIDTH = `V_OFF_DWIDTH,
    HBM_DWIDTH = `HBM_DWIDTH,
    HBM_AWIDTH = `HBM_AWIDTH,
    PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
    HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH
) (
    input                               clk,
    input                               uram_rst,
    input [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0] hbm_controller_data,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]              hbm_data_valid,
    input [PSEUDO_CHANNEL_NUM - 1 : 0]              hbm_controller_full,
    input [CORE_NUM - 1 : 0]            rst,
    input [CORE_NUM * V_OFF_AWIDTH - 1 : 0]     front_rd_active_v_offset_addr,
    input [CORE_NUM - 1 : 0]                    front_active_v_valid,

    output reg [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]    hbm_controller_addr,
    output reg [PSEUDO_CHANNEL_NUM - 1 : 0]                 hbm_addr_valid,
    output [CORE_NUM - 1 : 0]                               transfer_complete,
    output reg [CORE_NUM - 1 : 0]               next_rst,
    output [CORE_NUM * V_OFF_DWIDTH - 1 : 0]    uram_loffset,
    output [CORE_NUM * V_OFF_DWIDTH - 1 : 0]    uram_roffset,
    output [CORE_NUM - 1 : 0]                   uram_dvalid 
);

    generate
        genvar i;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                if (uram_rst) begin
                    hbm_controller_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH] <= {HBM_AWIDTH{1'b1}};
                    hbm_addr_valid[i] <= 0;
                end else if (hbm_controller_full[i] || hbm_controller_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH] == (((1 << 15) >> CORE_NUM_WIDTH)-1)) begin
                    hbm_addr_valid[i] <= 0;
                end else begin
                    hbm_controller_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH] <= hbm_controller_addr[(i + 1) * HBM_AWIDTH - 1 : i * HBM_AWIDTH] + 1;
                    hbm_addr_valid[i] <= 1;
                end
            end
        end
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst[i];
            end
            offset_uram_single #(.CORE_ID(i)) OFFSET_URAM_SINGLE (
                .clk                            (clk),
                .rst                            (uram_rst),
                .hbm_controller_data            (hbm_controller_data[(i + 1) * HBM_DWIDTH_PER_CORE - 1 : i * HBM_DWIDTH_PER_CORE]),
                .hbm_data_valid                 (hbm_data_valid[i >> 4]),
                .front_rd_active_v_offset_addr  (front_rd_active_v_offset_addr[(i + 1) * V_OFF_AWIDTH - 1 : i * V_OFF_AWIDTH]),
                .front_active_v_valid           (front_active_v_valid[i]),
                
                .transfer_complete              (transfer_complete[i]),
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
    CORE_ID = 0
) (
    input                           clk,
    input                           rst,
    input [HBM_DWIDTH_PER_CORE - 1 : 0] hbm_controller_data,
    input                               hbm_data_valid,
    input [V_OFF_AWIDTH - 1 : 0]    front_rd_active_v_offset_addr,
    input                           front_active_v_valid,

    output                          transfer_complete,
    output [V_OFF_DWIDTH - 1 : 0]   uram_loffset,
    output [V_OFF_DWIDTH - 1 : 0]   uram_roffset,
    output reg                      uram_dvalid
);

    // CHECK four clk delay (change from 2 to 3)
    reg local_uram_valid [0 : 3];
    reg [V_OFF_DWIDTH - 1 : 0]  local_hbm_controller_data;
    reg                         local_hbm_data_valid;
    reg [HBM_AWIDTH - 1 : 0]    local_hbm_controller_addr;
    integer i;

    assign transfer_complete = (local_hbm_controller_addr == ((1 << 14) >> CORE_NUM_WIDTH));

    always @ (posedge clk) begin
        local_uram_valid[0] <= front_active_v_valid;
        for (i = 0; i < 3; i = i + 1) begin
            local_uram_valid[i + 1] <= local_uram_valid[i];
        end
        uram_dvalid <= local_uram_valid[3];
    end

    always @ (posedge clk) begin
        if (rst) begin
            local_hbm_controller_addr <= 0;
            local_hbm_controller_data <= 0;
            local_hbm_data_valid <= 0;
        end else begin
            if (hbm_data_valid && !local_hbm_data_valid) begin
                local_hbm_controller_data <= hbm_controller_data[V_OFF_DWIDTH - 1 : 0];
                local_hbm_data_valid <= hbm_data_valid;
            end else if (hbm_data_valid && local_hbm_data_valid) begin
                local_hbm_controller_data <= 0;
                local_hbm_data_valid <= 0;
                local_hbm_controller_addr <= local_hbm_controller_addr + 1;
            end
        end
    end

    off_uram OFF_URAM (
        .clk    (clk),
        .we     (hbm_data_valid && local_hbm_data_valid),
        .mem_en (1'b1),
        .din    ({local_hbm_controller_data, hbm_controller_data[V_OFF_DWIDTH - 1 : 0]}),
        .addr   (hbm_data_valid && local_hbm_data_valid ? local_hbm_controller_addr : front_rd_active_v_offset_addr),

        .dout   ({uram_loffset, uram_roffset})
    );

endmodule

module off_uram #(parameter
    AWIDTH = `V_OFF_AWIDTH,
    DWIDTH = 2 * `V_OFF_DWIDTH,
    NBPIPE = 3
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
    // an additional enable signal just for the data ouptut
    always @ (posedge clk) begin
        if (mem_en_pipe_reg[NBPIPE]) begin
            dout <= mem_pipe_reg[NBPIPE - 1];
        end
    end
						
endmodule