//========================================================================
// Lab 1 - Iterative Div Unit
//========================================================================

`ifndef PARC_INT_DIV_ITERATIVE_V
`define PARC_INT_DIV_ITERATIVE_V

`include "imuldiv-DivReqMsg.v"

module imuldiv_IntDivIterative
(

  input         clk,
  input         reset,

  input         divreq_msg_fn,
  input  [31:0] divreq_msg_a,
  input  [31:0] divreq_msg_b,
  input         divreq_val,
  output        divreq_rdy,

  output [63:0] divresp_msg_result,
  output        divresp_val,
  input         divresp_rdy
);

  wire a_en;
  wire divreq_val;
  wire divresp_val;
  wire divreq_rdy;
  wire divresp_rdy;
  wire b_en; // not sure if we still need this
  wire a_mux_sel;

  wire sub_mux_sel;


  imuldiv_IntDivIterativeDpath dpath
  (
    .clk                (clk),
    .reset              (reset),
    .divreq_msg_fn      (divreq_msg_fn),
    .divreq_msg_a       (divreq_msg_a),
    .divreq_msg_b       (divreq_msg_b),
    .divreq_val         (divreq_val),
    .divreq_rdy         (divreq_rdy),
    .divresp_msg_result (divresp_msg_result),
    .divresp_val        (divresp_val),
    .divresp_rdy        (divresp_rdy),

    // hook up from control
    .a_en (a_en),
    .b_en (b_en),
    .a_mux_sel (a_mux_sel),
    .sub_mux_sel (sub_mux_sel)
  );

  imuldiv_IntDivIterativeCtrl ctrl
  (
    .clk (clk),
    .reset (reset),

    .divreq_val (divreq_val),
    .divresp_val (divresp_val),

    .divreq_rdy (divreq_rdy),
    .divresp_rdy (divresp_rdy),

    .a_en (a_en),
    .b_en (b_en),
    .a_mux_sel (a_mux_sel),
    .sub_mux_sel (sub_mux_sel)
  );

endmodule

//------------------------------------------------------------------------
// Datapath
//------------------------------------------------------------------------

module imuldiv_IntDivIterativeDpath
(
  input         clk,
  input         reset,

  input         divreq_msg_fn,      // Function of MulDiv Unit
  input  [31:0] divreq_msg_a,       // Operand A
  input  [31:0] divreq_msg_b,       // Operand B
  input         divreq_val,         // Request val Signal
  output        divreq_rdy,         // Request rdy Signal

  output [63:0] divresp_msg_result, // Result of operation
  output        divresp_val,        // Response val Signal
  input         divresp_rdy,         // Response rdy Signal

  input a_en,
  input b_en,
  input a_mux_sel
);

  //----------------------------------------------------------------------
  // Sequential Logic
  //----------------------------------------------------------------------

  reg         fn_reg;      // Register for storing function
  reg  [31:0] a_reg;       // Register for storing operand A
  reg  [31:0] b_reg;       // Register for storing operand B
  reg         val_reg;     // Register for storing valid bit



  // always @( posedge clk ) begin

  //   // Stall the pipeline if the response interface is not ready
  //   if ( divresp_rdy ) begin
  //     fn_reg  <= divreq_msg_fn;
  //     a_reg   <= divreq_msg_a;
  //     b_reg   <= divreq_msg_b;
  //     val_reg <= divreq_val;
  //   end

  // end

  //----------------------------------------------------------------------
  // Combinational Logic
  //----------------------------------------------------------------------

  // Extract sign bits

  wire sign_bit_a = a_reg[31];
  wire sign_bit_b = b_reg[31];

  // Unsign operands if necessary

  wire [31:0] unsigned_a = ( sign_bit_a ) ? (~a_reg + 1'b1) : a_reg;
  wire [31:0] unsigned_b = ( sign_bit_b ) ? (~b_reg + 1'b1) : b_reg;


  wire [64:0] initial_b = {1'b0, unsigned_b, 32'b0};

  wire sub_mux_out;

  wire [64:0] initial_a = {33'b0, unsigned_a};

  wire a_mux_out = a_mux_sel ? sub_mux_out : initial_a;

  wire a_shift_out = a_reg << 1;

  wire [64:0] sub_out = a_shift_out - b_reg;
  wire msb_sub_out = sub_out[64];
  wire final_subtraction = {msb_sub_out, 1'b1};

  sub_mux_out = sub_mux_sel ? a_shift_out : final_subtraction;

  // connect to the second part
  wire second_part[64:0] = a_reg;


  always @( posedge clk ) begin
    if (reset) begin
      a_reg <= 32'b0;
      b_reg <= 32'b0;
    end else begin
      if (a_en) begin
        // i'm wondering if this should be adjusted but i think it's okay
        a_reg <= a_mux_sel ? sub_mux_out : initial_a;
      end
      if (b_en) begin
        // i know this is fine
        b_reg <= initial_b;
    end
  end

  // Computation logic

  // wire [31:0] unsigned_quotient
  //   = ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_SIGNED )   ? unsigned_a / unsigned_b
  //   : ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_UNSIGNED ) ? a_reg / b_reg
  //   :                                                   32'bx;

  // wire [31:0] unsigned_remainder
  //   = ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_SIGNED )   ? unsigned_a % unsigned_b
  //   : ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_UNSIGNED ) ? a_reg % b_reg
  //   :                                                   32'bx;

  // Determine whether or not result is signed. Usually the result is
  // signed if one and only one of the input operands is signed. In other
  // words, the result is signed if the xor of the sign bits of the input
  // operands is true. Remainder opeartions are a bit trickier, and here
  // we simply assume that the result is signed if the dividend for the
  // rem operation is signed.

  wire is_result_signed_div = sign_bit_a ^ sign_bit_b;
  wire is_result_signed_rem = sign_bit_a;

  // Sign the final results if necessary

  wire [31:0] signed_quotient;
    // = ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_SIGNED
    //  && is_result_signed_div ) ? ~unsigned_quotient + 1'b1
    // :                            unsigned_quotient;

  wire [31:0] signed_remainder;
  //   = ( fn_reg == `IMULDIV_DIVREQ_MSG_FUNC_SIGNED
  //    && is_result_signed_rem )   ? ~unsigned_remainder + 1'b1
  //  :                              unsigned_remainder;

  assign divresp_msg_result = { signed_remainder, signed_quotient };

  // Set the val/rdy signals. The request is ready when the response is
  // ready, and the response is valid when there is valid data in the
  // input registers.

  assign divreq_rdy  = divresp_rdy;
  assign divresp_val = val_reg;

