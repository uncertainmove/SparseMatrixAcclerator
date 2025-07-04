// This is a generated file. Use and modify at your own risk.
////////////////////////////////////////////////////////////////////////////////

// Description: This is an AXI4 write master module example. The module
// demonstrates how to issue AXI write transactions to a memory mapped slave.
// Given a starting address offset and a transfer size in bytes, it will issue
// one or more AXI transactions by generating incrementing AXI write transfers
// when data is transfered over the AXI4-Stream interface.

// Theory of operation:
// It uses a minimum subset of the AXI4 protocol by omitting AXI4 signals that
// are not used.  When packaged as a kernel or IP, this allows for optimizations
// to occur within the AXI Interconnect system to increase Fmax, potentially
// increase performance, and reduce latency/area. When C_INCLUDE_DATA_FIFO is
// set to 1, a depth 32 FIFO is provided for extra buffering.
//
// When ctrl_start is asserted, the ctrl_addr_offset (assumed 4kb aligned) and
// the transfer size in bytes is registered into the module.  the The bulk of the
// logic consists of counters to track how many transfers/transactions have been
// issued.  When the transfer size is reached, and all transactions are
// committed, then done is asserted.
//
// Usage:
// 1) assign ctrl_addr_offset to a 4kB aligned starting address.
// 2) assign ctrl_xfer_size_in_bytes to the size in bytes of the requested transfer.
// 3) Assert ctrl_start for once cycle.  At the posedge, the ctrl_addr_offset and
// ctrl_xfer_size_in_bytes will be registered in the module, and will start
// to issue write address transfers when the first data arrives on the s_axis
// interface.  If the the transfer size is larger than 4096
// bytes, multiple transactions will be issued.
// 4) As write data is presented on the axi4-stream interface, WLAST and
// additional write address transfers will be issued as necessary.
// 5) When the final B-channel transaction has been received, the module will assert
//    the ctrl_done signal for one cycle.  If a data FIFO is present, data may
//    still be present in the FIFO.  It will
// 6) Jump to step 1.
////////////////////////////////////////////////////////////////////////////////

