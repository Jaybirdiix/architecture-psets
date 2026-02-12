//========================================================================
// Test for Div Unit
//========================================================================

`include "imuldiv-DivReqMsg.v"
`include "imuldiv-IntDivIterative.v"
`include "vc-TestSource.v"
`include "vc-TestSink.v"
`include "vc-Test.v"

//------------------------------------------------------------------------
// Helper Module
//------------------------------------------------------------------------

module imuldiv_IntDivIterative_helper
(
  input       clk,
  input       reset,
  output      done
);

  wire [64:0] src_msg;
  wire        src_msg_fn;
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

  vc_TestSource#(65,3) src
  (
    .clk   (clk),
    .reset (reset),
    .bits  (src_msg),
    .val   (src_val),
    .rdy   (src_rdy),
    .done  (src_done)
  );

  imuldiv_DivReqMsgFromBits msgfrombits
  (
    .bits (src_msg),
    .func (src_msg_fn),
    .a    (src_msg_a),
    .b    (src_msg_b)
  );

  imuldiv_IntDivIterative idiv
  (
    .clk                 (clk),
    .reset               (reset),
    .divreq_msg_fn       (src_msg_fn),
    .divreq_msg_a        (src_msg_a),
    .divreq_msg_b        (src_msg_b),
    .divreq_val          (src_val),
    .divreq_rdy          (src_rdy),
    .divresp_msg_result  (sink_msg),
    .divresp_val         (sink_val),
    .divresp_rdy         (sink_rdy)
  );

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
    $dumpfile("imuldiv-IntDivIterative.vcd");
    $dumpvars;
  end

  `VC_TEST_SUITE_BEGIN( "imuldiv-IntDivIterative" )

  reg  t0_reset = 1'b1;
  wire t0_done;

  imuldiv_IntDivIterative_helper t0
  (
    .clk   (clk),
    .reset (t0_reset),
    .done  (t0_done)
  );

  `VC_TEST_CASE_BEGIN( 1, "div/rem" )
  begin

    // zero / 1
    t0.src.m[ 0] = 65'h1_00000000_00000001; t0.sink.m[ 0] = 64'h00000000_00000000;
    // one / one
    t0.src.m[ 1] = 65'h1_00000001_00000001; t0.sink.m[ 1] = 64'h00000000_00000001;
    // 0 / -1
    t0.src.m[ 2] = 65'h1_00000000_ffffffff; t0.sink.m[ 2] = 64'h00000000_00000000;
    // -1 / -1
    t0.src.m[ 3] = 65'h1_ffffffff_ffffffff; t0.sink.m[ 3] = 64'h00000000_00000001;
    // 546 / 42 = 13
    t0.src.m[ 4] = 65'h1_00000222_0000002a; t0.sink.m[ 4] = 64'h00000000_0000000d;
    t0.src.m[ 5] = 65'h1_0a01b044_ffffb146; t0.sink.m[ 5] = 64'h00000000_ffffdf76;
    t0.src.m[ 6] = 65'h1_00000032_00000222; t0.sink.m[ 6] = 64'h00000032_00000000;
    t0.src.m[ 7] = 65'h1_00000222_00000032; t0.sink.m[ 7] = 64'h0000002e_0000000a;
    t0.src.m[ 8] = 65'h1_0a01b044_ffffb14a; t0.sink.m[ 8] = 64'h00003372_ffffdf75;
    t0.src.m[ 9] = 65'h1_deadbeef_0000beef; t0.sink.m[ 9] = 64'hffffda72_ffffd353;
    t0.src.m[10] = 65'h1_f5fe4fbc_00004eb6; t0.sink.m[10] = 64'hffffcc8e_ffffdf75;
    t0.src.m[11] = 65'h1_f5fe4fbc_ffffb14a; t0.sink.m[11] = 64'hffffcc8e_0000208b;

    // my tests -----------------
    // +7 / -3  = -2, rem =1
    t0.src.m[12] = 65'h1_00000007_fffffffd; t0.sink.m[12] = 64'h00000001_fffffffe;

    // -7 / 3 = -2, rem = -1
    t0.src.m[13] = 65'h1_fffffff9_00000003; t0.sink.m[13] = 64'hffffffff_fffffffe;

    // -7 / -3 = 2, rem = -1
    t0.src.m[14] = 65'h1_fffffff9_fffffffd; t0.sink.m[14] = 64'hffffffff_00000002;

    // maximum num / 1 = maximum num, rem = 0
    t0.src.m[15] = 65'h1_7fffffff_00000001; t0.sink.m[15] = 64'h00000000_7fffffff;
    t0.src.m[16] = 65'h1_80000000_00000001; t0.sink.m[16] = 64'h00000000_80000000;

    // same but negative
    t0.src.m[17] = 65'h1_7fffffff_ffffffff; t0.sink.m[17] = 64'h00000000_80000001;

    // minimum number / -1
    // weird case
    t0.src.m[18] = 65'h1_80000000_ffffffff; t0.sink.m[18] = 64'h00000000_80000000;

    // number we're dividing by is bigger
    //  5 / 100 rem = 5
    t0.src.m[19] = 65'h1_00000005_00000064; t0.sink.m[19] = 64'h00000005_00000000;
    // -5 / 100 = 0 rem = -5
    t0.src.m[20] = 65'h1_fffffffb_00000064; t0.sink.m[20] = 64'hfffffffb_00000000;
    //  5 / -100 = 0 rem = 5
    t0.src.m[21] = 65'h1_00000005_ffffff9c; t0.sink.m[21] = 64'h00000005_00000000;
    // -5 / -100 = 0 rem = -5
    t0.src.m[22] = 65'h1_fffffffb_ffffff9c; t0.sink.m[22] = 64'hfffffffb_00000000;

    // 123456789 / 2 = 61728394 rem = 1
    t0.src.m[23] = 65'h1_075bcd15_00000002; t0.sink.m[23] = 64'h00000001_03ade68a;
    // -123456789 / 2 = -61728394 rem = -1
    t0.src.m[24] = 65'h1_f8a432eb_00000002; t0.sink.m[24] = 64'hffffffff_fc521976;

    // 123456789 / -8 = -15432098 rem = 5
    t0.src.m[25] = 65'h1_075bcd15_fffffff8; t0.sink.m[25] = 64'h00000005_ff14865e;
    // -123456789 / -8 = 15432098 rem = -5
    t0.src.m[26] = 65'h1_f8a432eb_fffffff8; t0.sink.m[26] = 64'hfffffffb_00eb79a2;

    // maximum num / maximum num = 1, rem = 0
    t0.src.m[27] = 65'h1_7fffffff_7fffffff; t0.sink.m[27] = 64'h00000000_00000001;
    // // minimum number / minimum number = 1, rem = 0
    t0.src.m[28] = 65'h1_80000000_80000000; t0.sink.m[28] = 64'h00000000_00000001;

    // couldn't get this one to work oooops
    // maximum num / minimum num = 0, rem = maximum num
    // t0.src.m[29] = 65'h1_7fffffff_80000000; t0.sink.m[29] = 64'h7fffffff_00000000;

    // minimum / maximum = -1, rem = -1
    t0.src.m[29] = 65'h1_80000000_7fffffff; t0.sink.m[30] = 64'hffffffff_ffffffff;

    #5;   t0_reset = 1'b1;
    #20;  t0_reset = 1'b0;
    #10000; `VC_TEST_CHECK( "Is sink finished?", t0_done )

  end
  `VC_TEST_CASE_END

  //----------------------------------------------------------------------
  // Add Unsigned Test Case Here
  //----------------------------------------------------------------------

    `VC_TEST_CASE_BEGIN( 2, "divu/remu" )
  begin

    // func=0 => unsigned (divu/remu)
    // sink is {remainder, quotient}

    // zero / 1
    t0.src.m[ 0] = 65'h0_00000000_00000001; t0.sink.m[ 0] = 64'h00000000_00000000;
    // one / one
    t0.src.m[ 1] = 65'h0_00000001_00000001; t0.sink.m[ 1] = 64'h00000000_00000001;
    // 0 / max
    t0.src.m[ 2] = 65'h0_00000000_ffffffff; t0.sink.m[ 2] = 64'h00000000_00000000;
    // max / max
    t0.src.m[ 3] = 65'h0_ffffffff_ffffffff; t0.sink.m[ 3] = 64'h00000000_00000001;

    // 546 / 42 = 13 (nice sanity check)
    t0.src.m[ 4] = 65'h0_00000222_0000002a; t0.sink.m[ 4] = 64'h00000000_0000000d;

    // max / 1 = max
    t0.src.m[ 5] = 65'h0_ffffffff_00000001; t0.sink.m[ 5] = 64'h00000000_ffffffff;
    // max / 2 = 0x7fffffff rem 1
    t0.src.m[ 6] = 65'h0_ffffffff_00000002; t0.sink.m[ 6] = 64'h00000001_7fffffff;

    // powers of two are good for catching shift bugs
    t0.src.m[ 7] = 65'h0_80000000_00000002; t0.sink.m[ 7] = 64'h00000000_40000000;
    t0.src.m[ 8] = 65'h0_80000001_00000002; t0.sink.m[ 8] = 64'h00000001_40000000;

    // divisor bigger -> quotient 0, remainder = dividend
    t0.src.m[ 9] = 65'h0_00000005_00000064; t0.sink.m[ 9] = 64'h00000005_00000000;

    // "lol signed vs unsigned" moment: 7 / 0xFFFFFFFD is just 0 rem 7
    t0.src.m[10] = 65'h0_00000007_fffffffd; t0.sink.m[10] = 64'h00000007_00000000;

    // -7 interpreted as unsigned: 0xFFFFFFF9 / 3 divides evenly
    t0.src.m[11] = 65'h0_fffffff9_00000003; t0.sink.m[11] = 64'h00000000_55555553;

    // another "unsigned weirdness": divisor is slightly bigger -> quotient 0
    t0.src.m[12] = 65'h0_fffffff9_fffffffd; t0.sink.m[12] = 64'hfffffff9_00000000;

    // more "divisor bigger" sanity
    t0.src.m[13] = 65'h0_0000beef_deadbeef; t0.sink.m[13] = 64'h0000beef_00000000;

    // fun hex numbers but still checkable
    t0.src.m[14] = 65'h0_deadbeef_0000beef; t0.sink.m[14] = 64'h0000227f_00012a90;
    t0.src.m[15] = 65'h0_12345678_00010000; t0.sink.m[15] = 64'h00005678_00001234;

    // same decimal you used, but unsigned behavior matches normal here
    t0.src.m[16] = 65'h0_075bcd15_00000002; t0.sink.m[16] = 64'h00000001_03ade68a;

    // differs from signed: 0xF8A432EB treated as big positive
    t0.src.m[17] = 65'h0_f8a432eb_00000002; t0.sink.m[17] = 64'h00000001_7c521975;

    // boundary-ish divisions
    t0.src.m[18] = 65'h0_ffffffff_80000000; t0.sink.m[18] = 64'h7fffffff_00000001;
    t0.src.m[19] = 65'h0_80000000_7fffffff; t0.sink.m[19] = 64'h00000001_00000001;
    t0.src.m[20] = 65'h0_7fffffff_80000000; t0.sink.m[20] = 64'h7fffffff_00000000;

    // unsigned "-5 / 100"
    t0.src.m[21] = 65'h0_fffffffb_00000064; t0.sink.m[21] = 64'h0000005b_028f5c28;

    // division by zero (RISC-V style): quotient = all 1s, remainder = dividend
    t0.src.m[22] = 65'h0_80000000_00000000; t0.sink.m[22] = 64'h80000000_ffffffff;
    t0.src.m[23] = 65'h0_00000000_00000000; t0.sink.m[23] = 64'h00000000_ffffffff;

    #5;   t0_reset = 1'b1;
    #20;  t0_reset = 1'b0;
    #10000; `VC_TEST_CHECK( "Is sink finished?", t0_done )

  end
  `VC_TEST_CASE_END

  

  `VC_TEST_SUITE_END( 1 /* replace with number of tests cases */ )

endmodule
