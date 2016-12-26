from math import sin, cos, radians, pi
import os, sys, time, errno, pygame, random, platform
from numpy import array, arange, zeros, roll

pygame.init()
info            = pygame.display.Info()
clock           = pygame.time.Clock()

fps             = 60

clr_black       = (0, 0, 0)
clr_red         = (255, 0, 0)
clr_green       = (0, 255, 0)
clr_blue        = (0, 0, 255)
clr_white       = (255, 255, 255)
clr_yellow      = (255, 255, 0)
clr_purple      = (255, 0, 255)
clr_cyan        = (0, 255, 255)
clr_lightgray   = (250, 250, 250)

screenWidth     = info.current_w
screenHeight    = info.current_h
screenRes       = (screenWidth, screenHeight)

avail_x         = range(10)
avail_y         = range(-1, 4)
avail_force     = range(0, 1000)
avail_angle     = range(-10, 31)
force           =[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]


screen          = pygame.display.set_mode(screenRes, pygame.FULLSCREEN)
# screen          = pygame.display.set_mode((400, 300))
screen.fill(clr_black)
pygame.display.update()


if __name__ == "__main__":
    run     = True
    pause   = False

    render_event    = pygame.USEREVENT + 1
    pygame.time.set_timer(render_event, 10)

    animation_top   = screenHeight / 3
    layer_width     = 2
    layer_height    = screenHeight / 2

    first_posX      = (screenWidth * 1 / 14)
    second_posX     = first_posX+(screenWidth*3*14/800) 
    third_posX      = second_posX +(screenWidth*3*20/800) 
    fourth_posX     = third_posX +(screenWidth*3*5/800)
    fifth_posX		= fourth_posX +(screenWidth*3*11/800)
    sixth_posX		= fifth_posX +(screenWidth*3*25/800)
    seventh_posX    = sixth_posX +(screenWidth*3*16/800)
    eighth_posX      = seventh_posX +(screenWidth*3*9/800)
	
    a_posX	=(screenWidth*8/14)
    b_posX  =(screenWidth*12/14)

    needle_width    = 80
    needle_height   = 20

    while run :
        clock.tick(fps)

        for event in pygame.event.get():
            if event.type == pygame.QUIT :
                run = not run
            elif event.type == pygame.KEYDOWN :
                if event.key == pygame.K_ESCAPE : run = not run
                if event.key == pygame.K_SPACE  : pause = not pause
            elif event.type == render_event :
                x       = random.choice(avail_x)
                y       = random.choice(avail_y)
                angle   = random.choice(avail_angle)
                force[20]= random.choice(avail_force)

                step_X  = (eighth_posX - first_posX) / (len(avail_x) - 1)
                step_Y  = layer_height / (len(avail_y) - 1)
                rads    = radians(-angle)

                start_pos   = [first_posX + (x * step_X) - (needle_width * cos(rads)), ((y - avail_y[0]) * step_Y) + animation_top - (needle_width * sin(rads))]
                end_pos     = [first_posX + (x * step_X), ((y - avail_y[0]) * step_Y) + animation_top]

                screen.fill(clr_black)
                pygame.draw.line(screen, clr_lightgray, start_pos, end_pos, needle_height)
			
                for n in range(1,21):
					pygame.draw.line(screen, (255, 255, 255), [a_posX+(b_posX-a_posX)*(n-1)*0.05 , animation_top+(force[n-1]*layer_height/1500)], [a_posX+(b_posX-a_posX)*n*0.05 , animation_top+(force[n]*layer_height/1500)], 2)
                
                force[0]=force[1]
                force[1]=force[2]
                force[2]=force[3]
                force[3]=force[4]
                force[4]=force[5]
                force[5]=force[6]
                force[6]=force[7]
                force[7]=force[8]
                force[8]=force[9]
                force[9]=force[10]
                force[10]=force[11]
                force[11]=force[12]
                force[12]=force[13]
                force[13]=force[14]
                force[14]=force[15]
                force[15]=force[16]
                force[16]=force[17]
                force[17]=force[18]
                force[18]=force[19]
                force[19]=force[20]

				

				
                
        # lapisan 1
        pygame.draw.line(screen, clr_green, [second_posX, animation_top], [second_posX, animation_top + layer_height], layer_width)
        # lapisan 2
        pygame.draw.line(screen, clr_blue, [third_posX, animation_top], [third_posX, animation_top + layer_height], layer_width)
        # lapisan 3
        pygame.draw.line(screen, clr_purple, [fourth_posX, animation_top], [fourth_posX, animation_top + layer_height], layer_width)
		# lapisan 4
        pygame.draw.line(screen, clr_yellow, [fifth_posX, animation_top], [fifth_posX, animation_top + layer_height], layer_width)
		# lapisan 5
        pygame.draw.line(screen, clr_red, [sixth_posX, animation_top], [sixth_posX, animation_top + layer_height], layer_width)
		# lapisan 6
        pygame.draw.line(screen, clr_cyan, [seventh_posX, animation_top], [seventh_posX, animation_top + layer_height], layer_width)
		# lapisan 7
        pygame.draw.line(screen, clr_white, [eighth_posX, animation_top], [eighth_posX, animation_top + layer_height], layer_width)
		
        for i in range(11):
            pygame.draw.line(screen, (210, 210, 210), [a_posX , animation_top+layer_height*i*0.1], [b_posX , animation_top+layer_height*i*0.1], 1)
            pygame.draw.line(screen, (210, 210, 210), [a_posX+(b_posX-a_posX)*i*0.1 , animation_top], [a_posX + (b_posX-a_posX)*i*0.1 , animation_top+layer_height], 1)

        pygame.display.flip()

    pygame.quit()	