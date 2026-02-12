//========================================================================
// Lab 1 - Iterative Mul Unit
//========================================================================

`ifndef PARC_INT_MUL_ITERATIVE_V
`define PARC_INT_MUL_ITERATIVE_V

module imuldiv_IntMulIterative
(
  input                clk,
  input                reset,

  input  [31:0] mulreq_msg_a,
  input  [31:0] mulreq_msg_b,
  input         mulreq_val,
  output        mulreq_rdy,

  output [63:0] mulresp_msg_result,
  output        mulresp_val,
  input         mulresp_rdy
);

  wire a_en;
  wire a_mux_sel;

  wire b_en;
  wire b_mux_sel;

  wire counter_is_zero;

  // add/shift/stores values
  imuldiv_IntMulIterativeDpath dpath
  (
    .clk                (clk),
    .reset              (reset),

    .mulresp_msg_result (mulresp_msg_result),

    .a_en (a_en),
    .a_mux_sel (a_mux_sel),
    .b_en (b_en),
    .b_mux_sel (b_mux_sel),

    .mulreq_msg_a (mulreq_msg_a),
    .mulreq_msg_b (mulreq_msg_b)
  );

  // deciding which add/shift/stores happen this cycle
  imuldiv_IntMulIterativeCtrl ctrl
  (
    // a_en - should a_reg update this cycle ?
    // a_mux_sel - should a_reg load init (0) or shifted value (1) ?
    .a_en (a_en),
    .a_mux_sel (a_mux_sel),
    .b_en (b_en),
    .b_mux_sel (b_mux_sel),

    .clk (clk),
    .reset (reset),

    .mulreq_val (mulreq_val),
    .mulreq_rdy (mulreq_rdy),

    .mulresp_val        (mulresp_val),
    .mulresp_rdy        (mulresp_rdy)

  );



endmodule

//------------------------------------------------------------------------
// Datapath
//------------------------------------------------------------------------


