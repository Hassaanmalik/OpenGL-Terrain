//Hassaan Malik 
//1224997
//Comp Sci 3GC3
//Assignment 2 

//extra features are Fault Algoithm and Improved Map colour 
//Instead of a topographics map I made it groovy by making it look like earth


//opengl cross platform includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h> //added math libary to code to allow for use of sin,cos and sqrt

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//initializing all the required variables
float heightmap[300][300];
float normals[300][300][3];
bool drawON = true;
float maxHeight = 0;
float minHeight = 0;
int terrainView = 2;
int terrainType = 0;
bool initalGrid = true;

//size of the area
int sizex = 200;
int sizez = 200;
float cols[3] = {1,1,1};

//camera positions
float camPos[] = {50, 100, 50};
float angle = 0.0f;

//lighting code
bool lightingON = true;
float light_pos1[] = {-150.0,150.0,100,1.0};
float light_pos2[] = {150.0,150.0,-100,1.0};
float amb0[4] = {1, 1, 1, 1}; 
float diff0[4] = {1, 1, 1, 1}; 
float spec0[4] = {1, 1, 1, 1};

/* MATERIALS */
float m_amb[] = { 0.1745f, 0.01175f, 0.01175f, 0.55f };
float m_diff[] = {0.61424f, 0.04136f, 0.04136f, 0.55f };
float m_spec[] = {0.727811f, 0.626959f, 0.626959f, 0.55f };
float shiny = 76.8;

//shading
bool flatShading = false;

//colur mode to look like earth
bool earthMode = false;

// calculate normal vectors for each quad of the terrain
void calcNormalVectors(){
    float v1[3];
    float v2[3];
    float v[3];
    for(int i =0; i<sizex; i++){
        for(int j=0; j<sizez; j++){
            v1[0] = i+1;
            v1[1] = heightmap[i+1][j] - heightmap[i][j];
            v1[2] = j;
            
            v2[0] = i+1;
            v2[1] = heightmap[i+1][j+1] - heightmap[i][j];
            v2[2] = j+1;
            
            //calculate cross product 
            v[0] = v1[1]*v2[2] - v1[2]*v2[1];
            v[1] = v1[2]*v2[0] - v1[0]*v2[2];
            v[2] = v1[0]*v2[1] - v1[1]*v2[0];
            float l = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            
            //normal vectors
            normals[i][j][0] = v[0]/l;
            normals[i][j][1] = v[1]/l;
            normals[i][j][2] = v[2]/l;
        }
    }
}

void drawMesh(){ //draws the terrrain in this part of the code and also enables colours and earth mode 
	int i =0;
	while(i<sizex-1){ //runs while loop for i j to draw the grid
		int j =0;
		while(j<sizez-1){
			if(terrainView==0 || terrainView==2){//checks if the view requires wire mode or not
				glColor3fv(cols);
				glBegin(GL_LINE_LOOP);//draw a line loop for the wire map
					glVertex3f(i,heightmap[i][j],j);
					glVertex3f(i+1,heightmap[i+1][j],j);
					glVertex3f(i+1,heightmap[i+1][j+1],j+1);
					glVertex3f(i,heightmap[i][j+1],j+1);
				glEnd();
			}

			if(terrainView==1 || terrainView==2){//checks if the view requires quads
				if(lightingON==false){//checks if lighting is on or not
					float faceColour=(((heightmap[i][j] - minHeight) / (maxHeight - minHeight))+0.05); //calculates the average value for the poistion which gives the neede colour
					if(earthMode==true){//checks if the earth mode is on or off
						if (heightmap[i][j] < 5){//creates the colour of water
							glColor3f(0.549*faceColour,0.8196*faceColour,1*faceColour);
						}
						else if(heightmap[i][j] > 6 & heightmap[i][j] < 8){//creates the colour of sand
							glColor3f(0.76*faceColour,0.698*faceColour,0.5*faceColour);
						}
						else if(heightmap[i][j] > 8 & heightmap[i][j] < 12){//creates the colour of grass
							glColor3f(0.396*faceColour,0.56*faceColour,0.19*faceColour);
						}
						else if(heightmap[i][j] > 12 & heightmap[i][j] < 20){//creates the colour of rocks/mountain
							glColor3f(0.65*faceColour,0.43*faceColour,0.18*faceColour);
						}
						else if(heightmap[i][j] > 20){//creats the colour of snow
							glColor3f(1*faceColour,0.98*faceColour,0.98*faceColour);
						}
					}
					else if(earthMode==false){//creats a grayscale map
						float faceColour=(((heightmap[i][j] - minHeight) / (maxHeight - minHeight))+0.05);
						glColor3f(faceColour,faceColour,faceColour);

					}

				}


				glBegin(GL_QUADS);//draws the quads withthe normals 
					glNormal3fv(normals[i][j]);
					glVertex3f(i,heightmap[i][j],j);

					glNormal3fv(normals[i+1][j]);
					glVertex3f(i+1,heightmap[i+1][j],j);

					glNormal3fv(normals[i+1][j+1]);
					glVertex3f(i+1,heightmap[i+1][j+1],j+1);

					glNormal3fv(normals[i][j+1]);
					glVertex3f(i,heightmap[i][j+1],j+1);
				glEnd();
			}
			j++;
		}
		i++;
	}
}

