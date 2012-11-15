// Copyright (c) 2006-2007 David Henry
// Doom 3's MD5Mesh Viewer main source file.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include <moon9/debug/callstack.hpp>
#include <moon9/time/timer.hpp>

#include <moon9/hid/windows.hpp>

#include <moon9/render/window.hpp>
#include <moon9/render/render.hpp>
#include <moon9/render/imgui.hpp>
#include <moon9/render/md2.hpp>

#include <moon9/time/tweener.hpp>
#include <moon9/time/fps.hpp>

#include <moon9/math/tween.hpp>
#include <moon9/math/rand.hpp>

#include "Mathlib.h"
#include "Md5Model.h"
#include "TextureManager.h"
#include "Shader.h"
#include "Main.h"


// Current shader and vertex/fragment programs
ShaderProgram *shader = NULL;

// We can use a specific render path, depending on
// which shader/program we want to use...
render_path_e renderPath = R_shader;

// Tangent uniform's location
GLint tangentLoc = -1;

bool bParallax = false;
bool bDrawNormals = false;

namespace md5demo
{
    Md5Model *model = NULL;
    Md5Object *object1 = NULL;
    Md5Object *object2 = NULL;

    int renderFlags = Md5Object::kDrawModel;

    bool bAnimate = true;
    bool bTextured = true;
    bool bCullFace = true;
    bool bBounds = false;
    bool bLight = true;
    bool bSmooth = true;
    bool bWireframe = false;

    bool bIsActive = true;

    std::vector<std::string> animations;

    const char windowTitle[] = "MD5 Model Viewer Demo";

    // Camera
    Vector3f rot, eye;
}

using namespace md5demo;



// Application's Timer

class Timer
{
    public:

        Timer() : current_time(0), last_time(0)
        {}

        void update()
        {
            last_time = current_time;
            current_time = dt.s();
        }

        double deltaTime() const
        {
            return(current_time - last_time);
        }

    private:

    double current_time;
    double last_time;

    moon9::dt dt;
};

Timer timer;



class windemo : public moon9::window
{
    public:

    windemo( std::string title, int argc, const char **argv ) : moon9::window( title )
    {
        // Initialize OpenGL
        initOpenGL();

        // Initialize application
        initApplication( (argc > 1) ? argv[1] : "zfat.loader" );
    }

    ~windemo()
    {
        delete model;
        delete object1;
        delete object2;
        delete shader;

        Texture2DManager::kill();
    }

    //update()
    //draw3d()
    //draw2d()

    void update( double t, float dt )
    {}

    void render( double t, float dt )
    {
        static moon9::windows::mouse mouse(0);
        static moon9::windows::keyboard keyboard(0);

        mouse.update();
        keyboard.update();

        camera.update();
        camera.apply();

        draw3D();
        draw2D();

        handleUI( *this, mouse, keyboard );

        cameraMove( mouse, keyboard );

        if( mouse.left.click(0.2f) )
        {
            float mousex = mouse.client.newest().x;
            float mousey = this->h - mouse.client.newest().y;

            std::vector<int> picks = picking( mousex, mousey, 1 );

            if( std::find( picks.begin(), picks.end(), 7) != picks.end() )
            {
                bBounds ^= true;
            }
        }

        // Update the timer
        timer.update();
    }

    // -------------------------------------------------------------------------
    // extractFromQuotes
    //
    // Extract a std::string from quotes.
    // -------------------------------------------------------------------------

    inline const std::string extractFromQuotes( const std::string &str )
    {
        std::string::size_type start = str.find_first_of( '\"' ) + 1;
        std::string::size_type end = str.find_first_of( '\"', start ) - 2;
        return str.substr( start, end );
    }


    // -------------------------------------------------------------------------
    // parseLoaderScript
    //
    // Parse a script file for loading md5mesh and animations.
    // -------------------------------------------------------------------------

