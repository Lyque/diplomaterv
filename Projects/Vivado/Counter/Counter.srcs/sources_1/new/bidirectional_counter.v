`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/18/2017 06:54:53 PM
// Design Name: 
// Module Name: bidirectional_counter
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


module bidirectional_counter(
    input up,
    input down,
    input clk,
    input rst,
    output [15:0] x
    );
    
reg [2:0] upBuf;
reg [2:0] downBuf;
reg [15:0] cntrVal;
wire edgeUp;
wire edgeDown;
    
assign edgeUp = upBuf[1] & ~upBuf[2];
assign edgeDown = downBuf[1] & ~downBuf[2];

assign x[15:0] = cntrVal[15:0];
    
always @(posedge clk) 
begin
    if(rst == 1)
    begin
        upBuf[2:0] <= 'b000;
        downBuf[2:0] <= 'b000;
    end
    else
    begin
        upBuf[0] <= up;
        downBuf[0] <= down;
        upBuf[2:1] <= upBuf[1:0];
        downBuf[2:1] <= downBuf[1:0];
    end
end

always @(posedge clk) 
begin
    if(rst == 1)
    begin
        cntrVal[15:0] <= 'h0000;
    end
    else
    begin
        if(edgeUp == 1)
        begin
            if(edgeDown == 0)
            begin
                if(cntrVal[3:0] == 'h9)
                begin
                    if(cntrVal[7:4] == 'h9)
                    begin
                        if(cntrVal[11:8] == 'h9)
                        begin
                            if(cntrVal[15:12] == 'h9)
                            begin
                                cntrVal <= 'h0000;
                            end
                            else
                            begin
                                cntrVal[15:12] <= cntrVal[15:12] + 1;
                                cntrVal[11:0] <= 'h000;
                            end
                        end
                        else
                        begin
                            cntrVal[11:8] <= cntrVal[11:8] + 1;
                            cntrVal[7:0] <= 'h00;
                        end
                    end
                    else
                    begin
                        cntrVal[7:4] <= cntrVal[7:4] + 1;
                        cntrVal[3:0] <= 'h0;
                    end
                end
                else
                begin
                    cntrVal <= cntrVal + 1;
                end
            end
        end
        else
        if(edgeDown == 1)
        begin
            if(cntrVal[3:0] == 'h0)
            begin
                if(cntrVal[7:4] == 'h0)
                begin
                    if(cntrVal[11:8] == 'h0)
                    begin
                        if(cntrVal[15:12] == 'h0)
                        begin
                            cntrVal <= 'h9999;
                        end
                        else
                        begin
                            cntrVal[15:12] <= cntrVal[15:12] - 1;
                            cntrVal[11:0] <= 'h999;
                        end
                    end
                    else
                    begin
                        cntrVal[11:8] <= cntrVal[11:8] - 1;
                        cntrVal[7:0] <= 'h99;
                    end
                end
                else
                begin
                    cntrVal[7:4] <= cntrVal[7:4] - 1;
                    cntrVal[3:0] <= 'h9;
                end
            end
            else
            begin
                cntrVal <= cntrVal - 1;
            end
        end
    end
end
    
endmodule
