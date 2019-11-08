#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include<iostream>
#include<vector>
#include<sstream>
#include<string>
#include<iterator>
#include<unordered_set>
#include<memory>

namespace egl
{

class Platform
{
public:
	std::string type;
	EGLenum etype;
	void* handle;
	std::string identifier;
	std::string subidentifier;
	std::string properties;
	Platform(const std::string& t,const EGLenum& et,void* h):type(t),etype(et),handle(h),identifier("UNKNOWN"),subidentifier(""),properties("")
	{}
};
class Device:public Platform 
{
public:
	std::unordered_set<std::string> extensions;
	Device(const EGLDeviceEXT& h):Platform("DEVICE:",EGL_PLATFORM_DEVICE_EXT,h)
	{
		PFNEGLQUERYDEVICESTRINGEXTPROC eglQueryDeviceStringEXT =(PFNEGLQUERYDEVICESTRINGEXTPROC)eglGetProcAddress("eglQueryDeviceStringEXT");
		std::string extstr=eglQueryDeviceStringEXT(handle,EGL_EXTENSIONS);
		std::istringstream iss(extstr);
		subidentifier="";
		
		extensions=std::unordered_set<std::string>((std::istream_iterator<std::string>(iss)),std::istream_iterator<std::string>());
		if(extensions.count("EGL_MESA_device_software"))
		{
			identifier="MESA";
		}
		else if(extensions.count("EGL_NV_device_cuda"))
		{
			identifier="NV"; //EGL_CUDA_DEVICE_NV get cuda device id?
		}
		else
		{
			identifier="UNKNOWN";
		}
		if(extensions.count("EGL_EXT_device_drm"))
		{
			subidentifier+=eglQueryDeviceStringEXT(handle,EGL_DRM_DEVICE_FILE_EXT);
		}
	}
	
	static std::vector<Device> enumerate()
	{
		EGLint numDevices;
		std::vector<EGLDeviceEXT> alldevices;
		PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =(PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
		if(!eglQueryDevicesEXT(0,nullptr, &numDevices)) return std::vector<Device>();
		
		alldevices.resize(numDevices);
		if(!eglQueryDevicesEXT(numDevices,alldevices.data(),&numDevices)) return std::vector<Device>();
		
		return std::vector<Device>(alldevices.begin(),alldevices.end());
	}
};
class Config;
class Display
{
private:
	static EGLDisplay plat2handle(const Platform& plat,std::vector<EGLint> attribs)
	{
		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =(PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
		if(attribs.size() && attribs.back() != EGL_NONE) attribs.push_back(EGL_NONE);
		return eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,plat.handle,attribs.data());
	}
	std::shared_ptr<int> copycounter;
public:
	EGLDisplay handle;
	EGLint egl_major,egl_minor;
	Display(EGLDisplay h):copycounter(new int),handle(h)
	{
		  if(!eglInitialize(handle,&egl_major,&egl_minor))
		  {
			  throw std::runtime_error("Failed to open EGLDisplay");
		  }
	}
	Display(const Platform& plat,const std::vector<EGLint>& attribs=std::vector<EGLint>()):Display(plat2handle(plat,attribs))
	{}
	~Display()
	{
		if(copycounter.unique()) eglTerminate(handle);
	}
	std::vector<Config> configs(std::vector<EGLint> attrib=std::vector<EGLint>()) const;
};
std::ostream& operator<<(std::ostream& out,const Device& dv)
{
	out << "EGLDevice: " << dv.identifier << " (" << dv.subidentifier << ")" << std::endl << "\t";
	std::copy(dv.extensions.begin(),dv.extensions.end(),std::ostream_iterator<std::string>(out," "));
	return out;
}
class Config
{
public:
	EGLConfig handle;
	Config(EGLConfig h=EGL_NO_CONFIG_KHR):handle(h)
	{}
	Config(const Display& disp,std::vector<EGLint> attribs=std::vector<EGLint>())
	{
		if(attribs.size() && attribs.back() != EGL_NONE) attribs.push_back(EGL_NONE);
		if(!eglChooseConfig(disp.handle,attribs.data(),&handle,1,nullptr))
		{
			throw std::runtime_error("Failed to find matching config");
		}
	}
};
std::vector<Config> Display::configs(std::vector<EGLint> attribs) const
{
	if(attribs.size() && attribs.back() != EGL_NONE) attribs.push_back(EGL_NONE);
	EGLint numConfigs=0;
	if(!eglChooseConfig(handle,attribs.data(),nullptr,0,&numConfigs) || numConfigs==0)
	{
		return std::vector<Config>();
	}
	std::vector<EGLConfig> cfgvec(numConfigs);
	EGLint numConfigs2=0;
	if(!eglChooseConfig(handle,attribs.data(),cfgvec.data(),numConfigs,&numConfigs2) || numConfigs2==0)
	{
		return std::vector<Config>();
	}
	return std::vector<Config>(cfgvec.begin(),cfgvec.end());
}
class Context
{
public:
	EGLContext handle;
protected:
	Context(EGLContext h):handle(h)
	{}
public:
	Context(const Display& display,
			const Config& config,
			std::vector<EGLint> attribs=std::vector<EGLint>(),
			const Context& share_ctx=Context(EGL_NO_CONTEXT)
   		)
	{
		if(attribs.size() && attribs.back() != EGL_NONE) attribs.push_back(EGL_NONE);
		handle=eglCreateContext(display.handle,config.handle,share_ctx.handle,attribs.data());
		if(handle == EGL_NO_CONTEXT)
		{
			throw std::runtime_error("Failed to create context");
		}
	}
};
}

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