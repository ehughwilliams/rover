"""
"""
import pygame
from pygame import*
import sys
import string
import math
import os
import getopt
from time import *
from pygame.locals import *
from socket import *
from time import sleep

##########  TRACTOR SETUP  ###############
command_packet_len_tractor = 16
command_byte_list_tractor = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
command_char_list_tractor = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
#tractor_status_buffer = ''
command_byte_string_tractor = ""

#test to see if making these global helps (if this will make these global)
tractor_packet = ''
tractor_status = ''
tractor_message = ''
tractor_exception_message = ''
tractor_packet = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

var_0_tractor = 10
var_1_tractor = 11
var_2_tractor = 12
var_3_tractor = 13
var_4_tractor = 14
var_5_tractor = 15
var_6_tractor = 16
var_7_tractor = 17
var_8_tractor = 18
var_9_tractor = 19
var_10_tractor = 20
var_11_tractor = 21
var_12_tractor = 22
var_13_tractor = 23
var_14_tractor = 24
var_15_tractor = 25


############ pilt SETUP ###########################
command_packet_len_pilt = 16
command_byte_list_pilt = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
command_char_list_pilt = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
#pilt_status_buffer = ''
command_byte_string_pilt = ""

#test to see if making these global helps (if this will make these global)
pilt_packet = ''
pilt_status = ''
pilt_message = ''
pilt_exception_message = ''
pilt_packet = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]


var_0_pilt = 10
var_1_pilt = 11
var_2_pilt = 12
var_3_pilt = 13
var_4_pilt = 14
var_5_pilt = 15
var_6_pilt = 16
var_7_pilt = 17
var_8_pilt = 18
var_9_pilt = 19
var_10_pilt = 20
var_11_pilt = 21
var_12_pilt = 22
var_13_pilt = 23
var_14_pilt = 24
var_15_pilt = 25


############ buoy SETUP ###########################
command_packet_len_buoy = 16
command_byte_list_buoy = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
command_char_list_buoy = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
#buoy_status_buffer = ''
command_byte_string_buoy = ""

#test to see if making these global helps (if this will make these global)

buoy_packet = ''
buoy_status = ''
buoy_message = ''
buoy_exception_message = ''
buoy_packet = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

var_0_buoy = 10
var_1_buoy = 11
var_2_buoy = 12
var_3_buoy = 13
var_4_buoy = 14
var_5_buoy = 15
var_6_buoy = 16
var_7_buoy = 17
var_8_buoy = 18
var_9_buoy = 19
var_10_buoy = 20
var_11_buoy = 21
var_12_buoy = 22
var_13_buoy = 23
var_14_buoy = 24
var_15_buoy = 25

loops = 0


def main():
	loops=1
	

	#Initialize Sockets BEGIN
	
	#Initialize Sockets Tractor
	serverHostTractor = '192.168.1.150'
	serverPortTractor = 8880
	sockobjTractor = socket(AF_INET, SOCK_DGRAM)
	sockobjTractor.settimeout(1.0)

	#Initialize Sockets Pilt
	serverHostPilt = '192.168.1.151'
	serverPortPilt = 8881
	sockobjPilt = socket(AF_INET, SOCK_DGRAM)
	sockobjPilt.settimeout(1.0)
	
	#Initialize Sockets Buoy
	serverHostBuoy = '192.168.1.152'
	serverPortBuoy = 8882
	sockobjBuoy = socket(AF_INET, SOCK_DGRAM)
	sockobjBuoy.settimeout(1.0)
	print 'UDP Socket Init End'
	#Initialize Sockets END
	

########## PYGAME WINDOW SET UP ###############
#   Set the Position of the Window in the SDL Environment, not in Pygame directly
	window_x = 1600     #Set the X Position of the Window
	window_y = 40      #Set the Y Position of the Window
	os.environ['SDL_VIDEO_WINDOW_POS'] = "%d, %d" % (window_x, window_y) #
	video_flags = OPENGL|DOUBLEBUF     #Cool that we don't actually need to run the window to run the program
	display.set_mode((128,96), video_flags)
