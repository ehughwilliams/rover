#!/usr/bin/env python3

VERSION="""grrv2_ocu_color.py
use with grrv2_rover_controller_2017-12-12.py
"""

LANC_COMMAND_HELP = """
LANC COMMAND HELP

KEYBOARD COMMANDS
?: this help message
P: PHOTO    //Doesn't work
R, r: REC   //To confirm if RECORDING, select 'M' for Menu. REC/STBY status is displayed
z: ZOOM_OUT_4
Z: ZOOM_IN_4
f: FOCUS_NEAR //WORKS, IN MANUAL FOCUS MODE ONLY (Set Manual/Auto via menu).
F: FOCUS_FAR  //WORKS, IN MANUAL FOCUS MODE ONLY

CAMCORDER MENU COMMANDS
M, m: MENU
<CR>: SELECT / EXECUTE
8: MENU_UP
2: MENU_DOWN
4: MENU_LEFT or PREV;
6: MENU_RIGHT or NEXT;
O: CAMERA POWER ON
o: CAMERA POWER OFF
L: VIDEO LIGHTS ON
l: VIDEO LIGHTS OFF

RAW LANC COMMAND MODE
X, x: rawHexCommandMenu(); Enter header byte, then raw hex number command byte.
	If you don't know what this means, don't mess with it. :-)
"""


import pygame
from pygame import *

import getopt
import math
import os
import string
import sys

from pygame.locals import *
from socket import *
from time import *
from time import sleep

import colorama
from colorama import Fore, Back, Style

colorama.init()

# Joystick Structure info:
# 8 floats. Up to 6 real axes and 2 virtual axes (from buttons). joystickx[0][0] to [0][7]
# 13 ints.  Up to 12 buttons. joystickx[1][0] to [1][12]. [1][0]
# is ignored since joysticks label 12 buttons 1-12, not 0-11
# 1 string. Joystick Name as pulled from Pygame joystickx[2]

jstick = {
	"axes": [0,0,0,0],
	"scale": [125, -125, 125, -125],	# Corrects native joystick orientation, motor polarity, etc
	"buttons": [0,] * 13,  				# 13 bytes for 12 buttons: 0 is ignored since joysticks number 1-12 not 0-11
	"hat": [0,0], 						# joystick hat x and y, -1, 0, 1
}

class UDPSocket:
	"""UDPSocket tracks a remote and simplifies handling multiple targets"""
	def __init__(self, remoteIP, remotePort):
		"""Opens a new socket with a default destination of remoteIP, remotePort

        Args:
            remoteIP (str): Default remote IP.
            remotePort (int): Default remote Port
        """
  
		self.remoteIP = remoteIP
		self.remotePort = remotePort
		self.sockobj = socket(AF_INET, SOCK_DGRAM)
		self.sockobj.settimeout(1.0)
		self.last_message = None

	def sendto(self, message, target = None):
		"""Send a message to target"""
		msg = bytes(message)
		if target is None:
			target = (self.remoteIP, self.remotePort)

		try:
			self.sockobj.sendto(msg, target)
		except Exception as e:
			print(e)
		
		if msg != self.last_message:
			with open("command.log", "wb+") as f:
				f.write(msg)
			self.last_message = msg


	def recvfrom(self, size):
		"""receives a message
		
		Returns: (data, remote_address)
		"""
		data, remote_addr = self.sockobj.recvfrom(size)
		self.last_remote_addr = remote_addr
		return data, remote_addr


def initalize_joysticks():
	"""Walks through the list of joysticks and ensures PYGame has initilized them"""
	grab_input()

	for x in range(joystick.get_count()): 			# inits however many joysticks are attached, usually 1 or 2
		j = joystick.Joystick(x) 					# Logitech Dual Action is 1 joystick w/ 2 sticks with 1 XY axis pair (4 axes).
		j.init()                       				# Most elaborate joysticks are 1 joystick 1 stick with 2 axes (x, y)
		txt = 'Enable Joystick: ' + j.get_name()
		# jstick['name'] = j[x].get_name()
		print(txt)

	release_input()


def setup_window():
	"""sets the x,y position of the window and required video mode flags
	we don't actually need to run the window to run the program
	"""
	os.environ['SDL_VIDEO_WINDOW_POS'] = "%d, %d" % (1600, 40)
	video_flags = OPENGL|DOUBLEBUF     
	display.set_mode((128,96), video_flags)


