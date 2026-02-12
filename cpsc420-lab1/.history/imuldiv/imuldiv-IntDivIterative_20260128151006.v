//========================================================================
// Lab 1 - Iterative Div Unit
//========================================================================

`ifndef PARC_INT_DIV_ITERATIVE_V
`define PARC_INT_DIV_ITERATIVE_V

`include "imuldiv-DivReqMsg.v"

module imuldiv_IntDivIterative
(
  // ports
  // a port is already a signal, no need to redeclare

  // input ports are driven by the outside world (testbench)
  // output ports are driven by my module

  // never redeclare a port name inside the module !!

  input         clk,
  input         reset,

  input         divreq_msg_fn,
  input  [31:0] divreq_msg_a,
  input  [31:0] divreq_msg_b,

  // system says i have a request
  input         divreq_val,

  // i tell system i can accept a request now
  output        divreq_rdy,

  output [63:0] divresp_msg_result,

  // me saying i have a response now
  output        divresp_val,

  // i receive this, tells me that the system is ready to accept a response
  input         divresp_rdy
);

  // internal control wires
  wire a_en;
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

    // these should be passed in because i choose them
    // they are inputs to dpath??
    .divreq_val (divreq_val),
    // .divresp_val (divresp_val),

    // .divreq_rdy (divreq_rdy),
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
  input a_mux_sel,
  input sub_mux_sel
);

  //----------------------------------------------------------------------
  // Sequential Logic
  //----------------------------------------------------------------------

  reg         fn_reg;      // Register for storing function
  reg  [64:0] a_reg;       // Register for storing operand A
  reg  [64:0] b_reg;       // Register for storing operand B
  reg         val_reg;     // Register for storing valid bit
  reg a_sign_bit;
  reg b_sign_bit;


  //----------------------------------------------------------------------
  // Combinational Logic
  //----------------------------------------------------------------------

  // Extract sign bits

  wire sign_bit_a = a_reg[31];
  wire sign_bit_b = b_reg[31];

  // Unsign operands if necessary

  wire [31:0] unsigned_a = ( sign_bit_a ) ? (~a_reg + 1'b1) : a_reg;
  wire [31:0] unsigned_b = ( sign_bit_b ) ? (~b_reg + 1'b1) : b_reg;

  // a wire is a connection
  // it must be driven by an assign statement, or a module output

  // a reg is a variable i assign inside a procedural block
  // always @(*) is combinational
  // always @(posedge clk) is sequential

  // combinational logic updates instantly
  // sequential logic updates on the clock

  // datapath registers update only on clock edges
  // control outputs (enables, mux selects) are combinational functions of state
  // FSM state updates on posedge clk
  // control signals set in always @(*)

  // anything that is a stored thing (register, counter, state) must be assigned in always @(posedge clk)
  // anything that is choose this or that right now (mux select, enable) should be in always @(*) or assign


  wire [64:0] initial_b = {1'b0, unsigned_b, 32'b0};

  wire [64:0] sub_mux_out;

  wire [64:0] initial_a = {33'b0, unsigned_a};

  // would this normally require an 'assign'?
  wire [64:0] a_mux_out = a_mux_sel ? sub_mux_out : initial_a;

  wire [64:0] a_shift_out = a_reg << 1;

  wire [64:0] sub_out = a_shift_out - b_reg;
  // if subtraction succeeded (sub_out is non-negative) keep remainder from sub_out
  // set new quotient bit to one
  wire [64:0] a_next_ok = { sub_out[64:1], 1'b1 };
  // if subtraction failed, RESTORE remainder to shifted value (a_shift_out)
  // set new quotient bit to zero
  wire [64:0] a_next_fail = { a_shift_out[64:1], 1'b0};

  wire sub_negative = sub_out[64];

  wire [64:0] final_subtraction = sub_negative? a_next_fail : a_next_ok;

  // assign sub_mux_out = sub_mux_sel ? a_shift_out : final_subtraction;

  // this doesn't quite follow the diagram
  // CAUTION CAUTION CAUTION

  // connect to the second part
  wire [64:0] second_part = a_reg;


  always @( posedge clk ) begin
    if (reset) begin
      a_reg <= 65'b0;
      b_reg <= 65'b0;
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
  end

  // Computation logic


  // Determine whether or not result is signed. Usually the result is
  // signed if one and only one of the input operands is signed. In other
  // words, the result is signed if the xor of the sign bits of the input
  // operands is true. Remainder opeartions are a bit trickier, and here
  // we simply assume that the result is signed if the dividend for the
  // rem operation is signed.

  wire is_result_signed_div = sign_bit_a ^ sign_bit_b;
  wire is_result_signed_rem = sign_bit_a;


  wire [31:0] signed_quotient;

  wire [31:0] signed_remainder;

  // assign divresp_msg_result = { signed_remainder, signed_quotient };

  // Set the val/rdy signals. The request is ready when the response is
  // ready, and the response is valid when there is valid data in the
  // input registers.

  // assign divreq_rdy  = divresp_rdy;
  // assign divresp_val = val_reg;

  // PLACEHOLDER
  // should be remainder[31:0], quotient[31:0]
  assign divresp_msg_result = { a_reg[64:33], a_reg[31:0]};

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
  output reg b_en,

  output reg sub_mux_sel,
  output reg a_mux_sel
);

  localparam IDLE = 2'd0;
  localparam CALC = 2'd1;
  localparam DONE = 2'd2;

  reg [1:0] state;
  reg [5:0] count;

  // system has a request and I have ready to accept it
  wire accepting_requests = divreq_rdy && divreq_val;
  // i have a request and system can receive it
  wire sending_responses = divresp_rdy && divresp_val;

  // note that for BOTH of these I control only one piece so we rely on inputs from the system to determine their results

  always @( posedge clk) begin
    if (reset) begin
      state <= IDLE;
      count <= 6'd0;
    end else begin
      case (state)
        IDLE: begin
          // if ready for request and request is valid, transition to CALC state
          if (accepting_requests) begin
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
          if (sending_responses) begin
            state <= IDLE;
            count <= 6'd0;
          end
        end
      endcase
    end
  end

  // top thing only happens on clock cycles
  // so count is increased every clock cycle
  // this next thing happens whenever

  always @(*) begin
    // we can define control variables here because the circuit reacts to the logic coming from control
    divreq_rdy = 1'b1; // ready to recieve

    // divreq_val = 1'b0; // we do not have control over this variable !! this is given to us by the SYSTEM


    a_en = 1'b0;
    b_en = 1'b0;
    a_mux_sel = 1'b0;
    sub_mux_sel = 1'b0;

    case (state)
      IDLE: begin
        // ready to receive requests, so set to one
        divreq_rdy = 1'b1;
        divresp_val = 1'b0; // no valid output yet

        a_en = accepting_requests;
        a_mux_sel = 1'b0;
        
        b_en = accepting_requests;
        sub_mux_sel = 1'b0;
      end

      CALC: begin
        divreq_rdy = 1'b0; // NOT recieving inputs right now
        divresp_val = 1'b0; // still no valid output

        a_en = 1'b1;
        b_en = 1'b0;
        a_mux_sel = 1'b1;
        sub_mux_sel = 1'b1;
      end

      DONE: begin
        divresp_val = 1'b1; // response complete, ready to be sent out
        divreq_rdy = 1'b0; // done, still not accepting requests until we return to idle
      end

    endcase
  end

  

endmodule



`endif