void creatHeightFault(){//runs the fault algoithm to change the heightmap in terms of how fault divides the terain in lines
	float v = static_cast <float> (rand());
	float a = sin(v);
	float b = cos(v);
	float d = sqrt(sizex*sizex + sizez*sizez);
	
	float c = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * d - d/2;
	for (int i = 0; i < sizex; i++){
		for (int j = 0; j < sizez; j++){
			int x = i - sizex/2;
			int z = j - sizez/2;
			if(a*x + b*z -c > 0)
			{
				 heightmap[i][j] += 1;	
			}
			else
			{	
				 heightmap[i][j] -= 1;
			}
		}
	}
	int i =0;
    while(i<sizex){//calculates the max and min value of the terain so that it can be used for colouring 
    	int j =0;
    	while(j<sizez){
    		if(heightmap[i][j]>maxHeight){
    			maxHeight=heightmap[i][j];
    		}
    		if(heightmap[i][j]<minHeight){
    			minHeight=heightmap[i][j];
    		}
    		j++;
    	}
    	i++;
    }
}

void creatHeightCircle(){ //runs the circle algorith to create the terrain
    int randomx = rand()%sizex;
    int randomz = rand()%sizez;
    int randomRadius = (rand()%20)+10;
    float disp = (rand()%12)+1;
    for(int i = 0; i<sizex; i++){
        for(int j= 0; j<sizez; j++){
            int dx = i - randomx;
            int dz = j - randomz;
            float distance = sqrtf((dx*dx) + (dz*dz));
            float pd = (distance*2)/randomRadius;
            if (fabs(pd) <= 1.0){
                heightmap[i][j] += (disp/2.0) + (cos(pd*3.14)*(disp/2.0));
            }
        }
    }
    int i =0;
    while(i<sizex){//calculates the max and min value of the terain so that it can be used for colouring 
    	int j =0;
    	while(j<sizez){
    		if(heightmap[i][j]>maxHeight){
    			maxHeight=heightmap[i][j];
    		}
    		if(heightmap[i][j]<minHeight){
    			minHeight=heightmap[i][j];
    		}
    		j++;
    	}
    	i++;
    }

}


void mesh(){//calculates the mesh values for height
	if(drawON == true){
		int run = 0;
		if(initalGrid==false){//checks if the flat grid is on or off
			while (run<500){//for the algorithms for 500 instances 
			if(terrainType==1){
				creatHeightCircle();//runs the circle algorithm 
			}
			else if(terrainType==2){
				creatHeightFault();//runs the fault algorithm 
			}
			run++;
			}
		}
		calcNormalVectors();
		drawON=false;
	}
}

void resetHeightMap(){//resets height to zero
	for(int x = 0; x<sizex; x++){
	    for(int z= 0; z<sizez; z++){
			heightmap[x][z]=0;
		}
	}
}


