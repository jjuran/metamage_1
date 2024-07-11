#!/usr/bin/env jtest

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 g8N v
1 >> '[END]'
r n b q k   N r
v v v v v     v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 gxh8B v
1 >> '[END]'
r n b q k     B
v v v v v     v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 g8R+ v
1 >> '[END]'
r n b q k   R r
v v v v v     v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 gxh8Q+ v
1 >> '[END]'
r n b q k     Q
v v v v v     v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 g8K v
1 >= "promotion to king"

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 g8N h6 Nf6+ v
1 >> '[END]'
r n b q k     r
v v v v v      
          N   v
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 gxh8B h6 Bf6 v
1 >> '[END]'
r n b q k      
v v v v v      
          B   v
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 g8R+ Kf7 Rxd8 v
1 >> '[END]'
r n b R       r
v v v v v k   v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 Nf6 gxh8Q+ Kf7 Qxd8 v
1 >> '[END]'
r n b Q        
v v v v v k   v
          n    
               
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q K B N R
[END]

$ chess-test h4 g5 hxg5 f6 gxf6 Bg7 fxg7 h6 gxh8Q h5 Qxg8# v
1 >> '[END]'
white win
r n b q #   Q  
v v v v v      
               
              v
               
               
^ ^ ^ ^ ^ ^ ^  
R N B Q W B N R
[END]