########## PYGAME WINDOW SET UP END  ###############

	init()
	clock = pygame.time.Clock()
	ticks = pygame.time.get_ticks()
	event.set_grab(1)

	print 'Begin Main'

	while 1:
		buoy_packet = ''
		buoy_status = ''
		buoy_message = ''
		loops=loops+1
		if loops>100:
			loops=1

		clock.tick(100)  #clock.tick(x) x = Hertz of control loop. 100=100Hz, 20 = 20 Hz Works
		for e in event.get():
			if e.type == 2: #2 = KEYDOWN
				key_type = e.type
				key = (e.dict['key']) - (32 * e.dict['mod']) #mod = 1 is shift
				if key == -1940: #Ctrl l (small L)
					ctrl_l() #lock mouse and keyboard to game window
				if key == 27: #ESC
					esc() #release mouse and keyboard from game window
				if key == -1949: #Ctrl C
					end_program()
				if key == -1948: #Ctrl D
					end_program()
		
		
		#event.pump()

		
		####################TRACTOR DATA TRANSFER  ####################################
		###### BUILD COMMAND CHAR PACKET TRACTOR #########
		build_command_byte_packet_tractor()
		command_byte_list_tractor[15]= loops # build_command_byte_packet() doesn't pick up global 'loops'
		build_command_char_packet_tractor()
		
		print 'tractor_cmd_chr',
		for x in range(command_packet_len_tractor):
			print ord(command_char_list_tractor[x]),
		command_char_string_tractor = map_command_char_string_tractor()
	
		tractor_packet = ''
		sockobjTractor.sendto(command_char_string_tractor,(serverHostTractor,serverPortTractor))
		try:
			tractor_packet, server_address = sockobjTractor.recvfrom(1024)
		except timeout:
			print 'NO buoy STATUS PACKET RECEIVED.  Last known ',
		tractor_status = tractor_packet[0:16] #16 bytes rover NOTE horrible format - 0:16 is NOT INCLUSIVE of last byte!
		print ' tractor_status',
		for c in tractor_status: print ord(c), #FINALLY!!! This WORKS for UPD-received packet printing in ints!!!
		print ' from IP ',
		print server_address[0],  # we can get the address and port of the server and see whether its from 
		print ' Port ',           #  tractor, pilt, or buoy then do if-then to send tractor data to tractor buffer, etc
		print server_address[1],  #
		if server_address[0] == serverHostTractor: # this will let us have just one receiving packet and we can 
			print '   IP MATCH',        # see which server it came from and route it to the right buffer.
		
		print '  ',
		
		sys.stdout.flush()

		###################PILT DATA TRANSFER  #########################################
		### BUILD COMMAND CHAR PACKET PILT ###
		build_command_byte_packet_pilt()
		command_byte_list_pilt[15]= loops # build_command_byte_packet() doesn't pick up global 'loops'
		build_command_char_packet_pilt()
		
		print 'pilt_cmd_chr',
		for x in range(command_packet_len_pilt):
			print ord(command_char_list_pilt[x]),
		command_char_string_pilt = map_command_char_string_pilt()
	
		pilt_packet = ''
		sockobjPilt.sendto(command_char_string_pilt,(serverHostPilt,serverPortPilt))
		try:
			pilt_packet, server_address = sockobjPilt.recvfrom(1024)
		except timeout:
			print 'NO pilt STATUS PACKET RECEIVED.  Last known ',
		pilt_status = pilt_packet[0:16] #16 bytes rover NOTE horrible format - 0:16 is NOT INCLUSIVE of last byte!
		print ' pilt_status',
		for c in pilt_status: print ord(c), #FINALLY!!! This WORKS for UPD-received packet printing in ints!!!
		print ' from IP ',
		print server_address[0],  # we can get the address and port of the server and see whether its from 
		print ' Port ',           #  tractor, pilt, or buoy then do if-then to send tractor data to tractor buffer, etc
		print server_address[1],  #
		if server_address[0] == serverHostPilt: # this will let us have just one receiving packet and we can 
			print '   IP MATCH',        # see which server it came from and route it to the right buffer.
		print '  ',
		sys.stdout.flush()

		#################BUOY  DATA TRANSFER #############################
		#### BUILD COMMAND CHAR PACKET BUOY#####
		if loops == 100:
			print
			print
			print
			
			build_command_byte_packet_buoy()
			command_byte_list_buoy[15]= loops # build_command_byte_packet() doesn't pick up global 'loops'
			build_command_char_packet_buoy()
			
			print 'buoy_cmd_chr',
			for x in range(command_packet_len_buoy):
				print ord(command_char_list_buoy[x]),
			
			
			command_char_string_buoy = map_command_char_string_buoy()

			buoy_packet = ''
			sockobjBuoy.sendto(command_char_string_buoy,(serverHostBuoy,serverPortBuoy))
			try:
				buoy_packet, server_address = sockobjBuoy.recvfrom(1024)
			except timeout:
				print 'NO buoy STATUS PACKET RECEIVED.  Last known ',
			buoy_status = buoy_packet[0:16] #16 bytes rover NOTE horrible format - 0:16 is NOT INCLUSIVE of last byte!
			#buoy_message = buoy_packet[16:30] # 14 bytes Arduino status Starts with 16 when the above ENDS with 16
			print ' buoy_status',
			for c in buoy_status: print ord(c), #FINALLY!!! This WORKS for UPD-received packet printing in ints!!!
			print ' from IP ',
			print server_address[0],  # we can get the address and port of the server and see whether its from 
			print ' Port ',           #  tractor, pilt, or buoy then do if-then to send tractor data to tractor buffer, etc
			print server_address[1],  #
			if server_address[0] == serverHostBuoy: # this will let us have just one receiving packet and we can 
				print '   IP MATCH',        # see which server it came from and route it to the right buffer.
			print '  ',
			#print loops,  # \r = Carriage Return w/out NewLine, so text prints over itself on one line
			print
			print
			print
		sys.stdout.flush()
		#print "                      ", '\r', #Clear previous text extending beyond current. Kludge until tabs cleanup
		print loops  # \r = Carriage Return w/out NewLine, so text prints over itself on one line
		

