function [commands, arguments, CRCs, bits] = decodeSDCMD(CMD,SDCLK)
% Decode funtion for SD CARD command signal

length_of_command = 48;

first_index_of_command_index = 47-45;
length_of_command_index = 6;

first_index_of_argument = 47-39;
length_of_argument = 32;

first_index_of_CRC = 47-7;
length_of_CRC = 7;

index=1;
start_sample = 0;

downsample_phase = find(SDCLK);
SDCLK = downsample(SDCLK,2,downsample_phase(1)-1);
CMD = downsample(CMD,2,downsample_phase(1)-1); 

i = 1;

while(i<length(SDCLK))
    if(CMD(i) == 0)
        start_sample = i;
        command_index = 0;
        arg(1) = 0;
        arg(2) = 0;
        arg(3) = 0;
        arg(4) = 0;
        CRC = 0;
        
%         disp('Command bits: ');
%         num2str(CMD(start_sample+first_index_of_command_index:start_sample+first_index_of_command_index+length_of_command_index-1)')
%         disp('Argument bits: ');
%         num2str(CMD(start_sample+first_index_of_argument:start_sample+first_index_of_argument+length_of_argument-1)')
%         disp('CRC bits: ');
%         num2str(CMD(start_sample+first_index_of_CRC:start_sample+first_index_of_CRC+length_of_CRC-1)')
        
        if(start_sample+length_of_command-1<length(SDCLK))
            for j=0:length_of_command_index-1
                i=start_sample+first_index_of_command_index+j;
                command_index = command_index+CMD(i)*2^(length_of_command_index-j-1);
            end
            
            for k=1:4
                for j=0:length_of_argument/4-1
                    i=start_sample+first_index_of_argument+(k-1)*length_of_argument/4+j;
                    arg(k) = arg(k)+CMD(i)*2^(length_of_argument/4-j-1);
                end
            end
            
            for j=0:length_of_CRC-1
                i=start_sample+first_index_of_CRC+j;
                CRC = CRC+CMD(i)*2^(length_of_CRC-j-1);
            end
            
            if(CMD(start_sample+1))
                commands{index} = strcat('CMD',num2str(command_index));
            else
                commands{index} = strcat('RESP',num2str(command_index));
            end
            arguments{index}(1) = arg(1);
            arguments{index}(2) = arg(2);
            arguments{index}(3) = arg(3);
            arguments{index}(4) = arg(4);
            CRCs{index} = CRC;
            bits{index} = CMD(start_sample:start_sample+length_of_command-1);
            index = index+1;
        end
        i = start_sample + 48;
    end
    
    i = i+1;
end

end

