// This is a generated file. Use and modify at your own risk.
//////////////////////////////////////////////////////////////////////////////// 
// default_nettype of none prevents implicit wire declaration.
`include "accelerator.vh"
`default_nettype none
module delta_pr_accelerator_axi_top #(
  parameter integer C_M00_AXI_ADDR_WIDTH = 64  ,
  parameter integer C_M00_AXI_DATA_WIDTH = 1024,
  parameter integer C_M01_AXI_ADDR_WIDTH = 64  ,
  parameter integer C_M01_AXI_DATA_WIDTH = 512 ,
  parameter integer C_M02_AXI_ADDR_WIDTH = 64  ,
  parameter integer C_M02_AXI_DATA_WIDTH = 512 ,
  parameter integer C_M03_AXI_ADDR_WIDTH = 64  ,
  parameter integer C_M03_AXI_DATA_WIDTH = 1024,
  // acc parameter
  parameter integer V_ID_WIDTH = `V_ID_WIDTH,
  parameter integer V_OFF_AWIDTH = `V_OFF_AWIDTH,
  parameter integer V_OFF_DWIDTH = `V_OFF_DWIDTH,
  parameter integer V_VALUE_WIDTH = `V_VALUE_WIDTH,
  parameter integer CORE_NUM = `CORE_NUM,
  parameter integer CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
  parameter integer ITERATION_WIDTH = `ITERATION_WIDTH,
  parameter integer HBM_AWIDTH = `HBM_AWIDTH,
  parameter integer HBM_DWIDTH = `HBM_DWIDTH,
  parameter integer HBM_DWIDTH_PER_CORE = `HBM_DWIDTH_PER_CORE,
  parameter integer HBM_EDGE_MASK = `HBM_EDGE_MASK,
  parameter integer PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM,
  parameter integer GROUP_CORE_NUM = `GROUP_CORE_NUM,
  parameter integer DELTA_BRAM_AWIDTH = `DELTA_BRAM_AWIDTH
)
(
  // System Signals
  input  wire                              ap_clk         ,
  input  wire                              ap_rst_n       ,
  // AXI4 master interface m00_axi
  output wire                              m00_axi_awvalid,
  input  wire                              m00_axi_awready,
  output wire [C_M00_AXI_ADDR_WIDTH-1:0]   m00_axi_awaddr ,
  output wire [8-1:0]                      m00_axi_awlen  ,
  output wire                              m00_axi_wvalid ,
  input  wire                              m00_axi_wready ,
  output wire [C_M00_AXI_DATA_WIDTH-1:0]   m00_axi_wdata  ,
  output wire [C_M00_AXI_DATA_WIDTH/8-1:0] m00_axi_wstrb  ,
  output wire                              m00_axi_wlast  ,
  input  wire                              m00_axi_bvalid ,
  output wire                              m00_axi_bready ,
  output wire                              m00_axi_arvalid,
  input  wire                              m00_axi_arready,
  output wire [C_M00_AXI_ADDR_WIDTH-1:0]   m00_axi_araddr ,
  output wire [8-1:0]                      m00_axi_arlen  ,
  input  wire                              m00_axi_rvalid ,
  output wire                              m00_axi_rready ,
  input  wire [C_M00_AXI_DATA_WIDTH-1:0]   m00_axi_rdata  ,
  input  wire                              m00_axi_rlast  ,
  // AXI4 master interface m01_axi
  output wire                              m01_axi_awvalid,
  input  wire                              m01_axi_awready,
  output wire [C_M01_AXI_ADDR_WIDTH-1:0]   m01_axi_awaddr ,
  output wire [8-1:0]                      m01_axi_awlen  ,
  output wire                              m01_axi_wvalid ,
  input  wire                              m01_axi_wready ,
  output wire [C_M01_AXI_DATA_WIDTH-1:0]   m01_axi_wdata  ,
  output wire [C_M01_AXI_DATA_WIDTH/8-1:0] m01_axi_wstrb  ,
  output wire                              m01_axi_wlast  ,
  input  wire                              m01_axi_bvalid ,
  output wire                              m01_axi_bready ,
  output wire                              m01_axi_arvalid,
  input  wire                              m01_axi_arready,
  output wire [C_M01_AXI_ADDR_WIDTH-1:0]   m01_axi_araddr ,
  output wire [8-1:0]                      m01_axi_arlen  ,
  input  wire                              m01_axi_rvalid ,
  output wire                              m01_axi_rready ,
  input  wire [C_M01_AXI_DATA_WIDTH-1:0]   m01_axi_rdata  ,
  input  wire                              m01_axi_rlast  ,
  // AXI4 master interface m02_axi
  output wire                              m02_axi_awvalid,
  input  wire                              m02_axi_awready,
  output wire [C_M02_AXI_ADDR_WIDTH-1:0]   m02_axi_awaddr ,
  output wire [8-1:0]                      m02_axi_awlen  ,
  output wire                              m02_axi_wvalid ,
  input  wire                              m02_axi_wready ,
  output wire [C_M02_AXI_DATA_WIDTH-1:0]   m02_axi_wdata  ,
  output wire [C_M02_AXI_DATA_WIDTH/8-1:0] m02_axi_wstrb  ,
  output wire                              m02_axi_wlast  ,
  input  wire                              m02_axi_bvalid ,
  output wire                              m02_axi_bready ,
  output wire                              m02_axi_arvalid,
  input  wire                              m02_axi_arready,
  output wire [C_M02_AXI_ADDR_WIDTH-1:0]   m02_axi_araddr ,
  output wire [8-1:0]                      m02_axi_arlen  ,
  input  wire                              m02_axi_rvalid ,
  output wire                              m02_axi_rready ,
  input  wire [C_M02_AXI_DATA_WIDTH-1:0]   m02_axi_rdata  ,
  input  wire                              m02_axi_rlast  ,
  // AXI4 master interface m03_axi
  output wire                              m03_axi_awvalid,
  input  wire                              m03_axi_awready,
  output wire [C_M03_AXI_ADDR_WIDTH-1:0]   m03_axi_awaddr ,
  output wire [8-1:0]                      m03_axi_awlen  ,
  output wire                              m03_axi_wvalid ,
  input  wire                              m03_axi_wready ,
  output wire [C_M03_AXI_DATA_WIDTH-1:0]   m03_axi_wdata  ,
  output wire [C_M03_AXI_DATA_WIDTH/8-1:0] m03_axi_wstrb  ,
  output wire                              m03_axi_wlast  ,
  input  wire                              m03_axi_bvalid ,
  output wire                              m03_axi_bready ,
  output wire                              m03_axi_arvalid,
  input  wire                              m03_axi_arready,
  output wire [C_M03_AXI_ADDR_WIDTH-1:0]   m03_axi_araddr ,
  output wire [8-1:0]                      m03_axi_arlen  ,
  input  wire                              m03_axi_rvalid ,
  output wire                              m03_axi_rready ,
  input  wire [C_M03_AXI_DATA_WIDTH-1:0]   m03_axi_rdata  ,
  input  wire                              m03_axi_rlast  ,
  // Control Signals
  input  wire                              ap_start       ,
  output wire                              ap_idle        ,
  output wire                              ap_done        ,
  output wire                              ap_ready       ,
  input  wire [32-1:0]                     iteration_num  ,
  input  wire [32-1:0]                     vertex_num     ,
  input  wire [32-1:0]                     edge_num       ,
  input  wire [64-1:0]                     axi00_ptr0     ,
  input  wire [64-1:0]                     axi01_ptr0     ,
  input  wire [64-1:0]                     axi02_ptr0     ,
  input  wire [64-1:0]                     axi03_ptr0     
);