def main():
	print(VERSION)

	# pygame initialization
	setup_window()
	pygame.init()
	clock, ticks = pygame.time.Clock(), pygame.time.get_ticks()
	initalize_joysticks()

	# a socket to talk to the rover
	rover_socket = UDPSocket('192.168.1.99', 60615)

	print('Starting' + "\n"*4)

	pilt_upcounter = 0
	pilt_downcounter = 255
	loops=1

	motor_enable_flag = 0

	last_keydown = -1
	keyboard_keys = [0,0]

	while True:
		clock.tick(300)  # clock.tick(x) x = Hertz of control loop. 100=100Hz, 20 = 20 Hz Works

		for e in event.get():
			# print e.type,'  ',

			if e.type == pygame.JOYBUTTONDOWN:
				button = e.dict['button'] + 1
				# print("button down")
				jstick['buttons'][0] = button
				if button == 9:
					grab_input()
				if button == 10:
					release_input()
				# print button, ' ',jstick['buttons'][button]

			if e.type == pygame.JOYBUTTONUP:
				# print("button up")
				button = e.dict['button'] + 1
				jstick['buttons'][0] = 0
				# print button, ' ',jstick['buttons'][button]

			if e.type == pygame.KEYDOWN:
				# print(e)
				key, mod, uni = e.dict['key'], e.dict['mod'], e.unicode
				if len(uni) > 0:
					uni = ord(uni)
				else:
					uni = 0
				# key = e.dict['key'] - 32 * e.dict['mod'] # mod = 1 is shift

				# check for control keys
				if mod & pygame.KMOD_CTRL:
					if key == pygame.K_l:		# lock mouse and keyboard to game window
						grab_input()

					if key in (pygame.K_c, pygame.K_d):
						end_program()

				if key == pygame.K_ESCAPE:		# release mouse and keyboard from game window
					release_input()

				if key == pygame.K_SPACE: 		# ALL STOP  SPACE BAR (Big, easy to hit in a panic for faux E-STOP)
					motor_enable_flag = 0

				if key == pygame.K_e and mod & pygame.KMOD_SHIFT: # 'E', ENABLE MOTORS
					motor_enable_flag = 1

				# map arrows, pygame's 273..276 (up, down, right, left) => 128..131
				if key in (pygame.K_LEFT, pygame.K_RIGHT, pygame.K_UP, pygame.K_DOWN):
					map = {pygame.K_LEFT:131, pygame.K_RIGHT:130, pygame.K_UP:128, pygame.K_DOWN:129}
					keyboard_keys[0] = map[key]

				if key == 47:
					print(LANC_COMMAND_HELP)

				# Only send non-continuous commands once
				continuous_key_commands = ('f','F','z','Z')
				if uni not in continuous_key_commands and uni == last_keydown:
					uni = -1
					keyboard_keys[0] = 0

				# The above, including local commands & esc, space, and E all fall through to here and will be sent to the remote. Intentional?
				if uni >= 0 and uni < 256:
					keyboard_keys[0] = uni

			if e.type == pygame.KEYUP:
				key, mod = e.dict['key'], e.dict['mod']

				if key >= 0 and key < 256:
					keyboard_keys[0] = 0

				if key in (pygame.K_LEFT, pygame.K_RIGHT, pygame.K_UP, pygame.K_DOWN):
					keyboard_keys[0] = 0
				
				last_keydown = -1

			if e.type == pygame.JOYAXISMOTION:
				axis = e.dict['axis']
				# print("joy motion")
				jstick['axes'][axis] = int(e.dict['value'] * jstick['scale'][axis]) # -1.0 to 0 to 1.0 * +/-125

			if e.type == pygame.JOYHATMOTION: # HAT MOTION Use hat +/-y for LANC zoom? And +/-x for focus?
				# print("joy hat motion")
				jstick['hat'] = e.dict['value'] # 0,1 are x,y
				# print("\njstick hat: ", jstick['hat'])
				# since all LANC commands must be sent down the serial LANC wire
				# it's best to treat the Hat as keystrokes so they never compete
				# and the keystroke SwitchCase can prioritize them

				focus_commands = { 1: ord('F'), -1: ord('f') }
				zoom_commands =  { 1: ord('Z'), -1: ord('z') }

				# look it up in the dictionary. If it's not there, use '0'
				key = focus_commands.get(jstick['hat'][0], 0)
				# look up zoom state, if any, and allow that to override focus
				key = zoom_commands.get(jstick['hat'][1], key)
				# print('jstick hat to key', key)
				keyboard_keys[0] = key

			# if e.type == pygame.JOYBALLMOTION:
			# 	pass
			# if e.type in (pygame.MOUSEBUTTONDOWN, pygame.MOUSEBUTTONUP, pygame.MOUSEMOTION):
			# 	pass


		# send the joystick and keyboard commands to the rover
		motions = stick_to_motions(jstick['axes'])
		# print(motions, "\n")
		msg = build_rover_command_byte_packet(motions, joystick, keyboard_keys, motor_enable_flag)
		print('cmd_chr', *msg, end = ' ')
		rover_socket.sendto(msg)

		# RECEIVE DATA FROM ROBOT
		try:
			rover_packet, remote_address = rover_socket.recvfrom(1024)
			rover_status, pilt_status, rover_message = unpack_rover_message(rover_packet)

			pilt_upcounter = (pilt_upcounter + 1) % 255
			pilt_downcounter = 255 - pilt_upcounter

			print_rover_pilt_status(rover_status, pilt_status, rover_message, pilt_upcounter, pilt_downcounter, loops)
		except timeout:
			print('NO ROVER STATUS PACKET RECEIVED.  Last known ', end=' ')

		print("                \r", end=' ')
		sys.stdout.flush()

		loops = loops + 1
		if loops == 101:
			loops = 1

