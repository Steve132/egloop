
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <iostream>

  static const EGLint configAttribs[] = {
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_NONE
  };    

  static const int pbufferWidth = 9;
  static const int pbufferHeight = 9;

  static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
  };
static const EGLint contextAttribs[] = {
	EGL_CONTEXT_OPENGL_PROFILE_MASK,EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
		EGL_CONTEXT_MAJOR_VERSION,3,
		EGL_CONTEXT_MINOR_VERSION,1,
		EGL_NONE
};
int main(int argc, char *argv[])
{
  // 1. Initialize EGL
  EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  EGLint major, minor;

  eglInitialize(eglDpy, &major, &minor);

  // 2. Select an appropriate configuration
  EGLint numConfigs = 0;
  EGLConfig eglCfg;

  if(!eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs) || numConfigs == 0)
{
	std::cerr << "No config" << std::endl;
}

  // 3. Create a surface
 // EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, 
     //                                          pbufferAttribs);

  // 4. Bind the API
  eglBindAPI(EGL_OPENGL_API);

  // 5. Create a context and make it current
  EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, 
                                       contextAttribs);
  if(!eglCtx)
  {
	  std::cerr << "No context!" << std::endl;
  }

  //eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

  // from now on use your OpenGL context

  // 6. Terminate EGL when finished
  eglTerminate(eglDpy);
  return 0;
}
