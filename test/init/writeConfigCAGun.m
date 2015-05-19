% Configuration for GunLaunch
% 1 BPM (X and Y)

function writeConfigCAGun(fbIOC, loop)
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
%lcaPut(['FBCK:' fbIOC ':' loop ':ITERATEFUNC'], 'SineN');
lcaPut(['FBCK:' fbIOC ':' loop ':ITERATEFUNC'], 'TrajectoryFit');

%
% measurements
for i=1:40,
  name = sprintf('FBCK:%s:%s:M%dUSED', fbIOC, loop, i);
  lcaPut(name, 0);
end
lcaPut(['FBCK:' fbIOC ':' loop ':M1DEVNAME'], 'FF:BPMS:IN20:371:X');
lcaPut(['FBCK:' fbIOC ':' loop ':M1USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':M1CAMODE'], 1);
lcaPut('FF:BPMS:IN20:371:X', 1);
lcaPut('FF:BPMS:IN20:371:X.EGU', 'mm');

lcaPut(['FBCK:' fbIOC ':' loop ':M2DEVNAME'], 'FF:BPMS:IN20:425:X');
lcaPut(['FBCK:' fbIOC ':' loop ':M2USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':M2CAMODE'], 1);
lcaPut('FF:BPMS:IN20:425:X', 3);
lcaPut('FF:BPMS:IN20:425:X.EGU', 'mm');

lcaPut(['FBCK:' fbIOC ':' loop ':M3DEVNAME'], 'FF:BPMS:IN20:371:Y');
lcaPut(['FBCK:' fbIOC ':' loop ':M3USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':M3CAMODE'], 1);
lcaPut('FF:BPMS:IN20:371:Y', 2);
lcaPut('FF:BPMS:IN20:371:Y.EGU', 'mm');

lcaPut(['FBCK:' fbIOC ':' loop ':M4DEVNAME'], 'FF:BPMS:IN20:425:Y');
lcaPut(['FBCK:' fbIOC ':' loop ':M4USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':M4CAMODE'], 1);
lcaPut('FF:BPMS:IN20:425:Y', 4);
lcaPut('FF:BPMS:IN20:425:Y.EGU', 'mm');

%
% states
for i=1:10,
  name = sprintf('FBCK:%s:%s:S%dUSED', fbIOC, loop, i);
  lcaPut(name, 0);
end
lcaPut(['FBCK:' fbIOC ':' loop ':S1NAME'], 'X Position');
lcaPut(['FBCK:' fbIOC ':' loop ':S1USED'], 1);

lcaPut(['FBCK:' fbIOC ':' loop ':S2NAME'], 'X Angle');
lcaPut(['FBCK:' fbIOC ':' loop ':S2USED'], 1);

lcaPut(['FBCK:' fbIOC ':' loop ':S3NAME'], 'Y Position');
lcaPut(['FBCK:' fbIOC ':' loop ':S3USED'], 1);

lcaPut(['FBCK:' fbIOC ':' loop ':S4NAME'], 'Y Angle');
lcaPut(['FBCK:' fbIOC ':' loop ':S4USED'], 1);

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

lcaPut(['FBCK:' fbIOC ':' loop ':A2DEVNAME'], 'FF:YCOR:IN20:222:BCTRL');
lcaPut(['FBCK:' fbIOC ':' loop ':A2USED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':A2CAMODE'], 1);

lcaPut(['FBCK:' fbIOC ':' loop ':A3DEVNAME'], 'FF:XCOR:IN20:381:BCTRL');
lcaPut(['FBCK:' fbIOC ':' loop ':A3USED'], 0);
lcaPut(['FBCK:' fbIOC ':' loop ':A3CAMODE'], 1);

lcaPut(['FBCK:' fbIOC ':' loop ':A4DEVNAME'], 'FF:YCOR:IN20:382:BCTRL');
lcaPut(['FBCK:' fbIOC ':' loop ':A4USED'], 0);
lcaPut(['FBCK:' fbIOC ':' loop ':A4CAMODE'], 1);

%
% enable loop
lcaPut(['FBCK:' fbIOC ':' loop ':INSTALLED'], 1);
lcaPut(['FBCK:' fbIOC ':' loop ':STATE'], 'ON');

end
