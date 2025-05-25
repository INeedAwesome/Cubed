#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "Walnut/Core/Log.h"

#include "ClientLayer.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Cubed Client";
	spec.CustomTitlebar = false;
	spec.UseDockspace = false;
	spec.CenterWindow = true;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<Cubed::ClientLayer>();
	
	return app;
}