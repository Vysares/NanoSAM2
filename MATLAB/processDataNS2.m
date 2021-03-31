%% USAGE %%
%   processDataNS2 takes the serial output of the NanoSAM II payload
%       and plots it
%   expects temperature, teensy photodiode voltage, and ADC photo voltage
%       but can handle any of these being missing
%   
%   change filename to be the name of the data file to be analyzed
%   change filepath to be the relative path between this script location 
%       and the location of the data file

%% - - - - - - -  main script  - - - - - - -

clear; close all; clc

% config
filename = 'biggerFile.txt'; % only the name of the file and extension
filepath = ''; % absolute or relative path, must end in slash

% initialization 
fid = fopen([filepath filename]); % file ID to track lines we have read 
linesRead = 0;
linesIgnored = 0;
photoDirVals = zeros(0,2);  % timestamp (s), voltage (V)
photoSpiVals = zeros(0,2);  % timestamp (s), voltage (V)

% timestamp (s), heater (1=on,0=off), optics (C), analog (C), digital (C)
tempVals = zeros(0,5);      

% check strings
photoDirCheck = 'PHOTO_DIR,';
photoSpiCheck = 'PHOTO_SPI,';
tempCheck = 'TEMP,';

% guard to prevent comparisons between a char array that is too short
minLineSize = max([strlength(photoDirCheck), strlength(photoSpiCheck),...
    strlength(tempCheck)]);

% - - - - - - - data reading - - - - - - - %

% read until end of file
while (true)
    readLine = fgetl(fid); % read one line
    
    % check for end of file
    if ~ischar(readLine), break, end
    linesRead = linesRead + 1; % increment the line count
    
    % sort line according to first string 
    if (strlength(readLine) < minLineSize)
        % ignore line if it is too short to compare
        lineIgnored = linesIgnored + 1;
        
    elseif (strcmp(readLine(1:strlength(photoDirCheck)), photoDirCheck))
        % photodiode teensy measurement found
        readLine = erase(readLine, ','); % remove commas
        split = strsplit(readLine); % split whitespace delimited
        photoDirVals(end+1, 1) = millisec2sec(split{2}); % timestamp (s)
        photoDirVals(end, 2) = str2double(split{3}); % voltage
    
    elseif (strcmp(readLine(1:strlength(photoSpiCheck)), photoSpiCheck))
        % photodiode SPI measurement found
        readLine = erase(readLine, ','); % remove commas
        split = strsplit(readLine); % split whitespace delimited
        photoSpiVals(end+1, 1) = millisec2sec(split{2}); % timestamp (s)
        photoSpiVals(end, 2) = str2double(split{3}); % voltage
            
    elseif (strcmp(readLine(1:strlength(tempCheck)), tempCheck))
        % temp measurement found
        readLine = erase(readLine, ','); % remove commas
        split = strsplit(readLine); % split whitespace delimited
        tempVals(end+1, 1) = millisec2sec(split{2}); % timestamp (s)
        tempVals(end, 2) = str2double(split{3});   % heater on bool
        tempVals(end, 3) = str2double(split{4});   % optics temp (C)
        tempVals(end, 4) = str2double(split{5});   % analog temp (C)
        tempVals(end, 5) = str2double(split{6});   % digital temp (C)
     
    else
        % ignored line 
        linesIgnored = linesIgnored + 1;
        
    end
        
end

% close the file 
fclose(fid);


% - - - - - - - post processing - - - - - - - %
% normalize voltage and temp timestamps to smallest timestamp
minTimestamp = getMinTimestamp(photoSpiVals(1,1), photoDirVals(1,1),...
    tempVals(1,1));
photoSpiVals(:,1) = photoSpiVals(:,1) - minTimestamp;
photoDirVals(:,1) = photoDirVals(:,1) - minTimestamp;
tempVals(:,1) = tempVals(:,1) - minTimestamp;

% plots
plotPhotodiode(photoDirVals, photoSpiVals);
plotTemp(tempVals);


% end of main


%% - - - - - - -  Helper functions  - - - - - - -
function sec = millisec2sec(ms)
 % takes char array input, outputs a double
    sec = str2double(ms) / 1000;

end

function minTS = getMinTimestamp(spi, dir, temp)
 % normalizes timestamps by the smallest in the entire dataset
    minTS = min([spi, dir, temp]);
end

function plotPhotodiode(photoDirVals, photoSpiVals)
 % plots photodiode results
    figure 
    plot(photoDirVals(:,1), photoDirVals(:,2), "linewidth", 2)
    hold on 
    plot(photoSpiVals(:,1), photoSpiVals(:,2), "linewidth", 2)
    
    title("Photodiode Voltages")
    xlabel("Time (s)")
    ylabel("Voltage (V)")
    
    legend("Teensy ADC", "16-Bit ADC")

end

function plotTemp(tempVals)
 % plots temperature results
    figure % for temps
    time = tempVals(:,1);
    plot(time, tempVals(:,3), "linewidth", 2)
    hold on 
    plot(time, tempVals(:,4), "linewidth", 2)
    plot(time, tempVals(:,5), "linewidth", 2)
    
    title("Temperatures")
    xlabel("Time (s)")
    ylabel("Temperature (C)")
    
    % plot temp requirement
    yline(-20, 'g--', "linewidth", 2);
    yline(60, 'g--', "linewidth", 2);
    
    legend("Optics", "Analog", "Digital", "Temp Requirement Bounds")
    
    figure % for heater status
    plot(time, tempVals(:,2), "linewidth", 2)
    title("Heater Status")
    xlabel("Time (s)")
    ylabel("1 = on, 0 = off")
    ylim([-.1 1.1]);

end