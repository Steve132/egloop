#include "egloop.hpp"
#include <iostream>

int main(int,char**)
{
	std::vector<egloop::Device> egldevices=egloop::Device::enumerate();
	for(size_t i=0;i<egldevices.size();i++)
	{
		std::cout << egldevices[i] << std::endl;
	}
//	egloop::Display offscreen_display(egldevices[0]);
	egloop::Display offscreen_display=egloop::Display::Default();
	std::cout << "The EGL version is " << offscreen_display.egl_major << "." << offscreen_display.egl_minor << std::endl;

	
	egloop::Config opengl_anyconfig(offscreen_display,{
		EGL_RENDERABLE_TYPE,EGL_OPENGL_BIT
	});
	
	eglBindAPI(EGL_OPENGL_API);

	egloop::Context offscreen_context(offscreen_display,opengl_anyconfig,{
		EGL_CONTEXT_OPENGL_PROFILE_MASK,EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
		EGL_CONTEXT_MAJOR_VERSION,3,
		EGL_CONTEXT_MINOR_VERSION,1
	});
	return 0;
}