module imuldiv_IntMulIterativeDpath
(
  input         clk,
  input         reset,

  input [31:0] mulreq_msg_a,
  input [31:0] mulreq_msg_b,

  output [63:0] mulresp_msg_result, // Result of operation

  // my stuff
  input a_en,
  input a_mux_sel, // should start at zero
  input b_en,
  input b_mux_sel // should start at zero
);

  //----------------------------------------------------------------------
  // Sequential Logic
  //----------------------------------------------------------------------

  reg sign_a_reg;
  reg sign_b_reg;

  reg [4:0] counter_reg;

  reg  [63:0] a_reg;       // Register for storing operand A
  reg  [31:0] b_reg;       // Register for storing operand B

  // unsigned
  // wire [63:0] unsigned_a = mulreq_msg_a[31] ? (~mulreq_msg_a + 1'b1) : mulreq_msg_a
  // wire [31:0] unsigned_b = mulreq_msg_b[31] ? (~mulreq_msg_b + 1'b1) : muldivreq_msg_b

  // we actually unsign no matter what
  wire [63:0] unsigned_a = (~mulreq_msg_a + 1'b1);
  wire [31:0] unsigned_b = (~mulreq_msg_b + 1'b1);

  wire [63:0] initial_a = {32'b0, unsigned_a};

  wire [63:0] a_shift_out = a_reg << 1'b1;

  // on a zero we use initial a, after that we use a_shift_out
  wire a_mux_out = a_mux_sel ? a_shift_out : initial_a;

  // COUNTER
  wire [4:0] counter_decrement = counter_reg - 5'd1;
  // when cntr_mux_sel is one (CALC) decrement, otherwise set to 31
  wire [4:0] counter_next = cntr_mux_sel ? counter_decrement : 5'd31;

  assign counter_is_zero = (counter_reg == 5'd0);



  // reg [31:0] normalized_a;
  // reg [31:0] normalized_b;

  reg [63:0] result_reg;

  wire b_lsb = b_reg[0];

  wire [63:0] result_next = b_lsb ? (result_reg + a_reg) : result_reg;

  //----------------------------------------------------------------------
  // Combinational Logic
  //----------------------------------------------------------------------

  wire [63:0] a_shift_out = a_reg << 1; //ia

  // 32nd bit of input
  wire sign_a = mulreq_msg_a[31];
  wire [31:0] unsigned_a = sign_a ? (~mulreq_msg_a + 1'b1) : mulreq_msg_a;


  // this combines the zeros with unigned a
  wire [63:0] a_init = {32'b0, unsigned_a};
  

  // mux
  wire [63:0] a_mux_out = (a_mux_sel) ? a_shift_out : a_init;


  // b logic ------------

  wire [31:0] b_shift_out = b_reg >> 1; //ia
  wire sign_b = mulreq_msg_b[31];
  wire [31:0] unsigned_b = sign_b ? (~mulreq_msg_b + 1'b1) : mulreq_msg_b;

  // we're controlling b_mux_sel somewhere else- it should be initialized to zero and set to one during the CALC state
  // this can stay 32 bits
  wire [31:0] b_mux_out = b_mux_sel ? b_shift_out : unsigned_b;


  wire final_sign = sign_a_reg ^ sign_b_reg;
  wire [63:0] final_result = final_sign ? (~result_reg + 64'd1) : result_reg;

  always @( posedge clk ) begin
    if (reset) begin
      // clear, set to zero
      a_reg <= 64'b0;
      b_reg <= 32'b0;
      a_mag_reg <= 32'b0;
      b_mag_reg <= 32'b0;
      sign_a_reg <= 1'b0;
      sign_b_reg <= 1'b0;
      result_reg <= 64'b0;
    end else begin
      if (a_en) begin
        // set to the output of the mux
        a_reg <= a_mux_out;
      end
      if (b_en) begin
        b_reg <= b_mux_out;
      end

      // result update
      if (a_en && (a_mux_sel == 1'b0)) begin
        // this means we're cleared to start but it's the first round
        // initialize
        // save inputs
        sign_a_reg <= mulreq_msg_a[31];
        sign_b_reg <= mulreq_msg_b[31];
        a_mag_reg <= unsigned_a;
        b_mag_reg <= unsigned_b;

        result_reg <= 64'b0;

      end else if (a_en && (a_mux_sel == 1'b1)) begin
        // if we're taking shifted inputs, start adding b
        // on every clock cycle
        // adds if lsb is 1
        result_reg <= result_next;
      end
    end
      
  end

  // temp
  assign mulresp_msg_result = final_result;
  
  
  // // Extract sign bits

  // wire sign_bit_a = a_reg[31];
  // wire sign_bit_b = b_reg[31];

  // // Unsign operands if necessary

  // wire [31:0] unsigned_a = ( sign_bit_a ) ? (~a_reg + 1'b1) : a_reg;
  // wire [31:0] unsigned_b = ( sign_bit_b ) ? (~b_reg + 1'b1) : b_reg;

  // // Computation logic

  // wire [63:0] unsigned_result = unsigned_a * unsigned_b;

  // // Determine whether or not result is signed. Usually the result is
  // // signed if one and only one of the input operands is signed. In other
  // // words, the result is signed if the xor of the sign bits of the input
  // // operands is true. Remainder opeartions are a bit trickier, and here
  // // we simply assume that the result is signed if the dividend for the
  // // rem operation is signed.

  // wire is_result_signed = sign_bit_a ^ sign_bit_b;

  // assign mulresp_msg_result 
  //   = ( is_result_signed ) ? (~unsigned_result + 1'b1) : unsigned_result;

  // // Set the val/rdy signals. The request is ready when the response is
  // // ready, and the response is valid when there is valid data in the
  // // input registers.

  // assign mulreq_rdy  = mulresp_rdy;
  // assign mulresp_val = val_reg;

endmodule

//------------------------------------------------------------------------
// Control Logic
//------------------------------------------------------------------------

module imuldiv_IntMulIterativeCtrl
(
  input clk,
  input reset,

  input mulreq_val,
  output reg mulreq_rdy,

  output reg mulresp_val,
  input mulresp_rdy,

  output reg a_en,
  output reg a_mux_sel,
  output reg b_en,
  output reg b_mux_sel

);

localparam IDLE = 2'd0;
localparam CALC = 2'd1;
localparam DONE = 2'd2;

reg [1:0] state; // represents 4 numbers (really 3)
reg [5:0] count; // represents 64 numbers

wire req_rdy_and_req_val = mulreq_val && mulreq_rdy;
wire resp_val_and_resp_rdy = mulresp_val && mulresp_rdy;

always @( posedge clk ) begin
  // if at reset, set to idle
  if (reset) begin
    state <= IDLE;
    count <= 6'd0; // set all six bits to zero
  end else begin
    case (state)
      IDLE: begin
        // if ready for request and request is valid, transition to CALC state
        if (req_rdy_and_req_val) begin
          state <= CALC;
          count <= 6'd0;
        end
      end

      CALC: begin
        // if done counting, move to next state
        if (count == 6'd31) begin
          state <= DONE;
        end else begin
          // increase counter by one
          count <= count + 6'd1;
        end
      end

      DONE: begin
        if (resp_val_and_resp_rdy) begin
          state <= IDLE;
          count <= 6'd0;
        end
      end
    endcase
  end
end

always @(*) begin

  // not ready to receive, response invalid
  mulreq_rdy = 1'b0;
  mulresp_val = 1'b0;

  a_en = 1'b0;
  a_mux_sel = 1'b0;

  b_en = 1'b0;
  b_mux_sel = 1'b0;

  case (state)
    IDLE: begin
      // ready to receive requests
      mulreq_rdy = 1'b1;
      // state of ready for requests and requests valid
      // if true, we perform operations
      a_en = req_rdy_and_req_val;
      a_mux_sel = 1'b0; // initialized to zero to take response input not shifted input
      b_en = req_rdy_and_req_val;
      b_mux_sel = 1'b0;
    end
    CALC: begin
      // a_en becomes temporarily irrelevant
      a_en = 1'b1; // this is one every time because we've already checked that input is valid and ready and we're using /altering it 
      a_mux_sel = 1'b1; // get input from shifted values
      b_en = 1'b1;
      b_mux_sel = 1'b1; // get input from shifted values

    end
    DONE: begin
      mulresp_val = 1'b1;
    end
  endcase

end
    



endmodule

`endif