    void parseLoaderScript(const std::string &filename)
    {
        // Open the file to parse
        std::ifstream file( filename.c_str(), std::ios::in );

        if( file.fail() )
        {
            std::cerr << "Couldn't open " << filename << std::endl;
            throw "argh!";
        }

        // Get texture manager
        Texture2DManager *texMgr = Texture2DManager::getInstance();

        while( !file.eof() )
        {
            std::string token, buffer;
            std::string meshFile, animFile, textureFile;
            std::string meshName, animName;

            // Peek next token
            file >> token;

            if( token == "model" )
            {
                std::getline( file, buffer );
                meshFile = extractFromQuotes(buffer);

                // Delete previous model and object if existing
                delete model;
                delete object1;
                delete object2;

                // Load mesh model
                model = new Md5Model(meshFile);
                object1 = new Md5Object(model);
                object2 = new Md5Object(model);
            }
            else if( token == "anim" )
            {
                std::getline( file, buffer );
                animFile = extractFromQuotes(buffer);

                try
                {
                    // Load animation
                    if( model )
                        model->addAnim(animFile);
                }
                catch( Md5Exception &err )
                {
                    std::cerr << "Failed to load animation " << animFile << std::endl;
                    std::cerr << "Reason: " << err.what() << "(" << err.which() << ")" << std::endl;
                }
            }
            else if( token == "hide" )
            {
                std::getline( file, buffer );
                meshName = extractFromQuotes(buffer);

                // Set mesh's render state
                if(model)
                    model->setMeshRenderState(meshName, Md5Mesh::kHide);
            }
            else if((token == "decalMap") ||
                    (token == "specularMap") ||
                    (token == "normalMap") ||
                    (token == "heightMap") )
            {
                    // Get the next token and extract the mesh name
                    file >> buffer;
                    long start = buffer.find_first_of('\"') + 1;
                    long end = buffer.find_first_of('\"', start) - 1;
                    meshName = buffer.substr(start, end);

                    // Get the rest of line and extract texture's filename
                    std::getline(file, buffer);
                    textureFile = extractFromQuotes(buffer);

                    // If the model has been loaded, setup
                    // the texture to the desired mesh
                    if(model)
                    {
                        Texture2D *tex = texMgr->load(textureFile);
                        if(tex->fail())
                            std::cerr << "failed to load " << textureFile << std::endl;

                        if(token == "decalMap")
                            model->setMeshDecalMap(meshName, tex);
                        else if(token == "specularMap")
                            model->setMeshSpecularMap(meshName, tex);
                        else if(token == "normalMap")
                            model->setMeshNormalMap(meshName, tex);
                        else if(token == "heightMap")
                            model->setMeshHeightMap(meshName, tex);
                    }
            }
            else if( token == "setAnim" )
            {
                std::getline(file, buffer);
                animName = extractFromQuotes(buffer);

                // Set object's default animation
                object1->setAnim(animName);
                object2->setAnim(animName);
            }
        }

        file.close();

        if(!model || (!object1 && !object2) )
            throw Md5Exception("No mesh found!", filename);
    }




    // -------------------------------------------------------------------------
    // initShader
    //
    // Shader's uniform variables initialization.
    // -------------------------------------------------------------------------

    void initShader()
    {
        if( !shader )
            return;

        shader->use();

        if( GLEW_VERSION_2_0 )
        {
            GLuint prog = shader->handle();

            // Set uniform parameters
            glUniform1i( glGetUniformLocation( prog,        "decalMap" ), 0 );
            glUniform1i( glGetUniformLocation( prog,        "glossMap" ), 1 );
            glUniform1i( glGetUniformLocation( prog,       "normalMap" ), 2 );
            glUniform1i( glGetUniformLocation( prog,       "heightMap" ), 3 );
            glUniform1i( glGetUniformLocation( prog, "parallaxMapping" ), bParallax );

            // Get attribute location
            tangentLoc = glGetAttribLocation( prog, "tangent" );
        }
        else
        {
            GLhandleARB prog = shader->handle();

            // Set uniform parameters
            glUniform1iARB( glGetUniformLocationARB( prog,        "decalMap" ), 0 );
            glUniform1iARB( glGetUniformLocationARB( prog,        "glossMap" ), 1 );
            glUniform1iARB( glGetUniformLocationARB( prog,       "normalMap" ), 2 );
            glUniform1iARB( glGetUniformLocationARB( prog,       "heightMap" ), 3 );
            glUniform1iARB( glGetUniformLocationARB( prog, "parallaxMapping" ), bParallax );

            // Get attribute location
            tangentLoc = glGetAttribLocationARB( prog, "tangent" );
        }

        shader->unuse();

        // Warn ff we fail to get tangent location... We'll can still use
        // the shader, but without tangents
        if(tangentLoc == -1)
           std::cerr << "Warning! No \"tangent\" uniform found in shader!" << std::endl;
    }


