#!/usr/bin/python3

import cv2

from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

from PIL import Image

from objloader import *

class Scene:
    def __init__(self, obj3d_filename):

        # Carrega o modelo 3D do Pikachu
        self.obj3d = OBJ(obj3d_filename, swapyz=True)

        self.angulo_animacao = 0

    def display(self):

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        # habilita o uso de texturas (o Pikachu tem textura)
        glEnable(GL_TEXTURE_2D)

        # translada o objeto para ficar 10 unidades distante da camera (para podermos ver o objeto)
        glTranslate(0,0,-10)

        # Move o model em y para centralizar ele
        glTranslate(0,-2,0)

        # Rotaciona o modelo para podermos ve-lo de frente
        glRotate(90,1,0,0)
        glRotate(180,0,1,0)

        # Rotaciona o objecto em torno do eixo Z para criar uma animacao
        glRotate(self.angulo_animacao, 0, 0, 1)
        self.angulo_animacao += 0.5 

        # Renderiza o modelo 3D
        glCallList(self.obj3d.gl_list)

        # renderiza um cubo
        # glutWireCube(2.0)

        glutSwapBuffers()    

def camera_config(width, height):

    glClearColor(0.2, 0.2, 0.2, 0.0)
    glClearDepth(1.0)

    glEnable(GL_DEPTH_TEST)

    # Configura os parametros intrinsecos da camera
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
 
    fovy = 45
    aspect = (width)/(height)
    gluPerspective(fovy, aspect, 0.1, 100.0)

def initLighting():

    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glEnable(GL_COLOR_MATERIAL)

    light_pos = [1.0, 1.0, 1.0, 1.0]
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos)

    diffuse = [1.0, 1.0, 1.0, 1.0]
    ambient = [0.2, 0.2, 0.2, 1.0]
    specular = [1.0, 1.0, 1.0, 1.0]
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient)
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular)

    glShadeModel(GL_SMOOTH)

def idleCallback():

    glutPostRedisplay()

def keyboard_callback(key, x, y):
    if key == b'\x1b' or key == b'q':  # ESC or 'q' to exit
        print("Exiting application...")
        os._exit(0)

if __name__ == '__main__':
 
    glutInit()
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH)
    glutInitWindowSize(640, 480)
    glutCreateWindow(b'Realidade Aumentada [codigo esqueleto]')

    initLighting()

    scene = Scene(obj3d_filename="./pikachu_obj/Pikachu.obj")
    glutDisplayFunc(scene.display)

    glutReshapeFunc(camera_config)
    glutIdleFunc(idleCallback)
    glutKeyboardFunc(keyboard_callback)
    glutMainLoop()
