import pygame
import sys
import time

raw_data = []
clean_data = []
window = 0
base = 77

def import_file():
        global raw_data

        file_str = open("./data", "r")

        for line in file_str:
                data_temp = line.split(',')
                raw_data.append([float(data_temp[0]), float(data_temp[1]), float(data_temp[2])])

        for d in raw_data:
                d[2] = d[2] - base


def clean():
        global raw_data, clean_data

        tmp_buffer_x = [0 for i in range(0, 16)]
        tmp_buffer_y = [0 for i in range(0, 16)]
        curr_index = 0
        total_sum_x = 0
        total_sum_y = 0

        for d in raw_data:
                total_sum_x = total_sum_x - tmp_buffer_x[curr_index] + d[0]
                tmp_buffer_x[curr_index] = d[0]
                total_sum_y = total_sum_y - tmp_buffer_y[curr_index] + d[1]
                tmp_buffer_y[curr_index] = d[1]

                clean_data.append([float(int(total_sum_x) >> 4), float(int(total_sum_y) >> 4), d[2]])


                curr_index = (curr_index + 1) % 16
        
        

def draw(x, y):
        global window
        pygame.draw.circle(window, (255, 255, 255), (x, y), 1, 1)


def plot_data(data):
        global window
        start_time = int(time.time())

        #create the screen
        window = pygame.display.set_mode((128, 128)) 

        window.fill((0,0,0))

        #input handling (somewhat boilerplate code):
        while True: 

           
           for d in data:
                if(d[2] == int(time.time()) - start_time):
                        #draw a circle 
                        draw(int(d[0]),int(d[1]))

           #draw it to the screen
           pygame.display.update() 

           for event in pygame.event.get(): 
              if event.type == pygame.QUIT: 
                  pygame.quit()
                  sys.exit(0) 


import_file()
clean()
plot_data(clean_data)
