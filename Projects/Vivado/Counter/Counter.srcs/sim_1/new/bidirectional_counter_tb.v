`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/18/2017 07:45:21 PM
// Design Name: 
// Module Name: bidirectional_counter_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module bidirectional_counter_tb(

    );
    
    reg up;
    reg down;
    reg clk;
    reg rst;
    wire [15:0] out;
    
    // Bidirecional counter
    bidirectional_counter uut (
    .up(up),
    .down(down),
    .clk(clk),
    .rst(rst),
    .x(out)
    );
    
    initial
    begin
        up <= 'b0;
        down <= 'b0;
        clk <= 'b0;
        rst <= 'b0;
        #10 rst <= 'b1;
        #50 rst <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 down <= 'b1;
        #10 down <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        #10 up <= 'b0;
        #10 up <= 'b1;
        down <= 'b1;
        #10 up <= 'b0;
        down <= 'b0;
        #10 up <= 'b1;
        down <= 'b1;
        #10 up <= 'b0;
        down <= 'b0;
        #10 up <= 'b1;
        down <= 'b1;
        #10 up <= 'b0;
        down <= 'b0;
        #10 up <= 'b1;
        #100 up <= 'b0;
        #10 down <= 'b1;
        #100 down <= 'b0;
    end
    
    always
        #5 clk <= ~clk;
    
endmodule
