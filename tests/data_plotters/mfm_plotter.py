import pygame
import sys
import time
from math import *

PI_ = 3.1415

data = []
window = 0

def arch_spiral():
	a = 0.0
	b = 0.05
	
	for coreNb in range(0, 64):
		theta = (coreNb / 64.0) * 7.5 * 3.1415

		x = (a+b*theta)*cos(theta)
		y = (a+b*theta)*sin(theta)
		data.append([x,y])
		
		


def homog_disk():
        global data, PI_

	chipsID = [0, 1, 256, 257] # 0,0  0,1  1,0  1,1

	for chipID in chipsID:
		chipX = chipID & 0x000000FF
		chipY = chipID >> 8

		for coreID in range(0, 16):
			coreX = (coreID & 0x00000003) # 0 to 3
			coreY = (coreID >> 2) # 0 to 3

			p1 = float(coreX) / 4.0;
			p2 = float(coreY) / 4.0;

			if (p1 == 0.0):
				p1 = 0.09

			r = 1.15*sqrt(p1 - 0.05)
			theta = ((PI_/2.0) * p2) + ((PI_/2.0)*(chipX + 2.0*chipY)) # + base theta

			x = r*cos(theta)
			y = r*sin(theta)
			data.append([x,y])

        
        

def draw(x, y):
        global window
        pygame.draw.circle(window, (255, 255, 255), (x, y), 1, 1)


def draw_grid():
	global window
	delta = 128.0 / 10.0

	for i in range(0, 10):
		#horizontal
        	pygame.draw.line(window, (173, 255, 47), (0, (i+1)*delta), (128, (i+1)*delta))
        	pygame.draw.line(window, (173, 255, 47), ((i+1)*delta, 0), ((i+1)*delta, 128))
	

def plot_data(data):
        global window

        #create the screen
        window = pygame.display.set_mode((128, 128)) 

	while True:
		window.fill((0,0,0))

		#draw_grid()

		#input handling (somewhat boilerplate code):
		for d in data:
			draw(int(63.5*d[0] + 63.5),  int(63.5*d[1] + 63.5))

		#draw it to the screen
		pygame.display.update() 

		for event in pygame.event.get(): 
			if event.type == pygame.QUIT: 
				pygame.quit()
				sys.exit(0) 


#arch_spiral()
homog_disk()
plot_data(data)
