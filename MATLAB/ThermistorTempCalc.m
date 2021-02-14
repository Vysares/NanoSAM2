%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Use this script to generate a table of corresponding voltage/temperature
% values for use with the thermistors on NS2's boards.
% Output can be copied into thermLookup in hkUtil.hpp
%
% by Jackson Kistler
% modified 2/10/2021
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear all;
close all;

% Temperatures at which to find corresponding Vout
T_celsius = -30:5:40; % celsius

% Constants
R25 = 10000; % ohms, from datasheet
B = 3940; % from datasheet
Vcc = 3.3; % volts
Rs = 40000; % ohms

% Doing the math
T_kelvin = T_celsius + 273.15;
Rt = R25*exp(B*((1./T_kelvin) - (1./298.15)));
Vout = Vcc*(Rt./(Rt + Rs));

% print output
for i = 1:size(T_celsius,2)
    fprintf('{ %.3fF, %.3fF },\n', T_celsius(i), Vout(i))
end

% plot it
plot(Vout,T_celsius,'o')
grid on