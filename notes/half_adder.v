
// module statement, start module
module half_adder (
    input a,
    input b,
    output wire s,
    // carry
    output wire c_out
);

    // value of this wire s at all times is a xor b
    assign s = a ^ b;
    assign c_out = a & b;

    // assign statements used on wires ad only outside procedure blocks
    
// end module
endmodule;


// iverilog -o half_adder half_adder.v half_adder.t.v
// ./half_adder