timeunit 1ps;
timeprecision 1ps;

///////////////////////////////////////////////////////////////////////////////
// Local Parameters
///////////////////////////////////////////////////////////////////////////////
// Large enough for interesting traffic.
localparam integer  LP_NUM_EXAMPLES    = 5;

///////////////////////////////////////////////////////////////////////////////
// Wires and Variables
///////////////////////////////////////////////////////////////////////////////
(* KEEP = "yes" *)
logic                                areset                         = 1'b0;
logic                                ap_start_r                     = 1'b0;
logic                                ap_idle_r                      = 1'b1;
logic                                ap_start_pulse                ;
logic [LP_NUM_EXAMPLES-1:0]          ap_done_i                     ;
logic [LP_NUM_EXAMPLES-1:0]          ap_done_r                      = {LP_NUM_EXAMPLES{1'b0}};
logic [32-1:0]                       ctrl_constant                  = 32'd1;
// kernel
wire [CORE_NUM - 1 : 0]                 P1_iteration_done;
wire [CORE_NUM - 1 : 0]                 P1_next_rst;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P1_active_v_id;
wire [CORE_NUM - 1 : 0]                 P1_active_v_valid;
wire [CORE_NUM - 1 : 0]                 P1_iteration_end;
wire [CORE_NUM - 1 : 0]                 P1_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0]  P1_iteration_id;

wire [CORE_NUM - 1 : 0]                 P2_next_rst;
wire [CORE_NUM - 1 : 0]                 P2_stage_full;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P2_active_v_id;
wire [CORE_NUM - 1 : 0]                 P2_active_v_id_valid;
wire [CORE_NUM * V_OFF_AWIDTH - 1 : 0]  P2_rd_active_v_offset_addr;
wire [CORE_NUM * DELTA_BRAM_AWIDTH - 1 : 0]  P2_rd_active_v_value_addr;
wire [CORE_NUM - 1 : 0]                 P2_rd_active_v_addr_valid;
wire [CORE_NUM - 1 : 0]                 P2_iteration_end;
wire [CORE_NUM - 1 : 0]                 P2_iteration_end_valid;
wire [CORE_NUM *ITERATION_WIDTH - 1 : 0]P2_iteration_id;

wire                                    P3_initial_complete;
wire [CORE_NUM - 1 : 0]                 P3_next_rst;
wire [CORE_NUM * V_OFF_DWIDTH - 1 : 0]  P3_active_v_loffset;
wire [CORE_NUM * V_OFF_DWIDTH - 1 : 0]  P3_active_v_roffset;
wire [CORE_NUM - 1 : 0]                 P3_active_v_valid;

wire [CORE_NUM - 1 : 0]                 P4_next_rst;
wire [CORE_NUM - 1 : 0]                 P4_stage_full;
wire [CORE_NUM * HBM_AWIDTH - 1 : 0]    P4_rd_hbm_edge_addr;
wire [CORE_NUM * HBM_EDGE_MASK - 1 : 0] P4_rd_hbm_edge_mask;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P4_hbm_active_v_id;
wire [CORE_NUM - 1 : 0]                 P4_rd_hbm_edge_valid;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0] P4_hbm_active_v_value;
wire [CORE_NUM - 1 : 0]                 P4_hbm_active_v_value_valid;
wire [CORE_NUM - 1 : 0]                 P4_iteration_end;
wire [CORE_NUM - 1 : 0]                 P4_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0]  P4_iteration_id;

wire [CORE_NUM - 1 : 0]                 P5_next_rst;
wire [CORE_NUM - 1 : 0]                 P5_stage_full;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P5_active_v_id;
wire [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]    P5_rd_hbm_edge_addr;
wire [PSEUDO_CHANNEL_NUM - 1 : 0]                 P5_rd_hbm_edge_valid;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0] P5_active_v_value;
wire [CORE_NUM - 1 : 0]                 P5_active_v_valid;
wire [CORE_NUM - 1 : 0]                 P5_iteration_end;
wire [CORE_NUM - 1 : 0]                 P5_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0]  P5_iteration_id;

