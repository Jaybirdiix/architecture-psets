// test file

module half_adder_test ();
    // registers to hold current state
    reg [3:0] x;
    
    wire [3:0] y;

    wire c_out;

    // dut -> design under test
    inc4 dut (
        // how do we connect singles to input output of the circuit?
        .x(x),
        .y(y),
        .c_out(c_out)
    );

    initial begin 
        // inbuilt simulator functions
        $dumpfile("dump.vcd");
        $dumpvars;
    end

    // test cases
    initial begin
        x <= 4'h0;
        #10

        x <= 4'h4;
        $strobe("x = %h , y = %h , c_out = %h", x, y, c_out);
        #1

        x <= 4'h7;
        $strobe("x = %h , y = %h , c_out = %h", x, y, c_out);
        #1

        x <= 4'hf;
        $strobe("x = %h , y = %h , c_out = %h", x, y, c_out);
        #1

        x <= 4'h0;
        $strobe("x = %h , y = %h , c_out = %h", x, y, c_out);
        #1

        $finish;

    end

endmodule;

// iverilog -o inc4 inc4.t.v inc4.v half_adder.v
