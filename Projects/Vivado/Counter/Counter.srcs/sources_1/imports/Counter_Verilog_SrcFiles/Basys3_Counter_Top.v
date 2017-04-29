`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Digilent Inc.
// Engineer: Varun Kondagunturi
// 
// Create Date:    17:08:26 06/12/2014 
// Design Name: 
// Module Name:    Counter_Top_Module 
// Project Name: 
// Target Devices: 
// Tool versions: 
//
//
// Description: 
//This is the Top-Level Source file for the Abacus Project. 
//Slide switches provide two 8-bit binary inputs A and B. 
//Slide Switches [15 down to 8] is input A.
//Slide Switches [7 down to 0] is input B.
//Inputs from the Push Buttons ( btnU, btnD, btnR, btnL) will allow the user to select different arithmetic operations that will be computed on the inputs A and B.
//btnU: Subtraction/Difference. Result will Scroll
//When A>B, difference is positive. 
//When A<B, difference is negative. If the button is not held down but just pressed once, the result will scroll. To find out if the result is negative, press and hold onto the push button btnU. This will show the negative sign. 
//btnD: Multiplication/Product. Result will Scroll
//btnR: Quotient(Division Operation). Press and Hold the button to display result
//btnL: Remainder ( Division Operation). Press and Hold the button to display result
//Output is displayed on the 7 segment LED display. 
//
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
 module Basys3_Counter_Top(

//CLK Input
	 input clk,
	 
//Push Button Inputs	 
	 input btnC,
	 
// Slide Switch Inputs
    input [15:0] sw, 
// Bidirectional counter
    input [1:0] JA,
   
// LED Outputs
     output [15:0] led,
     
// Seven Segment Display Outputs
     output [6:0] seg,
     output [3:0] an, 
     output dp
 );
	
// Bidirectional counter output
wire [15:0] cntrVal;
	
//Seven Segment Display Signal
reg [15:0] x;//input to seg7 to define segment pattern
wire clk200;

PLLE2_BASE #(
      .BANDWIDTH("OPTIMIZED"),  // OPTIMIZED, HIGH, LOW
      .CLKFBOUT_MULT(10),        // Multiply value for all CLKOUT, (2-64)
      .CLKFBOUT_PHASE(0.0),     // Phase offset in degrees of CLKFB, (-360.000-360.000).
      .CLKIN1_PERIOD(10.0),      // Input clock period in ns to ps resolution (i.e. 33.333 is 30 MHz).
      // CLKOUT0_DIVIDE - CLKOUT5_DIVIDE: Divide amount for each CLKOUT (1-128)
      .CLKOUT0_DIVIDE(5),
      .CLKOUT1_DIVIDE(1),
      .CLKOUT2_DIVIDE(1),
      .CLKOUT3_DIVIDE(1),
      .CLKOUT4_DIVIDE(1),
      .CLKOUT5_DIVIDE(1),
      // CLKOUT0_DUTY_CYCLE - CLKOUT5_DUTY_CYCLE: Duty cycle for each CLKOUT (0.001-0.999).
      .CLKOUT0_DUTY_CYCLE(0.5),
      .CLKOUT1_DUTY_CYCLE(0.5),
      .CLKOUT2_DUTY_CYCLE(0.5),
      .CLKOUT3_DUTY_CYCLE(0.5),
      .CLKOUT4_DUTY_CYCLE(0.5),
      .CLKOUT5_DUTY_CYCLE(0.5),
      // CLKOUT0_PHASE - CLKOUT5_PHASE: Phase offset for each CLKOUT (-360.000-360.000).
      .CLKOUT0_PHASE(0.0),
      .CLKOUT1_PHASE(0.0),
      .CLKOUT2_PHASE(0.0),
      .CLKOUT3_PHASE(0.0),
      .CLKOUT4_PHASE(0.0),
      .CLKOUT5_PHASE(0.0),
      .DIVCLK_DIVIDE(1),        // Master division value, (1-56)
      .REF_JITTER1(0.0),        // Reference input jitter in UI, (0.000-0.999).
      .STARTUP_WAIT("FALSE")    // Delay DONE until PLL Locks, ("TRUE"/"FALSE")
   )
   clk_multiplier (
      // Clock Outputs: 1-bit (each) output: User configurable clock outputs
      .CLKOUT0(clk200),   // 1-bit output: CLKOUT0
      .CLKOUT1(),   // 1-bit output: CLKOUT1
      .CLKOUT2(),   // 1-bit output: CLKOUT2
      .CLKOUT3(),   // 1-bit output: CLKOUT3
      .CLKOUT4(),   // 1-bit output: CLKOUT4
      .CLKOUT5(),   // 1-bit output: CLKOUT5
      // Feedback Clocks: 1-bit (each) output: Clock feedback ports
      .CLKFBOUT(), // 1-bit output: Feedback clock
      .LOCKED(),     // 1-bit output: LOCK
      .CLKIN1(clk),     // 1-bit input: Input clock
      // Control Ports: 1-bit (each) input: PLL control ports
      .PWRDWN(0),     // 1-bit input: Power-down
      .RST(btnC),           // 1-bit input: Reset
      // Feedback Clocks: 1-bit (each) input: Clock feedback ports
      .CLKFBIN(0)    // 1-bit input: Feedback clock
   );

always @(*)
begin
x <= cntrVal;
end

// Bidirecional counter
bidirectional_counter cntr (
.up(JA[0]),
.down(JA[1]),
.clk(clk200),
.rst(btnC),
.x(cntrVal)
);

seg7decimal u7 (

.x(x),
.clk(clk200),
.clr(btnC),
.a_to_g(seg),
.an(an),
.dp(dp)
);

endmodule