wire [PSEUDO_CHANNEL_NUM - 1 : 0]               HBM_interface_full;
wire [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0]  HBM_interface_active_v_edge;
wire [CORE_NUM - 1 : 0]               HBM_interface_active_v_edge_valid;

wire [C_M00_AXI_DATA_WIDTH - 1 : 0]       M00_AXI_rd_tdata;
wire                                      M00_AXI_rd_tvalid;
wire                                      M00_AXI_rd_tready;
wire [C_M00_AXI_ADDR_WIDTH - 1 : 0]       M00_AXI_rd_addr;
wire                                      M00_AXI_rd_valid;
wire                                      M00_AXI_rd_ready;
wire [C_M00_AXI_ADDR_WIDTH - 1 : 0]       M00_AXI_wr_addr;
wire [C_M00_AXI_DATA_WIDTH - 1 : 0]       M00_AXI_wr_data;
wire                                      M00_AXI_wr_valid;
wire                                      M00_AXI_wr_ready;
wire                                      M00_AXI_wr_trsp;

wire [C_M01_AXI_DATA_WIDTH - 1 : 0]       M01_AXI_rd_tdata;
wire                                      M01_AXI_rd_tvalid;
wire                                      M01_AXI_rd_tready;
wire [C_M01_AXI_ADDR_WIDTH - 1 : 0]       M01_AXI_rd_addr;
wire                                      M01_AXI_rd_valid;
wire                                      M01_AXI_rd_ready;
wire [C_M01_AXI_ADDR_WIDTH - 1 : 0]       M01_AXI_wr_addr;
wire [C_M01_AXI_DATA_WIDTH - 1 : 0]       M01_AXI_wr_data;
wire                                      M01_AXI_wr_valid;
wire                                      M01_AXI_wr_ready;
wire                                      M01_AXI_wr_trsp;

wire [C_M02_AXI_DATA_WIDTH - 1 : 0]       M02_AXI_rd_tdata;
wire                                      M02_AXI_rd_tvalid;
wire                                      M02_AXI_rd_tready;
wire [C_M02_AXI_ADDR_WIDTH - 1 : 0]       M02_AXI_rd_addr;
wire                                      M02_AXI_rd_valid;
wire                                      M02_AXI_rd_ready;
wire [C_M02_AXI_ADDR_WIDTH - 1 : 0]       M02_AXI_wr_addr;
wire [C_M02_AXI_DATA_WIDTH - 1 : 0]       M02_AXI_wr_data;
wire                                      M02_AXI_wr_valid;
wire                                      M02_AXI_wr_ready;
wire                                      M02_AXI_wr_trsp;

wire [C_M03_AXI_DATA_WIDTH - 1 : 0]       M03_AXI_rd_tdata;
wire                                      M03_AXI_rd_tvalid;
wire                                      M03_AXI_rd_tready;
wire [C_M03_AXI_ADDR_WIDTH - 1 : 0]       M03_AXI_rd_addr;
wire                                      M03_AXI_rd_valid;
wire                                      M03_AXI_rd_ready;
wire [C_M03_AXI_ADDR_WIDTH - 1 : 0]       M03_AXI_wr_addr;
wire [C_M03_AXI_DATA_WIDTH - 1 : 0]       M03_AXI_wr_data;
wire                                      M03_AXI_wr_valid;
wire                                      M03_AXI_wr_ready;
wire                                      M03_AXI_wr_trsp;

wire [CORE_NUM - 1 : 0]                 P7_next_rst;
wire [PSEUDO_CHANNEL_NUM - 1 : 0]       P7_stage_full;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P7_update_v_id;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0] P7_update_v_value;
wire [CORE_NUM - 1 : 0]                 P7_update_v_valid;
wire [CORE_NUM - 1 : 0]                 P7_iteration_end;
wire [CORE_NUM - 1 : 0]                 P7_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0]  P7_iteration_id;

wire [CORE_NUM - 1 : 0]                 P8_next_rst;
wire [CORE_NUM - 1 : 0]                 P8_stage_full;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    P8_update_v_id;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0] P8_update_v_value;
wire [CORE_NUM - 1 : 0]                 P8_update_v_valid;
wire [CORE_NUM - 1 : 0]                 P8_iteration_end;
wire [CORE_NUM - 1 : 0]                 P8_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0]  P8_iteration_id;

wire [CORE_NUM - 1 : 0]                   P9_next_rst;
wire [CORE_NUM - 1 : 0]                   P9_dest_core_full;
wire [CORE_NUM * V_OFF_AWIDTH - 1 : 0]    P9_wr_vertex_bram_addr;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   P9_wr_vertex_bram_data;
wire [CORE_NUM - 1 : 0]                   P9_wr_vertex_bram_valid;
wire [CORE_NUM - 1 : 0]                   P9_wr_vertex_bram_iteration_end;
wire [CORE_NUM - 1 : 0]                   P9_wr_vertex_bram_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0] P9_wr_vertex_bram_iteration_id;

wire [CORE_NUM - 1 : 0]                   P10_next_rst;
wire                                      P10_transfer_done;
wire                                      P10_initial_done;
wire [CORE_NUM - 1 : 0]                   P10_stage_full;
wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   P10_src_recv_update_v_value;
wire [CORE_NUM - 1 : 0]                   P10_src_recv_update_v_valid;
wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      P10_backend_active_v_id;
wire [CORE_NUM - 1 : 0]                   P10_backend_active_v_updated;
wire [CORE_NUM - 1 : 0]                   P10_backend_active_v_valid;
wire [CORE_NUM - 1 : 0]                   P10_iteration_end;
wire [CORE_NUM - 1 : 0]                   P10_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0] P10_iteration_id;

wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      P11_active_v_id;
wire [CORE_NUM - 1 : 0]                   P11_active_v_updated;
wire [CORE_NUM - 1 : 0]                   P11_active_v_valid;
wire [CORE_NUM - 1 : 0]                   P11_iteration_end;
wire [CORE_NUM - 1 : 0]                   P11_iteration_end_valid;
wire [CORE_NUM * ITERATION_WIDTH - 1 : 0] P11_iteration_id;

reg rst, cycle_valid;
reg initial_uram, initial_ram, transfer_ram;
reg [31 : 0] acc_cycle;
reg [2 : 0] acc_state; // 00: initial state; 01: transfer uram data; 10: running; 11: done

(* keep = "TRUE" *)   wire          clk = ap_clk;

always @ (posedge clk) begin
  if (areset) begin
    rst <= 1'b1;
    cycle_valid <= 0;
    initial_uram <= 0;
    initial_ram <= 0;
    transfer_ram <= 0;
    acc_state <= 2'b00;
  end else begin
    case (acc_state)
      2'b00: if (ap_start_pulse) begin
        rst <= 1'b1;
        cycle_valid <= 0;
        initial_uram <= 1;
        initial_ram <= 1;
        transfer_ram <= 0;
        acc_state <= 2'b01;
      end
      2'b01: if (P3_initial_complete && P10_initial_done) begin
        rst <= 1'b0;
        cycle_valid <= 0;
        initial_uram <= 0;
        initial_ram <= 0;
        transfer_ram <= 0;
        acc_state <= 2'b10;
      end
      2'b10: if (P1_iteration_done == {CORE_NUM{1'b1}}) begin
        rst <= 1'b0;
        cycle_valid <= 1;
        initial_uram <= 0;
        initial_ram <= 0;
        transfer_ram <= 1;
        acc_state <= 2'b11;
      end
      2'b11: begin
        cycle_valid <= 0;
        if (P10_transfer_done) begin
          // keep done signal
          rst <= 1'b1;
          initial_uram <= 0;
          initial_ram <= 0;
          transfer_ram <= 0;
          acc_state <= 2'b11;
        end
      end
    endcase
  end
end

// BUFG u_AXI_CLK_IN_0  ( .I (ap_clk), .O (clk) );

///////////////////////////////////////////////////////////////////////////////
// Begin RTL
///////////////////////////////////////////////////////////////////////////////