endmodule

//------------------------------------------------------------------------
// Control Logic
//------------------------------------------------------------------------

module imuldiv_IntDivIterativeCtrl
(
  input clk,
  input reset,
  input divreq_val,
  input divresp_rdy,

  output reg divreq_rdy,
  output reg divresp_val,

  output reg a_en,
  output b_en,

  output reg sub_mux_sel,
  output reg a_mux_sel
);

  localparam IDLE = 2'd0;
  localparam CALC = 2'd1;
  localparam DONE = 2'd2;

  reg [1:0] state;
  reg [5:0] count;

  wire reqrdy_and_reqval = divreq_rdy && divreq_val;
  wire resprdy_and_respval = divresp_rdy && divresp_val;

  always @( posedge clk) begin
    if (reset) begin
      state <= IDLE;
      count <= 6'd0;
    end else begin
      case (state)
        IDLE: begin
          // if ready for request and request is valid, transition to CALC state
          if (reqrdy_and_reqval) begin
            state <= CALC;
            count <= 6'd0;
          end
        end

        CALC: begin
          if (count == 6'd31) begin
            state <= DONE;
          end else begin
            count <= count + 6'd1;
          end
        end

        DONE: begin
          // only return to idle state once the response is ready and valid
          if (resp_val_and_resp_rdy) begin
            state <= IDLE;
            count <= 6'd0;
          end
      endcase
    end
  end

  // top thing only happens on clock cycles
  // so count is increased every clock cycle
  // this next thing happens whenever

  always @(*) begin
    // we can define control variables here because the circuit reacts to the logic coming from control
    divreq_rdy = 1'b0;
    divreq_val = 1'b0;

    a_en = 1'b0;
    b_en = 1'b0;
    a_mux_sel = 1'b0;
    sub_mux_sel = 1'b0;

    case (state)
      IDLE: begin
        // ready to receive requests
        divreq_rdy = 1'b1;
        // I'm not sure if we set the request to valid though?

        a_en = reqrdy_and_reqval;
        a_mux_sel = 1'b0;
        
        b_en = req_rdy_and_req_val;
        sub_mux_sel = 1'b0;
      end

      CALC: begin
        a_en = 1'b1;
        a_mux_sel = 1'b1;
        b_en = 1'b1;
        sub_mux_sel = 1'b1;
      end

      DONE: begin
        divresp_val = 1'b1;
      end

    endcase
  end

  

endmodule



`endif
