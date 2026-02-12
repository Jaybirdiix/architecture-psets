
// inc4 adds 1 to a 4 bit input signal x to produce y
module inc4 (
    // 4 bit signal, labeled from 3 to zero (highest is 3)
    input [3:0] x,
    output wire [3:0] y,
    output wire c_out
);

wire c0, c1, c2, c3;
assign c_out = c3;

half_adder ha0 (
    // a comes from the zero'th bit of x
    .a(x[0]),
    .b(1'b1),
    .s(y[0]),
    .c_out(c0)
);

half_adder ha1 (
    .a(x[1]),
    .b(c0),
    .s(y[1]),
    .c_out(c1)
);

half_adder ha2 (
    .a(x[2]),
    .b(c1),
    .s(y[2]),
    .c_out(c2)
);

half_adder ha3 (
    .a(x[3]),
    .b(c2),
    .s(y[3]),
    .c_out(c3)
);


endmodule;