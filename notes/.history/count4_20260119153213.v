
module count4 (
    input clock,
    // signals
    input enable,
    input reset,
    // output
    output wire [3:0] count
)

// store the current amount
reg [3:0] value;

wire [3:0] inc4_out;

inc4 incrementer (
    .x(value),
    .y(inc4_out),
    // don't connect carry out (c_out)
);

// hold content of value register at all times
assign count = value;

// always when the clock has a positive edge (goes from zero to one)
// do the following
always @(posedge clock) begin
  // reset, everything goes to zero
  // if reset signal is equal to one
  if (reset == 1'b1)
//   four bit value of zero
    value <= 4'h0
end


endmodule