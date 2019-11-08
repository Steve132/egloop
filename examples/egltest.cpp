#include "egloop.hpp"


int main(int,char**)
{
	std::vector<egl::Device> egldevices=egl::Device::enumerate();
	for(size_t i=0;i<egldevices.size();i++)
	{
		std::cout << egldevices[i] << std::endl;
	}
	egl::Display offscreen_display(egldevices[0]);
	std::cout << "The EGL version is " << offscreen_display.egl_major << "." << offscreen_display.egl_minor << std::endl;

	egl::Config opengl_anyconfig(offscreen_display,{
		EGL_RENDERABLE_TYPE,EGL_OPENGL_BIT
	});
	eglBindAPI(EGL_OPENGL_API);
	egl::Context offscreen_context(offscreen_display,opengl_anyconfig,{
		EGL_CONTEXT_OPENGL_PROFILE_MASK,EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
		EGL_CONTEXT_MAJOR_VERSION,4,
		EGL_CONTEXT_MINOR_VERSION,5
	});
	return 0;
}