################  TRACTOR DEFS ##################		
def build_command_byte_packet_tractor():
	########BUILD COMMAND BYTE PACKET #################################################
	#byte values for tractor controller
	for i in range(16):
		command_byte_list_tractor[i]= i
			
def build_command_char_packet_tractor():
	############## BUILD COMMAND CHAR PACKET ###################################
	#because Sockobj.sendto() must have unsigned chars (no negatives)
	#so must cast all the bytes as chr
	for i in range(16):
		command_char_list_tractor[i]=chr(command_byte_list_buoy[i]) 
		
def map_command_char_string_tractor():
	#build the string which socko.sendto() needs as its arg (basically just adds End of Packet chr(45))
	command_char_string_tractor = (       # How to do this a smarter more Python idiomatic way?
		command_char_list_tractor[0] +
		command_char_list_tractor[1] +
		command_char_list_tractor[2] +
		command_char_list_tractor[3] +
		command_char_list_tractor[4] +
		command_char_list_tractor[5] +
		command_char_list_tractor[6] +
		command_char_list_tractor[7] +
		command_char_list_tractor[8] +
		command_char_list_tractor[9] +
		command_char_list_tractor[10] +
		command_char_list_tractor[11] +
		command_char_list_tractor[12] +
		command_char_list_tractor[13] +
		command_char_list_tractor[14] +
		command_char_list_tractor[15] +
		chr(45)    #End of Packet string
		)
	#print 'cbs in map = ',command_byte_string_tractor, '  ',
	return command_char_string_tractor

################  PILT DEFS ##################	
def build_command_byte_packet_pilt():
	########BUILD COMMAND BYTE PACKET #################################################
	#byte values for pilt controller
	for i in range(16):
		command_byte_list_pilt[i]= i

def build_command_char_packet_pilt():
	############## BUILD COMMAND CHAR PACKET ###################################
	#because Sockobj.sendto() must have unsigned chars (no negatives)
	#so must cast all the bytes as chr
	for i in range(16):
		command_char_list_pilt[i]=chr(command_byte_list_pilt[i]) 

def map_command_char_string_pilt():
	#build the string which socko.sendto() needs as its arg (basically just adds End of Packet chr(45))
	command_char_string_pilt = (       # How to do this a smarter more Python idiomatic way?
		command_char_list_pilt[0] +
		command_char_list_pilt[1] +
		command_char_list_pilt[2] +
		command_char_list_pilt[3] +
		command_char_list_pilt[4] +
		command_char_list_pilt[5] +
		command_char_list_pilt[6] +
		command_char_list_pilt[7] +
		command_char_list_pilt[8] +
		command_char_list_pilt[9] +
		command_char_list_pilt[10] +
		command_char_list_pilt[11] +
		command_char_list_pilt[12] +
		command_char_list_pilt[13] +
		command_char_list_pilt[14] +
		command_char_list_pilt[15] +
		chr(45)    #End of Packet string
		)
	#print 'cbs in map = ',command_byte_string_pilt, '  ',
	return command_char_string_pilt

################  BUOY DEFS ##################	
def build_command_byte_packet_buoy():
	########BUILD COMMAND BYTE PACKET #################################################
	#byte values for pilt controller
	for i in range(16):
		command_byte_list_buoy[i]= i


def build_command_char_packet_buoy():
	############## BUILD COMMAND CHAR PACKET ###################################
	#because Sockobj.sendto() must have unsigned chars (no negatives)
	#so must cast all the bytes as chr
	for i in range(16):
		command_char_list_buoy[i]=chr(command_byte_list_buoy[i]) 

	
def map_command_char_string_buoy():
	#build the string which sockobj.sendto() needs as its arg (basically just adds End of Packet chr(45))
	command_char_string_buoy = (       # How to do this a smarter more Python idiomatic way?
		command_char_list_buoy[0] +
		command_char_list_buoy[1] +
		command_char_list_buoy[2] +
		command_char_list_buoy[3] +
		command_char_list_buoy[4] +
		command_char_list_buoy[5] +
		command_char_list_buoy[6] +
		command_char_list_buoy[7] +
		command_char_list_buoy[8] +
		command_char_list_buoy[9] +
		command_char_list_buoy[10] +
		command_char_list_buoy[11] +
		command_char_list_buoy[12] +
		command_char_list_buoy[13] +
		command_char_list_buoy[14] +
		command_char_list_buoy[15] +
		chr(45)    #End of Packet string
		)
	#print 'cbs in map = ',command_byte_string_buoy, '  ',
	return command_char_string_buoy
	
	
def ctrl_l():
	event.set_grab(1)#lock mouse and keyboard to game window
def esc():
	event.set_grab(0)#release mouse and keyboard from game window
def end_program():
		event.set_grab(0)
		print "ALL STOP"
		print "EXITING OCU PROGRAM"
		sys.exit(0) # exits program 

if __name__ == '__main__': main()

