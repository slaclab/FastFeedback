function writeConfig(fbIOC, loop)
%
% disable loop
lcaPut(['FBCK:' fbIOC ':' loop ':STATE'], 'OFF');

%
% patterns
lcaPut(['FBCK:' fbIOC ':' loop ':POI1'], [12 23 41 23 32 53]);

%
% F matrix
lcaPut(['FBCK:' fbIOC ':' loop ':FMATRIX'], [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]);

%
% loop configs
lcaPut(['FBCK:' fbIOC ':' loop ':NAME'], 'TestLoop');
lcaPut(['FBCK:' fbIOC ':' loop ':INSTALLED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':TOTALPOI'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':ITERATEFUNC'], 'Sine');
%
% measurements
for i=1:40,
  name = sprintf('FBCK:%s:%s:M%dUSED', fbIOC, loop, i);
  lcaPut(name, 0);
end
lcaPut(['FBCK:' fbIOC ':' loop ':M1DEVNAME'], 'FF:BPMS:IN20:371:X1H');
lcaPut(['FBCK:' fbIOC ':' loop ':M1USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':M1CAMODE'], 1);

%
% states
for i=1:10,
  name = sprintf('FBCK:%s:%s:S%dUSED', fbIOC, loop, i);
  lcaPut(name, 0);
end
lcaPut(['FBCK:' fbIOC ':' loop ':S1NAME'], 'X Position');
lcaPut(['FBCK:' fbIOC ':' loop ':S1USED'], 1);

%
% setpoints

%
% actuators
for i=1:10,
  name = sprintf('FBCK:%s:%s:A%dUSED', fbIOC, loop, i);
  lcaPut(name, 0);
end
lcaPut(['FBCK:' fbIOC ':' loop ':A1DEVNAME'], 'FF:XCOR:IN20:221:BCTRL');
lcaPut(['FBCK:' fbIOC ':' loop ':A1USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':A1CAMODE'], 1);

%
% enable loop
lcaPut(['FBCK:' fbIOC ':' loop ':INSTALLED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':STATE'], 'ON');

end