    // -------------------------------------------------------------------------
    // initOpenGL
    //
    // OpenGL initialization.
    // -------------------------------------------------------------------------

    void initOpenGL()
    {
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glShadeModel(GL_SMOOTH);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);


        // Initialize GLSL shader support
        initShaderHandling();

        if( hasShaderSupport())
        {
            // Load shader
            VertexShader vs("bump.vert");
            FragmentShader fs("bump.frag");
            shader = new ShaderProgram("bump mapping", vs, fs);

            // Initialize shader's uniforms
            initShader();
        }

        // Announce avalaible render paths, select the best
        std::cout << std::endl << "Avalaible render paths:" << std::endl;

        std::cout << " [3] - No bump mapping (fixed pipeline)" << std::endl;
        renderPath = R_fixed;

        if( shader )
        {
            std::cout << " [7] - Bump mapping with parallax " << "(GLSL)" << std::endl;
            renderPath = R_shader;
        }

        // Announce which path has been chosen by default
        std::cout << std::endl;
    }


    // -------------------------------------------------------------------------
    // initApplication
    //
    // Application initialization.
    // -------------------------------------------------------------------------

    void initApplication( const std::string &filename )
    {

        // Load model and animations
        try
        {
            if(filename.find(".md5mesh") == filename.length() - 8)
            {
                // Load mesh model
                model = new Md5Model(filename);
                object1 = new Md5Object(model);
                object2 = new Md5Object(model);
            }
            else
            {
                parseLoaderScript(filename);
            }
        }
        catch(Md5Exception &err)
        {
            std::cerr << "Failed to load model from " << filename << std::endl;
            std::cerr << "Reason: " << err.what() << "(" << err.which() << ")" << std::endl;
            throw "argh!";
        }


        // Build animation list
        Md5Model::AnimMap anims = model->anims();
        animations.reserve(anims.size() + 1);
        animations.push_back(std::string()); // bind-pose

        for(Md5Model::AnimMap::iterator iter = anims.begin(); iter != anims.end(); ++iter)
            animations.push_back(iter->first);

        // Camera initialization
        rot._x = 0.0;  eye._x = 0.0;
        rot._y = 0.0;  eye._y = 0.0;
        rot._z = 0.0;  eye._z = 200.0;

    }

    // -------------------------------------------------------------------------
    // setupLight
    //
    // Setup light position and enable light0.
    // -------------------------------------------------------------------------

    void setupLight( GLfloat x, GLfloat y, GLfloat z )
    {
        GLfloat lightPos[4];
        lightPos[0] = x;
        lightPos[1] = y;
        lightPos[2] = z;
        lightPos[3] = 1.0f;

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        if( bLight )
        {
            glPushMatrix();
                glLoadIdentity();
                glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
            glPopMatrix();

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
        }
    }


    // -------------------------------------------------------------------------
    // drawObb
    //
    // Draw an Oriented Bouding Box.
    // -------------------------------------------------------------------------

    void drawObb( const OBBox_t &obb )
    {
        moon9::enable::blending blending;
        moon9::enable::depth depth;
        moon9::disable::lighting no_lighting;
        moon9::disable::texturing no_texturing;

        Vector3f corners[8];

        corners[0] = Vector3f(-obb.extent._x, -obb.extent._y, -obb.extent._z);
        corners[1] = Vector3f( obb.extent._x, -obb.extent._y, -obb.extent._z);
        corners[2] = Vector3f( obb.extent._x,  obb.extent._y, -obb.extent._z);
        corners[3] = Vector3f(-obb.extent._x,  obb.extent._y, -obb.extent._z);
        corners[4] = Vector3f(-obb.extent._x, -obb.extent._y,  obb.extent._z);
        corners[5] = Vector3f( obb.extent._x, -obb.extent._y,  obb.extent._z);
        corners[6] = Vector3f( obb.extent._x,  obb.extent._y,  obb.extent._z);
        corners[7] = Vector3f(-obb.extent._x,  obb.extent._y,  obb.extent._z);

        glPushAttrib(GL_ENABLE_BIT);

            for(int i = 0; i < 8; ++i)
            {
              corners[i] += obb.center;
              obb.world.transform(corners[i]);
            }

            GLuint indices[24] =
            {
              0, 1, 1, 2, 2, 3, 3, 0,
              4, 5, 5, 6, 6, 7, 7, 4,
              0, 4, 1, 5, 2, 6, 3, 7
            };

            glColor3f(1.0, 0.0, 0.0);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, corners);
            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
            glDisableClientState(GL_VERTEX_ARRAY);

        // GL_ENABLE_BIT
        glPopAttrib();

        //moon9::matrix::scale sc( vec3(obb.extent._x*2, obb.extent._y*2, obb.extent._z*2) );
        //moon9::cube c;

    }


    // -------------------------------------------------------------------------
    // drawAxes
    //
    // Draw the X, Y and Z axes at the center of world.
    // -------------------------------------------------------------------------

    void drawAxes( const Matrix4x4f &modelView )
    {
        // Setup world model view matrix
        glLoadIdentity();
        glMultMatrixf(modelView._m);

        // Draw the three axes
        glBegin(GL_LINES);
            // X-axis in red
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3fv(kZeroVectorf._v);
            glVertex3fv(kZeroVectorf + Vector3f(1.0f, 0.0f, 0.0));

            // Y-axis in green
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3fv(kZeroVectorf._v);
            glVertex3fv(kZeroVectorf + Vector3f(0.0f, 1.0f, 0.0));

            // Z-axis in blue
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3fv(kZeroVectorf._v);
            glVertex3fv(kZeroVectorf + Vector3f(0.0f, 0.0f, 1.0));
        glEnd();
    }

    Matrix4x4f setup_camera()
    {
        // Camera rotation
        Matrix4x4f camera;

        #if 0
            camera.identity();

            glTranslated(-eye._x, -eye._y, -eye._z);
            glRotated(rot._x, 1.0f, 0.0f, 0.0f);
            glRotated(rot._y, 0.0f, 1.0f, 0.0f);
            glRotated(rot._z, 0.0f, 0.0f, 1.0f);
        #else
            camera.fromEulerAngles( degToRad(rot._x), degToRad(rot._y), degToRad(rot._z) );
            camera.setTranslation( -eye );
        #endif

        Matrix4x4f axisRotation
        = RotationMatrix(kXaxis, -kPiOver2)
        * RotationMatrix(kZaxis, -kPiOver2);

        Matrix4x4f final = camera * axisRotation;
        //glMultMatrixf(final._m);

        return final;
    }

    void setup_scene()
    {
        // Setup scene lighting
        setupLight(0.0f, 20.0f, 100.0f);

        // Enable/disable texture mapping(fixed pipeline)
        if(bTextured)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);

        // Enable/disable backface culling
        if(bCullFace)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        // Setup polygon mode and shade model
        glPolygonMode(GL_FRONT_AND_BACK, bWireframe ? GL_LINE : GL_FILL);
        glShadeModel(bSmooth ? GL_SMOOTH : GL_FLAT);
    }

    void draw_zombie( Md5Object *object, Matrix4x4f &camera )
    {
        // Draw object
        double anim_speed = 1.0f;
        object->setModelViewMatrix( camera );
        object->setRenderFlags( renderFlags );
        object->animate( bAnimate ? timer.deltaTime() * anim_speed : 0.0f );
        object->computeBoundingBox();
        object->prepare( false );
        moon9::matrix::scale sc( 0.05f );
        object->render();
        if(bBounds)
            drawObb(object->boundingBox());

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        drawAxes(camera);
    }

    void draw_cube()
    {
        moon9::cube cube;
    }

    void draw_plane()
    {
        moon9::style::white color;
        //moon9::enable::blending bl;
        moon9::disable::lighting no_lights;
        moon9::style::texture tx( "textures/board.png" );
        moon9::matrix::rotate_x rx(90.f);
        moon9::matrix::scale sc(50.f);
        moon9::geometry::plane floor(50.f);

        moon9::disable::depth no_depth;
        moon9::geometry::axes axis;
    }

    void draw_runner()
    {
    }


    // -------------------------------------------------------------------------
    // draw3D
    //
    // Render the 3D part of the scene.
    // -------------------------------------------------------------------------

    void draw3D()
    {
        moon9::check check(__FILE__, __LINE__);

        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor4b( 255, 255, 255, 255 );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        Matrix4x4f camera = setup_camera();

        setup_scene();

        //glPushName(7);
            glLoadName(7);
            draw_zombie( object1, camera );
        //glPopName();

        //glPushName(1);
            //glLoadName(1);
            //draw_cube();
            //draw_runner();
        //glPopName();

        //glPushName(14);
            glLoadName(14);
            draw_plane();
        //glPopName();

        return ;
    }


    // ref: http://web.engr.oregonstate.edu/~mjb/cs553/Handouts/Picking/picking.pdf

    std::vector<int> picking(int x, int y, int radius)
    {
        GLuint buff[64] = {0};
        GLint view[4];

        /*
            This choose the buffer where store the values for the selection data
        */
        glSelectBuffer(64, buff);

        /*
            This retrieve info about the viewport
        */
        glGetIntegerv(GL_VIEWPORT, view);

        /*
            Switching in selecton mode
        */
        // uncomment to debug view
        glRenderMode(GL_SELECT);

        /*
            Clearing the name's stack
            This stack contains all the info about the objects
        */
        glInitNames();

        /*
            Now fill the stack with one element (or glLoadName will generate an error)
        */
        glPushName(0);

        /*
            Now modify the vieving volume, restricting selection area around the cursor
        */
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();

            /*
                restrict the draw to an area around the cursor
            */
            //gluPickMatrix(w/2, h/2, w, h, view); //center and radius
            gluPickMatrix(x, y, radius, radius, view);

            //gluPerspective(60, 1.0, 0.0001, 1000.0);

                float aspect = w / h;
                float halfy = std::tan( (/*moon9::camera::FOV*/ 75.0 * 0.5f) * 3.14159 / 360.0 );
            gluPerspective((/*moon9::camera::FOV*/ 75.0 * 0.5f), aspect, 1, 100000);


            /*
                Draw the objects onto the screen
            */
            glMatrixMode(GL_MODELVIEW);

            /*
                draw only the names in the stack, and fill the array
            */
            glutSwapBuffers();
            draw3D();

            /*
                Do you remeber? We do pushMatrix in PROJECTION mode
            */
            glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        /*
            get number of objects drawed in that area
            and return to render mode
        */
        std::vector<int> hits;  // deberia ser vector<glInt>

        GLint num_hits = glRenderMode(GL_RENDER);

        if( num_hits )
        {
            /*
                For each hit in the buffer are allocated 4 bytes:
                1. Number of hits selected (always one,
                                            beacuse when we draw each object
                                            we use glLoadName, so we replace the
                                            prevous name in the stack)
                2. Min Z
                3. Max Z
                4. Name of the hit (glLoadName)
            */

            for( int i = 0; i < num_hits; i++ )
                hits.push_back( (int)buff[i * 4 + 3] );
        }

        glMatrixMode(GL_MODELVIEW);

        return hits;
    }

    // -------------------------------------------------------------------------
    // draw2D
    //
    // Render the 2D part of the scene.
    // -------------------------------------------------------------------------

    void draw2D()
    {
        // Reset texture matrix
        glActiveTexture(GL_TEXTURE0);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
    }


    // -------------------------------------------------------------------------
    // handleKeyPress
    // -------------------------------------------------------------------------

    void handleUI( moon9::window &win, moon9::windows::mouse &mouse, moon9::windows::keyboard &keyboard )
    {
        float mousex = mouse.client.newest().x;
        float mousey = mouse.client.newest().y;
        bool mouselh = mouse.left.hold();

        static moon9::fps fps;
        fps.tick();

        moon9::ui renderer( win.w, win.h, mousex, mousey, mouselh, false );

        if( moon9::box b = moon9::box( "Options", 16 - 350, 16, 200*2, 350 ).enabled().color( moon9::orange ) )
        {
            // Print frame rate + current animation in the top-left corner

            moon9::pair( "fps", fps.get() );
            moon9::pair( "object1.anim", object1->currAnimName() );
            moon9::pair( "object2.anim", object2->currAnimName() );

            bool left = keyboard.left.trigger();
            bool right = keyboard.right.trigger();

            if( left || right )
            {
                // Find the current animation in the list
                std::vector<std::string>::iterator iter = std::find( animations.begin(), animations.end(), object1->currAnimName() );

                // Peek next or previous animation name
                if( right )
                {
                    ++iter;
                }
                else
                {
                    if( iter == animations.begin() )
                        iter = animations.end();

                    --iter;
                }

                if( iter == animations.end() )
                    iter = animations.begin();

                // Set the new animation to play
                object1->setAnim(*iter);
                object2->setAnim(*iter);

                return;
            }

            if( moon9::toggle("bAnimate", bAnimate) )
            ;

            if( moon9::toggle("bBounds", bBounds) )
            ;

            if( moon9::toggle("bCullFace", bCullFace) )
            ;

            if( moon9::toggle("bLight", bLight) )
            ;

            if( moon9::toggle("bDrawNormals", bDrawNormals) )
            ;

            static bool bDrawJointLabels =( renderFlags & Md5Object::kDrawJointLabels == 0);

            if( moon9::toggle("bDrawJointLabels", bDrawJointLabels) )
              renderFlags ^= Md5Object::kDrawJointLabels;

            static bool bDrawSkeleton =( renderFlags & Md5Object::kDrawSkeleton == 0 );

            if( moon9::toggle("bDrawSkeleton", bDrawSkeleton) )
              renderFlags ^= Md5Object::kDrawSkeleton;

            if( moon9::toggle("bParallax", bParallax) )
            {
                if( shader )
                {
                    shader->use();

                    if(GLEW_VERSION_2_0)
                        glUniform1i(glGetUniformLocation(shader->handle(), "parallaxMapping"), bParallax);
                    else
                        glUniform1iARB(glGetUniformLocationARB(shader->handle(), "parallaxMapping"), bParallax);

                    shader->unuse();
                }
                else
                {
                    bParallax = false;
                }
            }

            if( moon9::toggle("bSmooth", bSmooth) )
            ;

            if( moon9::toggle("bTextured", bTextured) )
            ;

            if( moon9::toggle("bWireframe", bWireframe) )
            ;

            //if( moon9::toggle("FullScreen", FullScreen) )
            {
              //SDL_WM_ToggleFullScreen(surface);
            }

            static int shader_model =(int)renderPath;

            const char *myradio[] = { "no bump mapping(fixed pipeline) / R_fixed", "bump mapping with parallax(GLSL) / R_shader" };

            if( moon9::radio l = moon9::radio( "Shader model", 2, myradio ) )
            {
                shader_model = l.result();
                std::cout << l.result() << std::endl;

                if( shader_model == 0 )
                {
                  renderPath = R_fixed;
                }

                if( shader_model == 1 )
                {
                    if(shader)
                        renderPath = R_shader;
                }
            }
        }
    }


    // -------------------------------------------------------------------------
    // MouseMove
    // -------------------------------------------------------------------------

    void cameraMove( moon9::windows::mouse &mouse, moon9::windows::keyboard &keyboard )
    {
        // Right button
        if( mouse.right.hold() )
        {
            // Zoom
            eye._z += mouse.client.newest().ydt; // * 0.1f;
            return;
        }

        // Wheel button
        if( mouse.wheel.newest().ydt )
        {
            // Zoom
            eye._z += mouse.wheel.newest().ydt * 100; // * 0.1f;
            return;
        }

        // Left button
        if( mouse.left.hold() )
        {
            if( keyboard.shift.hold() )
            {
                // Translation
                eye._x -= mouse.client.newest().xdt * 0.25f;
                eye._y += mouse.client.newest().ydt * 0.25f;
            }
            else
            {
                // Rotation
                rot._x += mouse.client.newest().ydt * 0.5f;
                rot._y += mouse.client.newest().xdt * 0.5f;
            }
        }
    }

};



// -------------------------------------------------------------------------
// main
//
// Application main entry point.
// -------------------------------------------------------------------------

int main(int argc, const char **argv)
{
    std::cout << "Built release " << __DATE__ << " / " << __TIME__ << std::endl;

    windemo win( windowTitle, argc, argv );

    // Loop until the end
    while( win.is_open() ) //&&(!keyboard.escape.trigger()) )
        win.flush();

    return 0;
}
