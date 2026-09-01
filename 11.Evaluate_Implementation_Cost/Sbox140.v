module sbox(
    input  wire [3:0] x,
    output reg  [3:0] y
);

always @(*) begin
    case (x)
        4'h0: y = 4'h4;
        4'h1: y = 4'h7;
        4'h2: y = 4'hb;
        4'h3: y = 4'h8;
        4'h4: y = 4'h1;
        4'h5: y = 4'ha;
        4'h6: y = 4'h6;
        4'h7: y = 4'hd;
        4'h8: y = 4'h9;
        4'h9: y = 4'he;
        4'ha: y = 4'h2;
        4'hb: y = 4'h5;
        4'hc: y = 4'hf;
        4'hd: y = 4'h0;
        4'he: y = 4'hc;
        4'hf: y = 4'h3;
        default: y = 4'h0;
    endcase
end

endmodule
