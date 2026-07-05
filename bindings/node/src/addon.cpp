#include <napi.h>

Napi::Value HelloWorld(const Napi::CallbackInfo& info)
{
	return Napi::String::New(info.Env(), "Hello, World!");
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
	exports.Set("hello_world", Napi::Function::New(env, HelloWorld));
	
	return exports;
}

NODE_API_MODULE(nebula, Init)