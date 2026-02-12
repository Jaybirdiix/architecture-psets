//========================================================================
// Test for Muldiv Unit
//========================================================================

`include "imuldiv-MulDivReqMsg.v"
`include "imuldiv-IntMulIterative.v"
`include "vc-TestSource.v"
`include "vc-TestSink.v"
`include "vc-Test.v"

//------------------------------------------------------------------------
// Helper Module
//------------------------------------------------------------------------

// wraps the Testsource and TestSink together
module imuldiv_IntMulIterative_helper
(
  input       clk,
  input       reset,
  output      done
);

  wire [66:0] src_msg;
  wire [31:0] src_msg_a;
  wire [31:0] src_msg_b;
  wire        src_val;
  wire        src_rdy;
  wire        src_done;

  wire [63:0] sink_msg;
  wire        sink_val;
  wire        sink_rdy;
  wire        sink_done;

  assign done = src_done && sink_done;

  // contains a local memory that stores the sequence of messages we would like to test our muldiv unit with
  // TestSource entry's widths must be equal to the width of two operands in addition to the function specifier bits: 2 * 32 + 3 = 67
  vc_TestSource#(67,3) src
  (
    .clk   (clk),
    .reset (reset),
    .bits  (src_msg),
    .val   (src_val),
    .rdy   (src_rdy),
    .done  (src_done)
  );

  imuldiv_MulDivReqMsgFromBits msgfrombits
  (
    .bits (src_msg),
    .func (),
    .a    (src_msg_a),
    .b    (src_msg_b)
  );

  imuldiv_IntMulIterative imul
  (
    .clk                (clk),
    .reset              (reset),
    .mulreq_msg_a       (src_msg_a),
    .mulreq_msg_b       (src_msg_b),
    .mulreq_val         (src_val),
    .mulreq_rdy         (src_rdy),
    .mulresp_msg_result (sink_msg),
    .mulresp_val        (sink_val),
    .mulresp_rdy        (sink_rdy)
  );

  // contains a local memory that stores the expected responses for each of the corresponding messages
  vc_TestSink#(64,3) sink
  (
    .clk   (clk),
    .reset (reset),
    .bits  (sink_msg),
    .val   (sink_val),
    .rdy   (sink_rdy),
    .done  (sink_done)
  );

endmodule

//------------------------------------------------------------------------
// Main Tester Module
//------------------------------------------------------------------------

module tester;

  // VCD Dump
  initial begin
    $dumpfile("imuldiv-IntMulIterative.vcd");
    $dumpvars;
  end

  `VC_TEST_SUITE_BEGIN( "imuldiv-IntMulIterative" )

  reg  t0_reset = 1'b1;
  wire t0_done;

  imuldiv_IntMulIterative_helper t0
  (
    .clk   (clk),
    .reset (t0_reset),
    .done  (t0_done)
  );

  `VC_TEST_CASE_BEGIN( 1, "mul" )
  begin

    // GIVEN TESTS

    // zero * zero is zero
    t0.src.m[0] = 67'h0_00000000_00000000; t0.sink.m[0] = 64'h00000000_00000000;
    // one times one is one
    t0.src.m[1] = 67'h0_00000001_00000001; t0.sink.m[1] = 64'h00000000_00000001;
    // -1 * 1 is negative_one?
    t0.src.m[2] = 67'h0_ffffffff_00000001; t0.sink.m[2] = 64'hffffffff_ffffffff;
    // 1 * -1 is -1
    t0.src.m[3] = 67'h0_00000001_ffffffff; t0.sink.m[3] = 64'hffffffff_ffffffff;
    // -1 * -1 = 1
    t0.src.m[4] = 67'h0_ffffffff_ffffffff; t0.sink.m[4] = 64'h00000000_00000001;
    // 8 * 3 is 24
    t0.src.m[5] = 67'h0_00000008_00000003; t0.sink.m[5] = 64'h00000000_00000018;
    // -8 * 8 = -64
    t0.src.m[6] = 67'h0_fffffff8_00000008; t0.sink.m[6] = 64'hffffffff_ffffffc0;
    // -8 * -8 = 64
    t0.src.m[7] = 67'h0_fffffff8_fffffff8; t0.sink.m[7] = 64'h00000000_00000040;
    t0.src.m[8] = 67'h0_0deadbee_10000000; t0.sink.m[8] = 64'h00deadbe_e0000000;
    t0.src.m[9] = 67'h0_deadbeef_10000000; t0.sink.m[9] = 64'hfdeadbee_f0000000;

    // my tests
    // zero * one
    t0.src.m[10] = 67'h0_00000000_00000001; t0.sink.m[10] = 64'h00000000_00000000;
    // zero * negative one
    t0.src.m[11] = 67'h0_00000000_ffffffff; t0.sink.m[11] = 64'h00000000_00000000;
    t0.src.m[1] = 67'h0_00000000_ffffffff; t0.sink.m[11] = 64'h00000000_00000000;

    #5;   t0_reset = 1'b1;
    #20;  t0_reset = 1'b0;
    #10000; `VC_TEST_CHECK( "Is sink finished?", t0_done )

  end
  `VC_TEST_CASE_END

  `VC_TEST_SUITE_END( 1 )

endmodule
