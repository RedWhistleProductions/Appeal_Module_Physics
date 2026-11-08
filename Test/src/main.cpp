#include <iostream>
#include <string>

// Diagnostic stubs: print the function name without performing any operations.

extern "C" void Debug(bool Enable)
{
    std::cout << "Debug" << std::endl;
}

extern "C" void Set_Paths(std::string Resources)
{
    std::cout << "Set_Paths" << std::endl;
}

extern "C" void Attach(std::string Entity)
{
    std::cout << "Attach" << std::endl;
}

extern "C" void Detach(std::string Entity)
{
    std::cout << "Detach" << std::endl;
}

extern "C" void Update(std::string Entity)
{
    std::cout << "Update" << std::endl;
}

extern "C" void Set(std::string Entity, std::string Key, std::string Value)
{
    std::cout << "Set" << std::endl;
}

extern "C" void Set_Body_Type(std::string Entity, std::string Type)
{
    std::cout << "Set_Body_Type" << std::endl;
}

extern "C" void Set_Position(std::string Entity, float X, float Y, float Z)
{
    std::cout << "Set_Position" << std::endl;
}

extern "C" void Set_Velocity(std::string Entity, float X, float Y, float Z)
{
    std::cout << "Set_Velocity" << std::endl;
}

extern "C" void Apply_Force(std::string Entity, float X, float Y, float Z)
{
    std::cout << "Apply_Force" << std::endl;
}

extern "C" void Apply_Impulse(std::string Entity, float X, float Y, float Z)
{
    std::cout << "Apply_Impulse" << std::endl;
}

extern "C" void Set_Gravity(float X, float Y, float Z)
{
    std::cout << "Set_Gravity" << std::endl;
}

extern "C" void Step(float Seconds) { std::cout << "Step" << std::endl; }

extern "C" void Set_Units_Per_Meter(float Units) { std::cout << "Set_Units_Per_Meter" << std::endl; }

extern "C" void Set_Box(std::string Entity, float Width, float Height) { std::cout << "Set_Box" << std::endl; }

extern "C" void Set_Box_3D(std::string Entity, float Width, float Height, float Depth) { std::cout << "Set_Box_3D" << std::endl; }

extern "C" void Set_Circle(std::string Entity, float Radius) { std::cout << "Set_Circle" << std::endl; }

extern "C" void Set_Sphere(std::string Entity, float Radius) { std::cout << "Set_Sphere" << std::endl; }

extern "C" void Set_Capsule(std::string Entity, float Radius, float Height) { std::cout << "Set_Capsule" << std::endl; }

extern "C" std::string Get(std::string Entity, std::string Field) { std::cout << "Get" << std::endl; return "0"; }

extern "C" std::string Contact(std::string Entity, int Index, std::string Field) { std::cout << "Contact" << std::endl; return "0"; }

extern "C" std::string Event(std::string Entity, int Index, std::string Field) { std::cout << "Event" << std::endl; return "0"; }
