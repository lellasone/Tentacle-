# Tentacle-
Firmware and Software to control a single-stage continuum lamp using aruco markers. 

Communications Protocal: 
  Commands must be sent to the tentacle over 2 wire serial using the following datagram format. 
  
  [start][command][arguments 6 bytes][stop]
  
  start, stop, and command bytes can be found at the top of SERIAL. The contents of arguments
  can very from command to command. However, for the "move" command it is apportioned as follows:
  
  [z_cord][z_cord][r_cord][r_cord][t_cord][t_cord]
  
  z_cord - The distance from the tenticle to the target surface, in cm. 
  r_cord - the distance from the tentacle to the target parallel to the surface. 
  t_cord - the angle between tenden A and the target. 0 = 0, 32640 = 2PI 
  Arguments should be sent MSB first. 
