#!/usr/bin/env jtest

$ dec2str 'N(000)'
1 >= 'NAN(000)'

$ dec2str 'N(001)'
1 >= 'NAN(001)'

$ dec2str 'N(00F)'
1 >= 'NAN(015)'

$ dec2str 'N(FFF)'
1 >= 'NAN(255)'

$ dec2str 'NxABCD'
1 >= 'NAN(188)'
