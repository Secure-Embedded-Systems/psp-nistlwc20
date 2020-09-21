module gift128(input  wire clk,
               input  wire [127:0] P,
               input  wire [127:0] K,
	           input  wire ld, 
               output wire [127:0] C,
               output wire done);

	localparam [0:40*6-1] GIFT_RC = {
     6'h01,6'h03,6'h07,6'h0F,6'h1F,6'h3E,6'h3D,6'h3B,6'h37,6'h2F,
     6'h1E,6'h3C,6'h39,6'h33,6'h27,6'h0E,6'h1D,6'h3A,6'h35,6'h2B,
     6'h16,6'h2C,6'h18,6'h30,6'h21,6'h02,6'h05,6'h0B,6'h17,6'h2E,
     6'h1C,6'h38,6'h31,6'h23,6'h06,6'h0D,6'h1B,6'h36,6'h2D,6'h1A};

	reg  [127:0] state;
	reg  [127:0] roundkey;
    reg  [5:0]   round;
    wire [127:0] round_out;
    wire [127:0] roundkey_out;

    wire  [127:0] next_state;
    wire  [127:0] next_roundkey;
    wire  [5:0]   next_round;

`define S0(V) V[(128    )-1 -: 32]
`define S1(V) V[(128- 32)-1 -: 32]
`define S2(V) V[(128- 64)-1 -: 32]
`define S3(V) V[(128- 96)-1 -: 32]
`define W0(V) V[(128    )-1 -: 16]
`define W1(V) V[(128- 16)-1 -: 16]
`define W2(V) V[(128- 32)-1 -: 16]
`define W3(V) V[(128- 48)-1 -: 16]
`define W4(V) V[(128- 64)-1 -: 16]
`define W5(V) V[(128- 80)-1 -: 16]
`define W6(V) V[(128- 96)-1 -: 16]
`define W7(V) V[(128-112)-1 -: 16]

    wire [127:0] sub_1, sub_2, sub_3;

    assign `S1(sub_1) =  `S1(state) ^ (`S0(state) & `S2(state));
    assign `S0(sub_1) =  `S0(state) ^ (`S1(sub_1) & `S3(state));
    assign `S2(sub_1) =  `S2(state) ^ (`S0(sub_1) | `S1(sub_1));
    assign `S3(sub_1) =  `S3(state) ^ `S2(sub_1);

    assign `S1(sub_2) =  `S1(sub_1) ^ `S3(sub_1);
    assign `S3(sub_2) = ~`S3(sub_1);
    assign `S2(sub_2) =  `S2(sub_1) ^ (`S0(sub_1) & `S1(sub_2));
    assign `S0(sub_2) =  `S0(sub_1);

    assign `S0(sub_3) =  `S3(sub_2);
    assign `S1(sub_3) =  `S1(sub_2);
    assign `S2(sub_3) =  `S2(sub_2);
    assign `S3(sub_3) =  `S0(sub_2);

    wire [31:0] pq0, pq1, pq2, pq3;
    wire [31:0] pi0, pi1, pi2, pi3;

    assign pi0 = `S0(sub_3);
    assign pi1 = `S1(sub_3);
    assign pi2 = `S2(sub_3);
    assign pi3 = `S3(sub_3);

    assign {pq0[15], pq0[23], pq0[31], pq0[ 7], pq0[14], pq0[22], pq0[30], pq0[ 6], 
            pq0[13], pq0[21], pq0[29], pq0[ 5], pq0[12], pq0[20], pq0[28], pq0[ 4], 
            pq0[11], pq0[19], pq0[27], pq0[ 3], pq0[10], pq0[18], pq0[26], pq0[ 2], 
            pq0[ 9], pq0[17], pq0[25], pq0[ 1], pq0[ 8], pq0[16], pq0[24], pq0[ 0]} = pi0;

    assign {pq1[23], pq1[31], pq1[ 7], pq1[15], pq1[22], pq1[30], pq1[ 6], pq1[14], 
            pq1[21], pq1[29], pq1[ 5], pq1[13], pq1[20], pq1[28], pq1[ 4], pq1[12], 
            pq1[19], pq1[27], pq1[ 3], pq1[11], pq1[18], pq1[26], pq1[ 2], pq1[10], 
            pq1[17], pq1[25], pq1[ 1], pq1[ 9], pq1[16], pq1[24], pq1[ 0], pq1[ 8]} = pi1;

    assign {pq2[31], pq2[ 7], pq2[15], pq2[23], pq2[30], pq2[ 6], pq2[14], pq2[22], 
            pq2[29], pq2[ 5], pq2[13], pq2[21], pq2[28], pq2[ 4], pq2[12], pq2[20], 
            pq2[27], pq2[ 3], pq2[11], pq2[19], pq2[26], pq2[ 2], pq2[10], pq2[18], 
            pq2[25], pq2[ 1], pq2[ 9], pq2[17], pq2[24], pq2[ 0], pq2[ 8], pq2[16]} = pi2;

    assign {pq3[ 7], pq3[15], pq3[23], pq3[31], pq3[ 6], pq3[14], pq3[22], pq3[30], 
            pq3[ 5], pq3[13], pq3[21], pq3[29], pq3[ 4], pq3[12], pq3[20], pq3[28], 
            pq3[ 3], pq3[11], pq3[19], pq3[27], pq3[ 2], pq3[10], pq3[18], pq3[26], 
            pq3[ 1], pq3[ 9], pq3[17], pq3[25], pq3[ 0], pq3[ 8], pq3[16], pq3[24]} = pi3;

	wire [31:0] ark0, ark1, ark2, ark3;

	assign ark0 = pq0;
	assign ark1 = {`W6(roundkey), `W7(roundkey)} ^ pq1;
	assign ark2 = {`W2(roundkey), `W3(roundkey)} ^ pq2;
	assign ark3 = GIFT_RC[6*round +: 6] ^ pq3 ^ 32'h80000000;

    wire [127:0] ark_out;

	assign ark_out = {ark0, ark1, ark2, ark3};

`define ROTRIGHT(V,S) {V[S-1:0], V[15:S]}

	wire [15:0] rw0, rw1, rw2, rw3, rw4, rw5, rw6, rw7;

	assign {rw0, rw1, rw2, rw3, rw4, rw5, rw6, rw7} = roundkey;

	assign roundkey_out = {`ROTRIGHT(rw6, 2), `ROTRIGHT(rw7, 12),
							rw0, rw1, rw2, rw3, rw4, rw5};

    assign round_out = ark_out;

    assign next_state = (ld) ? P : round_out;
    assign next_roundkey = (ld)   ? K : roundkey_out;
    assign next_round    = (ld)   ? 4'h0 : 
                           (done) ? 4'h0 : round + 4'h1;

    always @(posedge clk)
    begin
      state    <= next_state;
      roundkey <= next_roundkey; 
      round    <= next_round;
    end

    assign C = ark_out;
    assign done = (round == 6'd39);

endmodule
