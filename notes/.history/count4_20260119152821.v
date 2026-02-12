
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
    .x()
)


endmodule