//cycle inc
always @(posedge ap_clk) begin
  if (areset) begin
    acc_cycle <= 0;
  end else begin
    if (acc_state == 2'b10) begin
      acc_cycle <= acc_cycle + 1;
    end
  end
end

// Register and invert reset signal.
always @(posedge ap_clk) begin
  areset <= ~ap_rst_n;
end

// create pulse when ap_start transitions to 1
always @(posedge ap_clk) begin
  begin
    ap_start_r <= ap_start;
  end
end

assign ap_start_pulse = ap_start & ~ap_start_r;

// ap_idle is asserted when done is asserted, it is de-asserted when ap_start_pulse
// is asserted
always @(posedge ap_clk) begin
  if (areset) begin
    ap_idle_r <= 1'b1;
  end
  else begin
    ap_idle_r <= ap_done ? 1'b1 :
      ap_start_pulse ? 1'b0 : ap_idle;
  end
end

assign ap_idle = ap_idle_r;

// Done logic
always @(posedge ap_clk) begin
  if (areset) begin
    ap_done_r <= '0;
  end
  else begin
    ap_done_r <= (ap_done) ? '0 : ap_done_r | ap_done_i;
  end
end

assign ap_done = &ap_done_r && (acc_state == 2'b11) && P10_transfer_done;
assign ap_done_i[4] = acc_state == 2'b11;

// Ready Logic (non-pipelined case)
assign ap_ready = ap_done;

// HBM controller
mem_axi_control #(
  .C_M_AXI_ADDR_WIDTH ( C_M00_AXI_ADDR_WIDTH ),
  .C_M_AXI_DATA_WIDTH ( C_M00_AXI_DATA_WIDTH ),
  .C_ADDER_BIT_WIDTH  ( 32                   )
)
mem_axi_control_read_off (
  .aclk                    ( ap_clk                  ),
  .areset                  ( areset                  ),
  .kernel_clk              ( ap_clk                  ),
  .kernel_rst              ( areset                  ),
  .ctrl_addr_offset        ( axi00_ptr0              ),
  .ctrl_constant           ( ctrl_constant           ),
  .ap_start                ( ap_start_pulse          ),
  .m_axi_awvalid           ( m00_axi_awvalid         ),
  .m_axi_awready           ( m00_axi_awready         ),
  .m_axi_awaddr            ( m00_axi_awaddr          ),
  .m_axi_awlen             ( m00_axi_awlen           ),
  .m_axi_wvalid            ( m00_axi_wvalid          ),
  .m_axi_wready            ( m00_axi_wready          ),
  .m_axi_wdata             ( m00_axi_wdata           ),
  .m_axi_wstrb             ( m00_axi_wstrb           ),
  .m_axi_wlast             ( m00_axi_wlast           ),
  .m_axi_bvalid            ( m00_axi_bvalid          ),
  .m_axi_bready            ( m00_axi_bready          ),
  .m_axi_arvalid           ( m00_axi_arvalid         ),
  .m_axi_arready           ( m00_axi_arready         ),
  .m_axi_araddr            ( m00_axi_araddr          ),
  .m_axi_arlen             ( m00_axi_arlen           ),
  .m_axi_rvalid            ( m00_axi_rvalid          ),
  .m_axi_rready            ( m00_axi_rready          ),
  .m_axi_rdata             ( m00_axi_rdata           ),
  .m_axi_rlast             ( m00_axi_rlast           ),
  // acc
  .rd_addr                 (M00_AXI_rd_addr),
  .rd_valid                (M00_AXI_rd_valid),
  .rd_ready                (M00_AXI_rd_ready), // to acc
  .rd_tdata                (M00_AXI_rd_tdata),
  .rd_tvalid               (M00_AXI_rd_tvalid),
  .rd_tready               (M00_AXI_rd_tready), // 默认返回缓冲区足够
  .wr_data                 (M00_AXI_wr_data),
  .wr_addr                 (M00_AXI_wr_addr),
  .wr_valid                (M00_AXI_wr_valid),
  .wr_ready                (M00_AXI_wr_ready),
  .wr_rsp                  (M00_AXI_wr_trsp),
  .complete                (ap_done_i[0])
);
assign M00_AXI_wr_addr = {C_M00_AXI_ADDR_WIDTH{1'b0}};
assign M00_AXI_wr_data = {{992{1'b0}}, acc_cycle};
assign M00_AXI_wr_valid = cycle_valid;
assign M00_AXI_rd_tready = 1'b1;

// HBM controller
mem_axi_control #(
  .C_M_AXI_ADDR_WIDTH ( C_M01_AXI_ADDR_WIDTH ),
  .C_M_AXI_DATA_WIDTH ( C_M01_AXI_DATA_WIDTH ),
  .C_ADDER_BIT_WIDTH  ( 32                   )
)
mem_axi_control_read_edge0 (
  .aclk                    ( ap_clk                  ),
  .areset                  ( areset                  ),
  .kernel_clk              ( ap_clk                  ),
  .kernel_rst              ( areset                  ),
  .ctrl_addr_offset        ( axi01_ptr0              ),
  .ctrl_constant           ( ctrl_constant           ),
  .ap_start                ( ap_start_pulse          ),
  .m_axi_awvalid           ( m01_axi_awvalid         ),
  .m_axi_awready           ( m01_axi_awready         ),
  .m_axi_awaddr            ( m01_axi_awaddr          ),
  .m_axi_awlen             ( m01_axi_awlen           ),
  .m_axi_wvalid            ( m01_axi_wvalid          ),
  .m_axi_wready            ( m01_axi_wready          ),
  .m_axi_wdata             ( m01_axi_wdata           ),
  .m_axi_wstrb             ( m01_axi_wstrb           ),
  .m_axi_wlast             ( m01_axi_wlast           ),
  .m_axi_bvalid            ( m01_axi_bvalid          ),
  .m_axi_bready            ( m01_axi_bready          ),
  .m_axi_arvalid           ( m01_axi_arvalid         ),
  .m_axi_arready           ( m01_axi_arready         ),
  .m_axi_araddr            ( m01_axi_araddr          ),
  .m_axi_arlen             ( m01_axi_arlen           ),
  .m_axi_rvalid            ( m01_axi_rvalid          ),
  .m_axi_rready            ( m01_axi_rready          ),
  .m_axi_rdata             ( m01_axi_rdata           ),
  .m_axi_rlast             ( m01_axi_rlast           ),
  // acc
  .rd_addr                 (M01_AXI_rd_addr),
  .rd_valid                (M01_AXI_rd_valid),
  .rd_ready                (M01_AXI_rd_ready), // to acc
  .rd_tdata                (M01_AXI_rd_tdata),
  .rd_tvalid               (M01_AXI_rd_tvalid),
  .rd_tready               (M01_AXI_rd_tready), // 默认返回缓冲区足够
  .wr_data                 (M01_AXI_wr_data),
  .wr_addr                 (M01_AXI_wr_addr),
  .wr_valid                (M01_AXI_wr_valid),
  .wr_ready                (M01_AXI_wr_ready),
  .wr_rsp                  (M01_AXI_wr_trsp),
  .complete                (ap_done_i[1])
);
assign M01_AXI_wr_addr = {C_M01_AXI_ADDR_WIDTH{1'b0}};
assign M01_AXI_wr_data = {C_M01_AXI_DATA_WIDTH{1'b0}};
assign M01_AXI_wr_valid = 1'b0;
assign M01_AXI_rd_tready = 1'b1;

// HBM controller
mem_axi_control #(
  .C_M_AXI_ADDR_WIDTH ( C_M02_AXI_ADDR_WIDTH ),
  .C_M_AXI_DATA_WIDTH ( C_M02_AXI_DATA_WIDTH ),
  .C_ADDER_BIT_WIDTH  ( 32                   )
)
mem_axi_control_read_edge1 (
  .aclk                    ( ap_clk                  ),
  .areset                  ( areset                  ),
  .kernel_clk              ( ap_clk                  ),
  .kernel_rst              ( areset                  ),
  .ctrl_addr_offset        ( axi02_ptr0              ),
  .ctrl_constant           ( ctrl_constant           ),
  .ap_start                ( ap_start_pulse          ),
  .m_axi_awvalid           ( m02_axi_awvalid         ),
  .m_axi_awready           ( m02_axi_awready         ),
  .m_axi_awaddr            ( m02_axi_awaddr          ),
  .m_axi_awlen             ( m02_axi_awlen           ),
  .m_axi_wvalid            ( m02_axi_wvalid          ),
  .m_axi_wready            ( m02_axi_wready          ),
  .m_axi_wdata             ( m02_axi_wdata           ),
  .m_axi_wstrb             ( m02_axi_wstrb           ),
  .m_axi_wlast             ( m02_axi_wlast           ),
  .m_axi_bvalid            ( m02_axi_bvalid          ),
  .m_axi_bready            ( m02_axi_bready          ),
  .m_axi_arvalid           ( m02_axi_arvalid         ),
  .m_axi_arready           ( m02_axi_arready         ),
  .m_axi_araddr            ( m02_axi_araddr          ),
  .m_axi_arlen             ( m02_axi_arlen           ),
  .m_axi_rvalid            ( m02_axi_rvalid          ),
  .m_axi_rready            ( m02_axi_rready          ),
  .m_axi_rdata             ( m02_axi_rdata           ),
  .m_axi_rlast             ( m02_axi_rlast           ),
  // acc
  .rd_addr                 (M02_AXI_rd_addr),
  .rd_valid                (M02_AXI_rd_valid),
  .rd_ready                (M02_AXI_rd_ready), // to acc
  .rd_tdata                (M02_AXI_rd_tdata),
  .rd_tvalid               (M02_AXI_rd_tvalid),
  .rd_tready               (M02_AXI_rd_tready), // 默认返回缓冲区足够
  .wr_data                 (M02_AXI_wr_data),
  .wr_addr                 (M02_AXI_wr_addr),
  .wr_valid                (M02_AXI_wr_valid),
  .wr_ready                (M02_AXI_wr_ready),
  .wr_rsp                  (M02_AXI_wr_trsp),
  .complete                (ap_done_i[2])
);
assign M02_AXI_wr_addr = {C_M02_AXI_ADDR_WIDTH{1'b0}};
assign M02_AXI_wr_data = {C_M02_AXI_DATA_WIDTH{1'b0}};
assign M02_AXI_wr_valid = 1'b0;
assign M02_AXI_rd_tready = 1'b1;

mem_axi_control #(
  .C_M_AXI_ADDR_WIDTH ( C_M03_AXI_ADDR_WIDTH ),
  .C_M_AXI_DATA_WIDTH ( C_M03_AXI_DATA_WIDTH ),
  .C_ADDER_BIT_WIDTH  ( 32                   )
)
mem_axi_control_write_res (
  .aclk                    ( ap_clk                  ),
  .areset                  ( areset                  ),
  .kernel_clk              ( ap_clk                  ),
  .kernel_rst              ( areset                  ),
  .ctrl_addr_offset        ( axi03_ptr0              ),
  .ctrl_constant           ( ctrl_constant           ),
  .ap_start                ( ap_start_pulse          ),
  .m_axi_awvalid           ( m03_axi_awvalid         ),
  .m_axi_awready           ( m03_axi_awready         ),
  .m_axi_awaddr            ( m03_axi_awaddr          ),
  .m_axi_awlen             ( m03_axi_awlen           ),
  .m_axi_wvalid            ( m03_axi_wvalid          ),
  .m_axi_wready            ( m03_axi_wready          ),
  .m_axi_wdata             ( m03_axi_wdata           ),
  .m_axi_wstrb             ( m03_axi_wstrb           ),
  .m_axi_wlast             ( m03_axi_wlast           ),
  .m_axi_bvalid            ( m03_axi_bvalid          ),
  .m_axi_bready            ( m03_axi_bready          ),
  .m_axi_arvalid           ( m03_axi_arvalid         ),
  .m_axi_arready           ( m03_axi_arready         ),
  .m_axi_araddr            ( m03_axi_araddr          ),
  .m_axi_arlen             ( m03_axi_arlen           ),
  .m_axi_rvalid            ( m03_axi_rvalid          ),
  .m_axi_rready            ( m03_axi_rready          ),
  .m_axi_rdata             ( m03_axi_rdata           ),
  .m_axi_rlast             ( m03_axi_rlast           ),
  // acc
  .rd_addr                 (M03_AXI_rd_addr),
  .rd_valid                (M03_AXI_rd_valid),
  .rd_ready                (M03_AXI_rd_ready), // to acc
  .rd_tdata                (M03_AXI_rd_tdata),
  .rd_tvalid               (M03_AXI_rd_tvalid),
  .rd_tready               (M03_AXI_rd_tready), // 默认返回缓冲区足够
  .wr_data                 (M03_AXI_wr_data),
  .wr_addr                 (M03_AXI_wr_addr),
  .wr_valid                (M03_AXI_wr_valid),
  .wr_ready                (M03_AXI_wr_ready),
  .wr_rsp                  (M03_AXI_wr_trsp),
  .complete                (ap_done_i[3])
);
assign M03_AXI_rd_addr = {C_M03_AXI_ADDR_WIDTH{1'b0}};
assign M03_AXI_rd_valid = 1'b0;
assign M03_AXI_rd_tready = 1'b1;


// kernel
rd_active_vertex  RD_ACTIVE_VERTEX (
  .clk                (clk),
  .rst                (rst),
  .vertex_num         (vertex_num),
  .iteration_num      (iteration_num),
  .backend_active_v_id        (P11_active_v_id),
  .backend_active_v_updated   (P11_active_v_updated),
  .backend_active_v_id_valid  (P11_active_v_valid),
  .backend_iteration_end      (P11_iteration_end),
  .backend_iteration_end_valid(P11_iteration_end_valid),
  .backend_iteration_id       (P11_iteration_id),
  .next_stage_full            (P2_stage_full),

  .next_rst                   (P1_next_rst),
  .iteration_done             (P1_iteration_done),
  .active_v_id                (P1_active_v_id),
  .active_v_id_valid          (P1_active_v_valid),
  .iteration_end              (P1_iteration_end),
  .iteration_end_valid        (P1_iteration_end_valid),
  .iteration_id               (P1_iteration_id)
);

rd_active_vertex_offset RD_ACTIVE_VERTEX_OFFSET (
  .clk                        (clk),
  .rst                        (P1_next_rst),
  .front_active_v_id          (P1_active_v_id),
  .front_active_v_valid       (P1_active_v_valid),
  .front_iteration_end        (P1_iteration_end),
  .front_iteration_end_valid  (P1_iteration_end_valid),
  .front_iteration_id         (P1_iteration_id),
  .next_stage_full            (P4_stage_full),

  .next_rst                   (P2_next_rst),
  .stage_full                 (P2_stage_full),
  .active_v_id                (P2_active_v_id),
  .active_v_id_valid          (P2_active_v_id_valid),
  .rd_active_v_offset_addr    (P2_rd_active_v_offset_addr),
  .rd_active_v_value_addr     (P2_rd_active_v_value_addr),
  .rd_active_v_addr_valid     (P2_rd_active_v_addr_valid),
  .iteration_end              (P2_iteration_end),
  .iteration_end_valid        (P2_iteration_end_valid),
  .iteration_id               (P2_iteration_id)
);

offset_uram OFFSET_URAM (
  .clk                        (clk),
  .rst                        (P2_next_rst),
  .vertex_num                 (vertex_num),
  .initial_uram               (initial_uram),
  .front_hbm_rd_data          (M00_AXI_rd_tdata),
  .front_hbm_rd_valid         (M00_AXI_rd_tvalid),
  .front_hbm_rd_ready         (M00_AXI_rd_ready),
  .front_rd_active_v_offset_addr  (P2_rd_active_v_offset_addr),
  .front_active_v_valid       (P2_rd_active_v_addr_valid),

  .next_rst                   (P3_next_rst),
  .hbm_rd_addr                (M00_AXI_rd_addr),
  .hbm_rd_valid               (M00_AXI_rd_valid),
  .hbm_rd_complete            (P3_initial_complete),
  .uram_loffset               (P3_active_v_loffset),
  .uram_roffset               (P3_active_v_roffset),
  .uram_dvalid                (P3_active_v_valid)
);

rd_active_vertex_edge RD_ACTIVE_VERTEX_EDGE (
  .clk                        (clk),
  .rst                        (P2_next_rst),
  .front_active_v_id          (P2_active_v_id),
  .front_active_v_valid       (P2_active_v_id_valid),
  .front_active_v_loffset     (P3_active_v_loffset),
  .front_active_v_roffset     (P3_active_v_roffset),
  .front_offset_valid         (P3_active_v_valid),
  .front_iteration_end        (P2_iteration_end),
  .front_iteration_end_valid  (P2_iteration_end_valid),
  .front_iteration_id         (P2_iteration_id),
  .front_active_v_value       (P10_src_recv_update_v_value),
  .front_active_v_value_valid (P10_src_recv_update_v_valid),
  .combine_stage_full         (P5_stage_full),

  .next_rst                   (P4_next_rst),
  .stage_full                 (P4_stage_full),
  .rd_hbm_edge_addr           (P4_rd_hbm_edge_addr),
  .rd_hbm_edge_mask           (P4_rd_hbm_edge_mask),
  .hbm_active_v_id            (P4_hbm_active_v_id),
  .rd_hbm_edge_valid          (P4_rd_hbm_edge_valid),
  .hbm_active_v_value         (P4_hbm_active_v_value),
  .hbm_active_v_value_valid   (P4_hbm_active_v_value_valid),
  .iteration_end              (P4_iteration_end),
  .iteration_end_valid        (P4_iteration_end_valid),
  .iteration_id               (P4_iteration_id)
);

generate_hbm_edge_rqst GENERATE_HBM_EDGE_RQST (
  .clk                        (clk),
  .rst                        (P4_next_rst),
  .front_rd_hbm_edge_addr     (P4_rd_hbm_edge_addr),
  .front_rd_hbm_edge_mask     (P4_rd_hbm_edge_mask),
  .front_active_v_id          (P4_hbm_active_v_id),
  .front_rd_hbm_edge_valid    (P4_rd_hbm_edge_valid),
  .front_active_v_value       (P4_hbm_active_v_value),
  .front_active_v_value_valid (P4_hbm_active_v_value_valid),
  .front_iteration_end        (P4_iteration_end),
  .front_iteration_end_valid  (P4_iteration_end_valid),
  .front_iteration_id         (P4_iteration_id),
  .hbm_full                   (HBM_interface_full),
  .next_stage_full            (P7_stage_full),

  .next_rst                   (P5_next_rst),
  .stage_full                 (P5_stage_full),
  .rd_hbm_edge_addr           (P5_rd_hbm_edge_addr),
  .rd_hbm_edge_valid          (P5_rd_hbm_edge_valid),
  .active_v_id                (P5_active_v_id),
  .active_v_value             (P5_active_v_value),
  .active_v_id_valid          (P5_active_v_valid),
  .iteration_end              (P5_iteration_end),
  .iteration_end_valid        (P5_iteration_end_valid),
  .iteration_id               (P5_iteration_id)
);

hbm_interface HBM_INTERFACE (
  .clk                        (clk),
  .rst                        (P5_next_rst),
  .front_rd_hbm_edge_addr     (P5_rd_hbm_edge_addr),
  .front_rd_hbm_edge_valid    (P5_rd_hbm_edge_valid),
  .hbm_controller_edge        ({M02_AXI_rd_tdata, M01_AXI_rd_tdata}),
  .hbm_controller_valid       ({M02_AXI_rd_tvalid, M01_AXI_rd_tvalid}),
  .hbm_controller_full        ({!M02_AXI_rd_ready, !M01_AXI_rd_ready}),

  .stage_full                 (HBM_interface_full),
  .rd_hbm_edge_addr           ({M02_AXI_rd_addr, M01_AXI_rd_addr}),
  .rd_hbm_edge_valid          ({M02_AXI_rd_valid, M01_AXI_rd_valid}),
  .active_v_edge              (HBM_interface_active_v_edge),
  .active_v_edge_valid        (HBM_interface_active_v_edge_valid)
);

scheduler SCHEDULER (
  .clk                        (clk),
  .rst                        (P5_next_rst),
  .front_active_v_id          (P5_active_v_id),
  .front_active_v_value       (P5_active_v_value),
  .front_active_v_valid       (P5_active_v_valid),
  .front_iteration_end        (P5_iteration_end),
  .front_iteration_end_valid  (P5_iteration_end_valid),
  .front_iteration_id         (P5_iteration_id),
  .hbm_interface_active_v_edge  (HBM_interface_active_v_edge),
  .hbm_interface_active_v_edge_valid  (HBM_interface_active_v_edge_valid),
  .next_stage_full            (P8_stage_full),

  .next_rst                   (P7_next_rst),
  .stage_full                 (P7_stage_full),
  .update_v_id                (P7_update_v_id),
  .update_v_value             (P7_update_v_value),
  .update_v_valid             (P7_update_v_valid),
  .iteration_end              (P7_iteration_end),
  .iteration_end_valid        (P7_iteration_end_valid),
  .iteration_id               (P7_iteration_id)
);

HyperX_Network HYPERX_NETWORK(
    .clk                              (clk),
    .hy1_front_rst                    (P7_next_rst),
    .hy1_front_push_flag              ({CORE_NUM{1'b1}}),
    .hy1_front_update_v_id            (P7_update_v_id),
    .hy1_front_update_v_value         (P7_update_v_value),
    .hy1_front_pull_first_flag        (0),
    .hy1_front_update_v_valid         (P7_update_v_valid),
    .hy1_front_iteration_end          (P7_iteration_end),
    .hy1_front_iteration_end_valid    (P7_iteration_end_valid),
    .hy1_front_iteration_id           (P7_iteration_id),
    .source_core_full                 (P9_dest_core_full),

    .hy1_rst                          (P8_next_rst),
    .hy1_stage_full                   (P8_stage_full),
    .hy1_push_flag                    (),
    .hy1_update_v_id                  (P8_update_v_id),
    .hy1_update_v_value               (P8_update_v_value),
    .hy1_pull_first_flag              (),
    .hy1_update_v_valid               (P8_update_v_valid),
    .hy1_iteration_end                (P8_iteration_end),
    .hy1_iteration_end_valid          (P8_iteration_end_valid),
    .hy1_iteration_id                 (P8_iteration_id)
);


backend_core BACKEND_CORE (
  .clk                      (clk),
  .rst                      (P8_next_rst),
  .recv_update_v_id         (P8_update_v_id),
  .recv_update_v_value      (P8_update_v_value),
  .recv_update_v_valid      (P8_update_v_valid),
  .recv_iteration_end       (P8_iteration_end),
  .recv_iteration_end_valid (P8_iteration_end_valid),
  .recv_iteration_id        (P8_iteration_id),
  .next_stage_full          (P10_stage_full),

  .next_rst                 (P9_next_rst),
  .dest_core_full           (P9_dest_core_full),
  .wr_vertex_bram_addr      (P9_wr_vertex_bram_addr),
  .wr_vertex_bram_data      (P9_wr_vertex_bram_data),
  .wr_vertex_bram_valid     (P9_wr_vertex_bram_valid),
  .wr_vertex_bram_iteration_end       (P9_wr_vertex_bram_iteration_end),
  .wr_vertex_bram_iteration_end_valid (P9_wr_vertex_bram_iteration_end_valid),
  .wr_vertex_bram_iteration_id        (P9_wr_vertex_bram_iteration_id)
);

vtx_ram_controller VTX_RAM_CONTROLLER (
  .clk                        (clk),
  .rst                        (P9_next_rst),
  .vertex_num                 (vertex_num),
  .initial_ram                (initial_ram),
  .transfer_ram               (transfer_ram),
  .hbm_wr_ready               (M03_AXI_wr_ready),
  .hbm_wr_trsp                (M03_AXI_wr_trsp),
  .rd_addr_src                (P2_rd_active_v_value_addr),
  .rd_valid_src               (P2_rd_active_v_addr_valid),
  .wr_addr_dst                (P9_wr_vertex_bram_addr),
  .wr_v_value_dst             (P9_wr_vertex_bram_data),
  .wr_valid_dst               (P9_wr_vertex_bram_valid),
  .front_iteration_end        (P9_wr_vertex_bram_iteration_end),
  .front_iteration_end_valid  (P9_wr_vertex_bram_iteration_end_valid),
  .front_iteration_id         (P9_wr_vertex_bram_iteration_id),
  
  .next_rst                   (P10_next_rst),
  .initial_done               (P10_initial_done),
  .transfer_done              (P10_transfer_done),
  .hbm_wb_data                (M03_AXI_wr_data),
  .hbm_wb_addr                (M03_AXI_wr_addr),
  .hbm_wb_valid               (M03_AXI_wr_valid),
  .stage_full                 (P10_stage_full),
  .src_recv_update_v_value    (P10_src_recv_update_v_value),
  .src_recv_update_v_valid    (P10_src_recv_update_v_valid),
  .backend_active_v_id        (P10_backend_active_v_id),
  .backend_active_v_updated   (P10_backend_active_v_updated),
  .backend_active_v_valid     (P10_backend_active_v_valid),
  .iteration_end              (P10_iteration_end),
  .iteration_end_valid        (P10_iteration_end_valid),
  .iteration_id               (P10_iteration_id)
);

apply_iteration_end APPLY_ITERATION_END (
  .clk                        (clk),
  .rst                        (P10_next_rst),
  .front_active_v_id          (P10_backend_active_v_id),
  .front_active_v_updated     (P10_backend_active_v_updated),
  .front_active_v_valid       (P10_backend_active_v_valid),
  .front_iteration_end        (P10_iteration_end),
  .front_iteration_end_valid  (P10_iteration_end_valid),
  .front_iteration_id         (P10_iteration_id),

  .active_v_id                (P11_active_v_id),
  .active_v_updated           (P11_active_v_updated),
  .active_v_valid             (P11_active_v_valid),
  .iteration_end              (P11_iteration_end),
  .iteration_end_valid        (P11_iteration_end_valid),
  .iteration_id               (P11_iteration_id)
);

endmodule : delta_pr_accelerator_axi_top
`default_nettype wire