void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
		case 'e'://enables and disables earth mode
			if(earthMode==true){
				earthMode=false;
			}
			else{
				lightingON=false;
				glDisable(GL_LIGHTING);
				earthMode=true;
			}
			break;
		case 'k'://enables and disables shading
			if (flatShading==true){
				glShadeModel(GL_FLAT);
				flatShading=false;
			}
			else{
				glShadeModel(GL_SMOOTH);
				flatShading=true;
			}
			break;
		case 'l'://enables and disables lighting
			if(lightingON==true){
				lightingON=false;
				glDisable(GL_LIGHTING);
			}
			else{
				lightingON=true;
					glEnable(GL_LIGHTING);
				    glEnable(GL_LIGHT0);
				    glEnable(GL_LIGHT1);
			}
			break;
		case 'q'://quit the program
		case 27:
			exit (0);
			break;

		case 'r'://randomizes the terain 
			drawON = true;
			resetHeightMap();
			if(terrainType==1){
				creatHeightCircle();
			}
			else if(terrainType==2){
				creatHeightFault();
			}
			break;

		case 'w'://changes the view from wire to no wire to both 
			if(terrainView == 0){
				terrainView=1;
			}
			else if(terrainView==1){
				terrainView=2;
			}
			else if(terrainView==2){
				terrainView=0;
			}
			break;

		case 'v'://changes the view from no hieght to circle algorithm to fault algorithm 
			drawON = true;
			resetHeightMap();
			if(terrainType == 0){
				initalGrid=false;
				terrainType=1;
			}
			else if(terrainType==1){
				initalGrid=false;
				terrainType=2;
			}
			else if(terrainType==2){
				terrainType=0;
				initalGrid=true;
			}
			break;

		//moves the light sources 
		case 's':
			light_pos1[2] -=20;
			light_pos2[2] -=20;
			break;
		case 'z':
			light_pos1[0] -=20;
			light_pos2[0] -=20;
			break;
		case 'x':
			light_pos1[2] +=20;
			light_pos2[2] +=20;
			break;
		case 'c':
			light_pos1[0] +=20;
			light_pos2[0] +=20;
			break;
			
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			if(camPos[0]>-10){
				camPos[0] -= 2;
			}
			break;

		case GLUT_KEY_RIGHT:
			if(camPos[0]<sizez+70){
				camPos[0] += 2;
			}
			break;

		case GLUT_KEY_UP:
			if(camPos[2]>70){
				camPos[2] -= 2;
			}
			break;

		case GLUT_KEY_DOWN:
			if(camPos[2]<sizez+70){
				camPos[2] += 2;
			}
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 50;
			break;

		case GLUT_KEY_END:
			if(camPos[1]>10){
				camPos[1] -= 50;
			}
			break;

	}
	glutPostRedisplay();
}

void init(void){
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 1000);

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}
//cases for the menu
void menu (int value){//changes the size x and size z for a certain size 
	switch(value){
		case 'quit':
			exit(0);
		case 1:
			sizex = 50;
			sizez = 50;
			drawON=true;
			camPos[1] = 200;
			resetHeightMap();
			mesh();
			break;
		case 2:
			sizex = 100;
			sizez = 100;
			drawON=true;
			camPos[1] = 200;
			resetHeightMap();
			mesh();
			break;
		case 3:
			sizex = 150;
			sizez = 150;
			drawON=true;
			resetHeightMap();
			mesh();
			break;
		case 4:
			sizex = 200;
			sizez = 200;
			drawON=true;
			resetHeightMap();
			mesh();
			break;
		case 5:
			sizex = 250;
			sizez = 250;
			drawON=true;
			resetHeightMap();
			mesh();
			break;
		case 6:
			sizex = 300;
			sizez = 300;
			drawON=true;
			resetHeightMap();
			mesh();
			break;
	}
	glutPostRedisplay();

}

//menu for changing the size of the grid 
void initMenu (){
	glutCreateMenu(menu);
	glutAddMenuEntry("50 x 50",1);
	glutAddMenuEntry("100 x 100",2);
	glutAddMenuEntry("150 x 150",3);
	glutAddMenuEntry("200 x 200",4);
	glutAddMenuEntry("250 x 250",5);
	glutAddMenuEntry("300 x 300",6);
	glutAddMenuEntry("Quit",'quit');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	float origin[3] = {0,0,0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);

	//code for drawing the mesh
	mesh();
	drawMesh();
	
	//lighting code
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec0);

    //materials
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb); 
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff); 
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec); 
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);


	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Terrain");	//creates the window
	initMenu();
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	printf("----------How to use Terrain Program-------------\n");
	printf("Up,Down,Left,Right to control the camera.\n");
	printf("home and end to move camera up and down.\n");
	printf("s,z,x,c to control the camera movement \n");
	printf("e to enable/disable earth mode \n");
	printf("v to switch between circle algorthm, fault algorithm and flat grid \n");
	printf("l to enable/disable lighting mode \n");
	printf("k to enable/disable shading mode \n");
	printf("w to swithc between wire mode, non wire mode and both \n");
	printf("r to randomize the gird \n");
	printf("press q or esc to quit\n");

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}