// default_nettype of none prevents implicit wire declaration.
`default_nettype none

module mem_axi_control_write #(
  // Set to the address width of the interface
  parameter integer C_M_AXI_ADDR_WIDTH  = 64,

  // Set the data width of the interface
  // Range: 32, 64, 128, 256, 512, 1024
  parameter integer C_M_AXI_DATA_WIDTH  = 32,

  // Width of the ctrl_xfer_size_in_bytes input
  // Range: 16:C_M_AXI_ADDR_WIDTH
  parameter integer C_XFER_SIZE_WIDTH   = C_M_AXI_ADDR_WIDTH,

  // Specifies the maximum number of AXI4 transactions that may be outstanding.
  parameter integer C_MAX_OUTSTANDING   = 32,

  // Includes a data fifo between the AXI4-Stream slave and the AXI4 write
  // channel master.  Depth is set to 32.
  parameter integer C_INCLUDE_DATA_FIFO = 0
)
(
  // AXI Interface
  input  wire                            aclk,
  input  wire                            areset,

  // Control signals
  input  wire                            ctrl_start,              // Pulse high for one cycle to begin reading
  // The following ctrl signals are sampled when ctrl_start is asserted
  input  wire [C_M_AXI_ADDR_WIDTH-1:0]   ctrl_addr_offset,        // Starting Address offset

  // AXI4 master interface (write only)
  output wire                            m_axi_awvalid,
  input  wire                            m_axi_awready,
  output wire [C_M_AXI_ADDR_WIDTH-1:0]   m_axi_awaddr,
  output wire [7:0]                      m_axi_awlen,

  output wire                            m_axi_wvalid,
  input  wire                            m_axi_wready,
  output wire [C_M_AXI_DATA_WIDTH-1:0]   m_axi_wdata,
  output wire [C_M_AXI_DATA_WIDTH/8-1:0] m_axi_wstrb,
  output wire                            m_axi_wlast,

  input  wire                            m_axi_bvalid,
  output wire                            m_axi_bready,
  // AXI4-Stream interface
  input wire                             acc_clk,
  input wire                             acc_rst,
  input wire [C_M_AXI_DATA_WIDTH-1:0]    wr_data,
  input wire [C_M_AXI_ADDR_WIDTH-1:0]    wr_addr,
  input wire                             wr_valid,
  output wire                            wr_ready,
  output wire                            wr_rsp,
  output wire                            complete
);

timeunit 1ps;
timeprecision 1ps;

///////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////
function integer f_min (
  input integer a,
  input integer b
);
  f_min = (a < b) ? a : b;
endfunction

/////////////////////////////////////////////////////////////////////////////
// Local Parameters
/////////////////////////////////////////////////////////////////////////////
localparam integer LP_DW_BYTES                   = C_M_AXI_DATA_WIDTH/8;
localparam integer LP_LOG_DW_BYTES               = $clog2(LP_DW_BYTES);
localparam integer LP_MAX_BURST_LENGTH           = 1;   // Max AXI Protocol burst length
localparam integer LP_MAX_BURST_BYTES            = 4096;  // Max AXI Protocol burst size in bytes
localparam integer LP_AXI_BURST_LEN              = f_min(LP_MAX_BURST_BYTES/LP_DW_BYTES, LP_MAX_BURST_LENGTH);
localparam integer LP_LOG_BURST_LEN              = $clog2(LP_AXI_BURST_LEN);
localparam integer LP_LOG_MAX_W_TO_AW            = 8; // Allow up to 256 outstanding w to aw transactions
localparam [C_M_AXI_ADDR_WIDTH-1:0] LP_ADDR_MASK = LP_DW_BYTES*LP_AXI_BURST_LEN - 1;
localparam integer LP_FIFO_DEPTH                 = 32;
localparam integer LP_FIFO_READ_LATENCY          = 1; // 2: Registered output on BRAM, 1: Registered output on LUTRAM
localparam integer LP_FIFO_COUNT_WIDTH           = $clog2(LP_FIFO_DEPTH)+1;
localparam integer LP_OUTSTANDING_CNTR_WIDTH     = $clog2(C_MAX_OUTSTANDING+1);

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////
// Control
logic                                 ctrl_start_d1 = 1'b0;
logic [C_M_AXI_ADDR_WIDTH-1:0]        addr_offset_r;
logic                                 start    = 1'b0;
logic                                 start_d1 = 1'b0;
// Write data channel
logic                                 wxfer;       // Unregistered write data transfer
logic                                 w_running = 1'b0;
// Write address channel
logic                                 awxfer;
logic                                 ar_buffer_empty;
logic                                 ar_buffer_full;
logic                                 r_buffer_empty;
logic                                 r_buffer_full;
logic                                 awvalid_r;
// Write response channel
wire                                  bxfer;
logic                                 stall_aw;
logic [LP_OUTSTANDING_CNTR_WIDTH-1:0] outstanding_vacancy_count;

/////////////////////////////////////////////////////////////////////////////
// Control logic
/////////////////////////////////////////////////////////////////////////////
always @(posedge aclk) begin
  ctrl_start_d1 <= ctrl_start;
end

always @(posedge aclk) begin
  if (ctrl_start) begin
    // Align transfer to burst length to avoid AXI protocol issues if starting address is not correctly aligned.
    addr_offset_r <= ctrl_addr_offset & ~LP_ADDR_MASK;
  end
end

always @(posedge aclk) begin
  start <= ctrl_start_d1;
end

generate
if (C_INCLUDE_DATA_FIFO == 1) begin : gen_fifo

  // xpm_fifo_async: ASynchronous FIFO
  // Xilinx Parameterized Macro, Version 2017.4
  xpm_fifo_async # (
    .FIFO_MEMORY_TYPE    ( "distributed"        ) , // string; "auto", "block", "distributed", or "ultra";
    .ECC_MODE            ( "no_ecc"             ) , // string; "no_ecc" or "en_ecc";
    .FIFO_WRITE_DEPTH    ( LP_FIFO_DEPTH        ) , // positive integer
    .WRITE_DATA_WIDTH    ( C_M_AXI_ADDR_WIDTH   ) , // positive integer
    .WR_DATA_COUNT_WIDTH ( LP_FIFO_COUNT_WIDTH  ) , // positive integer, not used
    .PROG_FULL_THRESH    ( 10                   ) , // positive integer, not used
    .FULL_RESET_VALUE    ( 1                    ) , // positive integer; 0 or 1
    .USE_ADV_FEATURES    ( "1F1F"               ) , // string; "0000" to "1F1F";
    .READ_MODE           ( "fwft"               ) , // string; "std" or "fwft";
    .FIFO_READ_LATENCY   ( LP_FIFO_READ_LATENCY ) , // positive integer;
    .READ_DATA_WIDTH     ( C_M_AXI_ADDR_WIDTH   ) , // positive integer
    .RD_DATA_COUNT_WIDTH ( LP_FIFO_COUNT_WIDTH  ) , // positive integer, not used
    .PROG_EMPTY_THRESH   ( 10                   ) , // positive integer, not used
    .DOUT_RESET_VALUE    ( "0"                  ) , // string, don't care
    .WAKEUP_TIME         ( 0                    ) // positive integer; 0 or 2;
  )
  inst_xpm_fifo_sync_for_addr (
    .sleep         ( 1'b0                     ) ,
    .rst           ( acc_rst                  ) ,
    .wr_clk        ( acc_clk                  ) ,
    .wr_en         ( wr_valid                 ) ,
    .din           ( wr_addr + addr_offset_r  ) ,
    .full          (                          ) ,
    .overflow      (                          ) ,
    .prog_full     ( ar_buffer_full           ) ,
    .wr_data_count (                          ) ,
    .almost_full   (                          ) ,
    .wr_ack        (                          ) ,
    .wr_rst_busy   (                          ) ,
    .rd_clk        ( aclk                     ) ,
    .rd_en         ( awxfer                   ) ,
    .dout          ( m_axi_awaddr             ) ,
    .empty         ( ar_buffer_empty          ) ,
    .prog_empty    (                          ) ,
    .rd_data_count (                          ) ,
    .almost_empty  (                          ) ,
    .data_valid    (                          ) ,
    .underflow     (                          ) ,
    .rd_rst_busy   (                          ) ,
    .injectsbiterr ( 1'b0                     ) ,
    .injectdbiterr ( 1'b0                     ) ,
    .sbiterr       (                          ) ,
    .dbiterr       (                          )
  );

  // xpm_fifo_async: ASynchronous FIFO
  // Xilinx Parameterized Macro, Version 2017.4
  xpm_fifo_async # (
    .FIFO_MEMORY_TYPE    ( "distributed"        ) , // string; "auto", "block", "distributed", or "ultra";
    .ECC_MODE            ( "no_ecc"             ) , // string; "no_ecc" or "en_ecc";
    .FIFO_WRITE_DEPTH    ( LP_FIFO_DEPTH        ) , // positive integer
    .WRITE_DATA_WIDTH    ( C_M_AXI_DATA_WIDTH   ) , // positive integer
    .WR_DATA_COUNT_WIDTH ( LP_FIFO_COUNT_WIDTH  ) , // positive integer, not used
    .PROG_FULL_THRESH    ( 10                   ) , // positive integer, not used
    .FULL_RESET_VALUE    ( 1                    ) , // positive integer; 0 or 1
    .USE_ADV_FEATURES    ( "1F1F"               ) , // string; "0000" to "1F1F";
    .READ_MODE           ( "fwft"               ) , // string; "std" or "fwft";
    .FIFO_READ_LATENCY   ( LP_FIFO_READ_LATENCY ) , // positive integer;
    .READ_DATA_WIDTH     ( C_M_AXI_DATA_WIDTH   ) , // positive integer
    .RD_DATA_COUNT_WIDTH ( LP_FIFO_COUNT_WIDTH  ) , // positive integer, not used
    .PROG_EMPTY_THRESH   ( 10                   ) , // positive integer, not used
    .DOUT_RESET_VALUE    ( "0"                  ) , // string, don't care
    .WAKEUP_TIME         ( 0                    ) // positive integer; 0 or 2;
  )
  inst_xpm_fifo_sync_for_data (
    .sleep         ( 1'b0                     ) ,
    .rst           ( acc_rst                  ) ,
    .wr_clk        ( acc_clk                  ) ,
    .wr_en         ( wr_valid                 ) ,
    .din           ( wr_data                  ) ,
    .full          (                          ) ,
    .overflow      (                          ) ,
    .prog_full     ( r_buffer_full            ) ,
    .wr_data_count (                          ) ,
    .almost_full   (                          ) ,
    .wr_ack        (                          ) ,
    .wr_rst_busy   (                          ) ,
    .rd_clk        ( aclk                     ) ,
    .rd_en         ( wxfer                    ) ,
    .dout          ( m_axi_wdata              ) ,
    .empty         ( r_buffer_empty           ) ,
    .prog_empty    (                          ) ,
    .rd_data_count (                          ) ,
    .almost_empty  (                          ) ,
    .data_valid    ( m_axi_wvalid             ) ,
    .underflow     (                          ) ,
    .rd_rst_busy   (                          ) ,
    .injectsbiterr ( 1'b0                     ) ,
    .injectdbiterr ( 1'b0                     ) ,
    .sbiterr       (                          ) ,
    .dbiterr       (                          )
  );
  assign wr_ready = !ar_buffer_full && !r_buffer_full;
end
else begin : gen_no_fifo
  // Gate valid/ready signals with running so transfers don't occur before the
  // xfer size is known.
  assign m_axi_wvalid  = wr_valid;
  assign m_axi_wdata   = wr_data;
  assign m_axi_awaddr = wr_addr;
  assign wr_ready = m_axi_awready && m_axi_wready;
end
endgenerate

assign wxfer = m_axi_wvalid & m_axi_wready;
assign m_axi_wstrb   = {(C_M_AXI_DATA_WIDTH/8){1'b1}};
/////////////////////////////////////////////////////////////////////////////
// AXI Write Address Channel
/////////////////////////////////////////////////////////////////////////////
// The address channel samples the data channel and send out transactions when
// first beat of m_axi_wdata is asserted. This ensures that address requests are not
// sent without data on the way.
assign m_axi_awvalid = awvalid_r;
assign awxfer = m_axi_awvalid & m_axi_awready;

always @ (posedge aclk) begin
  if (areset) begin
    awvalid_r <= 1'b0;
  end else begin
    awvalid_r <= ~ar_buffer_empty & ~stall_aw & ~awvalid_r ? 1'b1 :
                  m_axi_awready ? 1'b0 : awvalid_r;
  end
end

assign m_axi_awlen   = 0; // force to 1

/////////////////////////////////////////////////////////////////////////////
// AXI Write Response Channel
/////////////////////////////////////////////////////////////////////////////
assign m_axi_bready = 1'b1;
assign bxfer = m_axi_bready & m_axi_bvalid;
assign m_axi_wlast = 1'b1;
assign wr_rsp = bxfer;

assign complete = outstanding_vacancy_count == C_MAX_OUTSTANDING[0+:LP_OUTSTANDING_CNTR_WIDTH] &&
  ar_buffer_empty && r_buffer_empty;

// Keeps track of the number of outstanding transactions. Stalls
// when the value is reached so that the FIFO won't overflow.
// If no FIFO present, then just limit at max outstanding transactions.
mem_axi_control_counter #(
  .C_WIDTH ( LP_OUTSTANDING_CNTR_WIDTH                       ) ,
  .C_INIT  ( C_MAX_OUTSTANDING[0+:LP_OUTSTANDING_CNTR_WIDTH] )
)
inst_aw_to_b_transaction_cntr (
  .clk        ( aclk                              ) ,
  .clken      ( 1'b1                              ) ,
  .rst        ( areset                            ) ,
  .load       ( 1'b0                              ) ,
  .incr       ( awxfer                            ) ,
  .decr       ( bxfer                             ) ,
  .load_value ( {LP_OUTSTANDING_CNTR_WIDTH{1'b0}} ) ,
  .count      ( outstanding_vacancy_count         ) ,
  .is_zero    ( stall_aw                          )
);

endmodule : mem_axi_control_write

`default_nettype wire