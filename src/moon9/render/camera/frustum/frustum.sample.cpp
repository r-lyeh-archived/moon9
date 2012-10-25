/* ------------------------------------------------------

 View Frustum Culling - Lighthouse3D

  -----------------------------------------------------*/

#include <moon9/render/render.hpp>
#include <moon9/render/frustum.hpp>

#include <moon9/spatial/vec.hpp>
#include <moon9/spatial/frustum.hpp>
#include <moon9/spatial/plane.hpp>

#include <moon9/spatial/frustum.hpp>

#include <moon9/hid/windows.hpp>
#include <moon9/debug/assert.hpp>

float a = 0;

float nearP = 1.0f, farP = 100.0f;
float fov = 45, aspect = 1;

int frame=0, timebase=0;

int frustumOn = 1;
int spheresDrawn = 0;
int spheresTotal = 0;
int draw_debug = 0;

moon9::frustum frustum;

void changeSize(int w, int h)
{
	aspect = float(w) / float( h > 0 ? h : 1 );

	// Reset the coordinate system before modifying
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport( 0, 0, w, h );

	// Set the correct perspective.
	gluPerspective( fov, aspect, nearP, farP );
	glMatrixMode( GL_MODELVIEW );

	frustum.setup( fov, aspect, nearP, farP );
}

void render() {

	glColor3f(0,1,0);
	spheresTotal=0;
	spheresDrawn=0;

/*
	if(draw_debug&1)
	frustum.draw_points();
	if(draw_debug&2)
	frustum.draw_lines();
	if(draw_debug&4)
	frustum.draw_normals();
	if(draw_debug&8)
	frustum.draw_planes();
*/

	for (int i = -200; i < 200; i+=4)
			for(int k =  -200; k < 200; k+=4) {
				spheresTotal++;
				moon9::vec3 a(i,0,k);

				bool ix = collide(frustum,a,0.5);

				if ( !frustumOn || ix ) {
					glPushMatrix();
					glTranslatef(i,0,k);
					glutSolidSphere(0.5,5,5);
					glPopMatrix();
					spheresDrawn++;
				}
			}

}


moon9::vec3 p(0,0,5),l(0,0,0),u(0,1,0);


void input()
{
	static moon9::windows::keyboard keyboard(0);
	keyboard.update();

	if( keyboard.s.hold() ) {
		p = p + moon9::vec3(0,0,1) * 0.1f;
		l = l + moon9::vec3(0,0,1) * 0.1f;
	}
	else
	if( keyboard.w.hold() ) {
		p = p - moon9::vec3(0,0,1) * 0.1f;
		l = l - moon9::vec3(0,0,1) * 0.1f;
	}

	if( keyboard.d.hold() ) {
		p = p + moon9::vec3(1,0,0)*0.1f;
		l = l + moon9::vec3(1,0,0)*0.1f;
	}
	else
	if( keyboard.a.hold() ) {
		p = p - moon9::vec3(1,0,0)*0.1f;
		l = l - moon9::vec3(1,0,0)*0.1f;
	}

	if( keyboard.e.hold() ) {
		p = p + moon9::vec3(0,1,0)*0.1f;
		l = l + moon9::vec3(0,1,0)*0.1f;
	}
	else
	if( keyboard.c.hold() ) {
		p = p - moon9::vec3(0,1,0)*0.1f;
		l = l - moon9::vec3(0,1,0)*0.1f;
	}

	if( keyboard.r.trigger() ) {
		p = moon9::vec3(0,0,5);
		l = moon9::vec3(0,0,0);
		u = moon9::vec3(0,1,0);
	}

	if( keyboard.f.trigger() )
		frustumOn = !frustumOn;

	if( keyboard.escape.trigger() )
		exit(0);

	if(keyboard.zero.trigger())
		draw_debug = 0;

	if(keyboard.one.trigger())
		draw_debug |= 1;

	if(keyboard.two.trigger())
		draw_debug |= 2;

	if(keyboard.three.trigger())
		draw_debug |= 4;

	if(keyboard.four.trigger())
		draw_debug |= 8;
}

void renderScene(void) {

	input();

	float fps,time;


	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(p.x,p.y,p.z,
		      l.x,l.y,l.z,
			  u.x,u.y,u.z);

	frustum.update(p,l,u);

	render();

	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0/(time-timebase);
		timebase = time;
		frame = 0;

		moon9::format title( "Frustum:%d Spheres Drawn:%d Total Spheres:%d FPS:%.2f",
					frustumOn,spheresDrawn,spheresTotal,fps );

		glutSetWindowTitle( title.c_str() );
	}

	glutSwapBuffers();
}





int main(int argc, char **argv)
{
	{
		using moon9::vec3;
		moon9::plane pl( vec3(0,0,0),vec3(1,0,0),vec3(1,1,0) );
		assert3( distance( pl, vec3(1,1,1) ), ==, 1 );
	}


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,640);
	glutCreateWindow("Camera View");

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	printf("Help Screen\n\nW,S,A,D,E,C: Move around\nR: Reset Position\nF: Turn frustum On/Off");

	glutMainLoop();

	return 0;
}