def pilt_indicator_hue(x):
	"""return a color format for printing based on the range of the value"""

	# Well within the limits of travel of pan and tilt
	if 96 <= x <= 160:
		return Style.BRIGHT + Fore.GREEN 

	# Approaching the limits of travel of pan and tilt
	if 64 <= x <= 192:
		return Style.BRIGHT + Fore.YELLOW

	# AT OR EXCEEDING the limits of travel of pan and tilt
	return Style.BRIGHT + Fore.RED
	
def pilt_indicator(x):
	"""Convert a PILT reading to a colorized string"""
	return pilt_indicator_hue(x) + str(x) + Style.RESET_ALL

def print_rover_pilt_status(rover_status, pilt_status, rover_message, pilt_upcounter, pilt_downcounter, loops):
	"""Prints one line corresponding to the rover status. Multiple calls will overwrite previous lines."""
	print(' rover_status', *rover_status, end=' ')

	print('   pilt_status', end=' ')

	for i in range(min(14, len(pilt_status))):
		if i==2:
			print(pilt_indicator(pilt_upcounter), end = ' ')
		elif i==5:
			print(pilt_indicator(pilt_downcounter), end = ' ')
		else:
			print(pilt_status[i], end=' ')

	print('  %s   %d            \r' % (rover_message, loops), end=' ')

	sys.stdout.flush()

def build_rover_command_byte_packet(motions, joystick, keyboard_keys, motor_enable_flag):
	"""Builds a rover command message from parameters"""

	def wire_signed(x):
		"""Converts (-1,1) to (0,1) for PILT"""
		if x == -1:
			return 0
		return 1

	msg = [
		motions['pwm_left'],						# Left Stick (X-Y mixed): Left PWM abs value
		wire_signed(motions['sign_left']),			# Left Stick (X-Y mixed): Left PWM boolean sign map(1=1, -1=0)
		motions['pwm_right'],						# Left Stick (X-Y mixed): Right PWM abs value
		wire_signed(motions['sign_right']),			# Left Stick (X-Y mixed): Right PWM boolean sign map(1=1, -1=0)
		abs(motions['stick_pan']),					# Right Stick,PWM, Pan
		wire_signed(sign(motions['stick_pan'])),	# Right Stick,DIR, Pan
		abs(motions['stick_tilt']),					# Right Stick,PWM, Tilt
		wire_signed(sign(motions['stick_tilt'])),	# Right Stick,DIR, Pan
		jstick['buttons'][0],
		keyboard_keys[0], 				# commands for lanc, lights, whatever to Rover
		jstick['hat'][0] + 1,	 		# JS Hat values are -1, 0, +1.
		jstick['hat'][1] + 1, 			# be unsigned: like joystick axes, it must be converted
		motor_enable_flag,
		0, 								# Spare
		0, 								# Spare
		0, 								# Spare
	] + [45]
	# [45] is an end of message marker

	assert len(msg) == 17
	return msg

