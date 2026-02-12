// test file

module half_adder_test ();
    // registers to hold current state
    reg a, b;

    // wires
    // to read ?
    wire s, c_out;

    // dut -> design under test
    half_adder dut (
        // how do we connect singles to input output of the circuit?
        .a(a),
        .b(b),
        .s(s),
        .c_out(c_out)
    );

    initial begin 
        // inbuilt simulator functions
        $dumpfile("dump.vcd");
        $dumpvars;
    end

    // test cases
    initial begin
        // as soon as the simulation starts, set a and b to zero
        a <= 0;
        b <= 0;
        #1 // delay for verilog?

        // feed different values into the circuit and see what values are
        a <= 0;
        b <= 0;
        // inbuilt simulator function
        $strobe("a = %h , b = %h , s = %h , c_out = %h", a, b, s, c_out);
        #1

        a <= 1;
        b <= 0;
        $strobe("a = %h , b = %h , s = %h , c_out = %h", a, b, s, c_out);
        #1

        a <= 0;
        b <= 1;
        $strobe("a = %h , b = %h , s = %h , c_out = %h", a, b, s, c_out);
        #1

        a <= 1;
        b <= 1;
        $strobe("a = %h , b = %h , s = %h , c_out = %h", a, b, s, c_out);
        #1

        $finish;



    end

endmodule;