def unpack_rover_message(rover_packet):
	"""break apart the received packet and convert from array of bytes to native python types
	Returns:
		rover_status (:obj:`list` of :obj:`int`))
		pilt_status  (:obj:`list` of :obj:`int`))
		rover_message (str)
	"""
	
	rover_status  = [x for x in rover_packet[0:16]]	# 16 bytes rover
	pilt_status   = [x for x in rover_packet[16:30]]	# 14 bytes Arduino status
	rover_message = rover_packet[32:48]						# Arduino puts.
	if not isinstance(rover_message, str):
		# In Python 3 strings are no longer equivalent to byte arrays, so go through a decode step
		rover_message = rover_message.decode('ascii')
	return rover_status, pilt_status, rover_message


def stick_to_motions(joystick):
	"""Translates joystick to PWMs, signs, octant, pan, and tilt"""
	pwm_left, pwm_right, sign_left, sign_right, octant = map_x_y_to_left_right_pwms(jstick['axes'])
	stick_pan = jstick['axes'][2]
	stick_tilt = jstick['axes'][3]
	motions = {
		"pwm_left": pwm_left,
		"pwm_right": pwm_right,
		"sign_left": sign_left,
		"sign_right": sign_right,
		"stick_pan": stick_pan,
		"stick_tilt": stick_tilt,
		"octant": octant,
	}
	return motions


def get_octant(joystick):
	"""get_octant() handles left gamepad joystick mixing for the tank skid-steering
	As normal cartesian grid has 4 quadrants where X and Y are + /-, we need 8 divisions
	So 'octants'. Based on 3 conditions:
		abs_x => abs_y; sgn_x >0; sgn_y >0. from these are derived the left and right
		'tank track' pwm mixing for single stick skid steer used in map_x_y_to_left_right()
	"""

	stick_x = jstick['axes'][0]
	stick_y = jstick['axes'][1]

	octant = 0
	if abs(stick_y) >= abs(stick_x):
		octant += 4

	if stick_y < 0:
		octant += 2

	if stick_x < 0:
		octant += 1

	return octant


def map_x_y_to_left_right_pwms(joystick):
	"""basically, left joystick positions within XY octants defined as:
	stick fwd, left right tracks fwd: back = bkwd
	stick 90 left or right = pivot lft or rt (one track fwd opposite track rev)
	stick 45 deg = soft turn; one track moving, other track stopped.
	intermediate angles smooth continuum of translate rotated between the above
	"""

	octant = get_octant(jstick['axes'])

	stick_x, stick_y = jstick['axes'][0], jstick['axes'][1]
	abs_x, abs_y     = abs(stick_x), abs(stick_y)
	sign_x, sign_y   = sign(stick_x), sign(stick_y)

	if octant in (0, 3):
		pwm_left = abs_y
		pwm_right = abs_y - abs_x
		sign_left = sign_y
		sign_right = sign_y

	if octant in (1, 2):
		pwm_left = abs_y - abs_x
		pwm_right = abs_y
		sign_left = sign_y
		sign_right = sign_y

	if octant in (4, 7):
		pwm_left = abs_x
		pwm_right = abs_x - abs_y
		sign_left = sign_x
		sign_right = -sign_x

	if octant in (5, 6):
		pwm_left = abs_x - abs_y
		pwm_right = abs_x
		sign_left = sign_x
		sign_right = -sign_x

	dead_boundary = 12

	if pwm_left < dead_boundary:
		pwm_left = 0
	if pwm_right < dead_boundary:
		pwm_right = 0

	return (pwm_left, pwm_right, sign_left, sign_right, octant)


def sign(number):
	"""Returns 1 if the number >= 0, otherwise -1"""
	if number >= 0:
		return 1
	return -1

def grab_input():
	"""Lock mouse and keyboard to window"""
	event.set_grab(True)

def release_input():
	"""Release mouse and keyboard from being locked to window"""
	event.set_grab(False)

def end_program():
	release_input()
	print("ALL STOP")
	print("EXITING OCU PROGRAM")
	sys.exit(0) # exits program

if __name__ == '__main__':
	main()

