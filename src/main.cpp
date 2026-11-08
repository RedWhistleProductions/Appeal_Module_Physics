#include "Plugin.h"
#include "Data_Source.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <set>

namespace
{
    struct Physics_Functions
    {
        void (*Set_Box_3D)(std::string,float,float,float) = nullptr;
        void (*Set_Sphere)(std::string,float) = nullptr;
        void (*Step)(float) = nullptr;
        void (*Set_Units_Per_Meter)(float) = nullptr;
        void (*Set_Box)(std::string,float,float) = nullptr;
        void (*Set_Circle)(std::string,float) = nullptr;
        void (*Set_Capsule)(std::string,float,float) = nullptr;
        std::string (*Get)(std::string,std::string) = nullptr;
        std::string (*Contact)(std::string,int,std::string) = nullptr;
        std::string (*Event)(std::string,int,std::string) = nullptr;
        void (*Debug)(bool Enable) = nullptr;
        void (*Set_Paths)(std::string Resources) = nullptr;
        void (*Attach)(std::string Entity) = nullptr;
        void (*Detach)(std::string Entity) = nullptr;
        void (*Update)(std::string Entity) = nullptr;
        void (*Set)(std::string Entity, std::string Key, std::string Value) = nullptr;
        void (*Set_Body_Type)(std::string Entity, std::string Type) = nullptr;
        void (*Set_Position)(std::string Entity, float X, float Y, float Z) = nullptr;
        void (*Set_Velocity)(std::string Entity, float X, float Y, float Z) = nullptr;
        void (*Apply_Force)(std::string Entity, float X, float Y, float Z) = nullptr;
        void (*Apply_Impulse)(std::string Entity, float X, float Y, float Z) = nullptr;
        void (*Set_Gravity)(float X, float Y, float Z) = nullptr;
    };

    struct Plugin_Slot
    {
        std::string Name;
        std::string File;
        std::unique_ptr<Plugin> Loaded_Plugin;
        Physics_Functions Functions;
        bool Should_Load = false;
        bool Is_Loaded = false;
        int Users = 0;
    };

    bool Module_Debug = false;
    std::string Resources_Path = "./Resources";
    std::string Load_Mode = "Manual";
    std::map<std::string, Plugin_Slot> Plugins;
    std::map<std::string, std::string> Entity_Bodies;

    template<typename T>
    void Assign(Plugin_Slot &Slot, std::string Name, T &Function)
    {
        Slot.Loaded_Plugin->Assign(Name, Function);
    }

    void Print(const std::string &Message)
    {
        if(Module_Debug) std::cout << Message << std::endl;
    }

    Plugin_Slot *Find_Slot(const std::string &Name)
    {
        auto Found = Plugins.find(Name);
        if(Found == Plugins.end())
        {
            if(Module_Debug) std::cerr << "Physics plugin slot not found: " << Name << std::endl;
            return nullptr;
        }
        return &Found->second;
    }

    bool Load_Slot(Plugin_Slot &Slot)
    {
        if(Slot.Is_Loaded) return true;
        if(Slot.File.empty())
        {
            if(Module_Debug) std::cerr << "Physics plugin slot has no file: " << Slot.Name << std::endl;
            return false;
        }

        Slot.Loaded_Plugin = std::make_unique<Plugin>();
        Slot.Loaded_Plugin->Load(Slot.File);
        Assign(Slot, "Set_Box_3D", Slot.Functions.Set_Box_3D);
        Assign(Slot, "Set_Sphere", Slot.Functions.Set_Sphere);
        Assign(Slot, "Step", Slot.Functions.Step);
        Assign(Slot, "Set_Units_Per_Meter", Slot.Functions.Set_Units_Per_Meter);
        Assign(Slot, "Set_Box", Slot.Functions.Set_Box);
        Assign(Slot, "Set_Circle", Slot.Functions.Set_Circle);
        Assign(Slot, "Set_Capsule", Slot.Functions.Set_Capsule);
        Assign(Slot, "Get", Slot.Functions.Get);
        Assign(Slot, "Contact", Slot.Functions.Contact);
        Assign(Slot, "Event", Slot.Functions.Event);
        Assign(Slot, "Debug", Slot.Functions.Debug);
        Assign(Slot, "Set_Paths", Slot.Functions.Set_Paths);
        Assign(Slot, "Attach", Slot.Functions.Attach);
        Assign(Slot, "Detach", Slot.Functions.Detach);
        Assign(Slot, "Update", Slot.Functions.Update);
        Assign(Slot, "Set", Slot.Functions.Set);
        Assign(Slot, "Set_Body_Type", Slot.Functions.Set_Body_Type);
        Assign(Slot, "Set_Position", Slot.Functions.Set_Position);
        Assign(Slot, "Set_Velocity", Slot.Functions.Set_Velocity);
        Assign(Slot, "Apply_Force", Slot.Functions.Apply_Force);
        Assign(Slot, "Apply_Impulse", Slot.Functions.Apply_Impulse);
        Assign(Slot, "Set_Gravity", Slot.Functions.Set_Gravity);

        if(Slot.Functions.Debug != nullptr) Slot.Functions.Debug(Module_Debug);
        if(Slot.Functions.Set_Paths != nullptr) Slot.Functions.Set_Paths(Resources_Path);

        Slot.Is_Loaded = true;
        Print("Physics loaded plugin: " + Slot.Name);
        return true;
    }

    void Unload_Slot(Plugin_Slot &Slot, bool Force = false)
    {
        if(!Slot.Is_Loaded) return;
        if(!Force && Slot.Users > 0)
        {
            Print("Physics plugin still in use: " + Slot.Name);
            return;
        }

        Slot.Functions = Physics_Functions{};
        Slot.Loaded_Plugin.reset();
        Slot.Is_Loaded = false;
        Print("Physics unloaded plugin: " + Slot.Name);
    }

    void Apply_Plan()
    {
        for(auto &Pair : Plugins)
        {
            Plugin_Slot &Slot = Pair.second;
            bool Wanted = Slot.Should_Load;
            if(Load_Mode == "All") Wanted = true;
            else if(Load_Mode == "On_Demand") Wanted = Slot.Users > 0;

            if(Wanted) Load_Slot(Slot);
            else Unload_Slot(Slot);
        }
    }

    Plugin_Slot *Entity_Slot(const std::string &Entity)
    {
        auto Body = Entity_Bodies.find(Entity);
        if(Body == Entity_Bodies.end()) return nullptr;
        return Find_Slot(Body->second);
    }
}

extern "C" void Init(std::string File)
{
    Plugin_Slot &Slot = Plugins["Default"];
    Slot.Name = "Default";
    Slot.File = File;
    Slot.Should_Load = true;
    Load_Slot(Slot);
}

extern "C" void Interpreter(Data_Source *Data)
{
    std::string Command;
    *Data >> Command;

    if(Command == "Init")
    {
        std::string File;
        *Data >> File;
        Init(File);
    }
    else if(Command == "Debug")
    {
        std::string Value;
        *Data >> Value;
        if(Value == "1" || Value == "On" || Value == "on" || Value == "True" || Value == "true")
        {
            Module_Debug = true;
        }
        else if(Value == "0" || Value == "Off" || Value == "off" || Value == "False" || Value == "false")
        {
            Module_Debug = false;
        }
        for(auto &Pair : Plugins)
            if(Pair.second.Functions.Debug != nullptr) Pair.second.Functions.Debug(Module_Debug);
    }
    else if(Command == "Set_Paths")
    {
        *Data >> Resources_Path;
        for(auto &Pair : Plugins)
            if(Pair.second.Functions.Set_Paths != nullptr) Pair.second.Functions.Set_Paths(Resources_Path);
    }
    else if(Command == "Add_Plugin")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Plugin_Slot &Slot = Plugins[Name];
        Slot.Name = Name;
        Slot.File = File;
        Print("Physics plugin added: " + Name + " -> " + File);
    }
    else if(Command == "Remove_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Unload_Slot(*Slot, true);
            for(auto i=Entity_Bodies.begin();i!=Entity_Bodies.end();)
                if(i->second==Name) i=Entity_Bodies.erase(i); else ++i;
            Plugins.erase(Name);
        }
    }
    else if(Command == "Load_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Slot->Should_Load = true;
            Load_Slot(*Slot);
        }
    }
    else if(Command == "Unload_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Slot->Should_Load = false;
            Unload_Slot(*Slot);
        }
    }
    else if(Command == "Set_Plugin_Wanted")
    {
        std::string Name;
        int Wanted;
        *Data >> Name;
        *Data >> Wanted;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr) Slot->Should_Load = Wanted != 0;
    }
    else if(Command == "Apply_Load_Plan")
    {
        Apply_Plan();
    }
    else if(Command == "Set_Load_Mode")
    {
        *Data >> Load_Mode;
        Print("Physics load mode: " + Load_Mode);
        Apply_Plan();
    }
    else if(Command == "List_Plugins")
    {
        std::cout << "Physics plugins:" << std::endl;
        for(const auto &Pair : Plugins)
        {
            const Plugin_Slot &Slot = Pair.second;
            std::cout << "  " << Slot.Name << " loaded=" << Slot.Is_Loaded
                      << " wanted=" << Slot.Should_Load << " users=" << Slot.Users
                      << " file=" << Slot.File << std::endl;
        }
    }
    else if(Command == "Use" || Command == "Attach")
    {
        std::string Entity, Plugin_Name;
        *Data >> Entity;
        *Data >> Plugin_Name;
        Plugin_Slot *New_Slot = Find_Slot(Plugin_Name);
        if(New_Slot == nullptr) return;
        Load_Slot(*New_Slot);

        auto Existing = Entity_Bodies.find(Entity);
        if(Existing != Entity_Bodies.end())
        {
            Plugin_Slot *Old_Slot = Find_Slot(Existing->second);
            if(Old_Slot != nullptr)
            {
                if(Old_Slot->Functions.Detach != nullptr) Old_Slot->Functions.Detach(Entity);
                if(Old_Slot->Users > 0) --Old_Slot->Users;
            }
        }

        Entity_Bodies[Entity] = Plugin_Name;
        ++New_Slot->Users;
        if(New_Slot->Functions.Attach != nullptr) New_Slot->Functions.Attach(Entity);
        if(Load_Mode == "On_Demand") Apply_Plan();
    }
    else if(Command == "Release" || Command == "Detach")
    {
        std::string Entity;
        *Data >> Entity;
        auto Existing = Entity_Bodies.find(Entity);
        if(Existing != Entity_Bodies.end())
        {
            Plugin_Slot *Slot = Find_Slot(Existing->second);
            if(Slot != nullptr)
            {
                if(Slot->Functions.Detach != nullptr) Slot->Functions.Detach(Entity);
                if(Slot->Users > 0) --Slot->Users;
            }
            Entity_Bodies.erase(Existing);
            if(Load_Mode == "On_Demand") Apply_Plan();
        }
    }
    else if(Command == "Update")
    {
        std::string Entity;
        *Data >> Entity;
        if(Entity == "All")
        {
            std::set<std::string> Stepped;
            for(const auto &Pair : Entity_Bodies)
            {
                Plugin_Slot *Slot = Find_Slot(Pair.second);
                if(Slot && Slot->Functions.Step) {
                    if(Stepped.insert(Pair.second).second) Slot->Functions.Step(1.0f/60);
                } else if(Slot && Slot->Functions.Update) Slot->Functions.Update(Pair.first);
            }
        }
        else
        {
            Plugin_Slot *Slot = Entity_Slot(Entity);
            if(Slot != nullptr && Slot->Functions.Update != nullptr) Slot->Functions.Update(Entity);
        }
    }
    else if(Command == "Step" || Command == "Set_Units_Per_Meter")
    {
        float Value; *Data >> Value;
        for(auto &Pair : Plugins) if(Pair.second.Is_Loaded) {
            auto Fn=Command=="Step"?Pair.second.Functions.Step:Pair.second.Functions.Set_Units_Per_Meter;
            if(!Fn) throw std::runtime_error("Physics plugin does not support "+Command+": "+Pair.first);
            Fn(Value);
        }
    }
    else if(Command == "Get" || Command == "Contact" || Command == "Event" || Command == "Touching")
    {
        std::string Entity,Field; int Index=0; *Data >> Entity;
        auto* Slot=Entity_Slot(Entity);
        if(!Slot || !Slot->Functions.Get) throw std::runtime_error("Physics query requires an attached collision-capable body: "+Entity);
        auto& F=Slot->Functions;
        if(Command=="Get") { *Data >> Field; Data->Return_Value(F.Get(Entity,Field)); }
        else if(Command=="Touching") {
            *Data >> Field; if(!F.Contact) throw std::runtime_error("Physics Contact unsupported");
            int Count=std::stoi(F.Get(Entity,"Contact_Count")); bool Found=false;
            for(int i=0;i<Count;++i) if(F.Contact(Entity,i,"Other")==Field) Found=true;
            Data->Return_Value(Found?"1":"0");
        } else {
            *Data >> Index; *Data >> Field;
            auto Fn=Command=="Contact"?F.Contact:F.Event;
            if(!Fn) throw std::runtime_error("Physics query unsupported: "+Command);
            Data->Return_Value(Fn(Entity,Index,Field));
        }
    }
    else if(Command == "Set_Box_3D" || Command == "Set_Sphere")
    {
        std::string Entity; float X,Y=0,Z=0; *Data >> Entity; *Data >> X;
        if(Command=="Set_Box_3D") { *Data >> Y; *Data >> Z; }
        auto* Slot=Entity_Slot(Entity);
        if(!Slot) throw std::runtime_error("Physics body not attached: "+Entity);
        if(Command=="Set_Box_3D" && Slot->Functions.Set_Box_3D) Slot->Functions.Set_Box_3D(Entity,X,Y,Z);
        else if(Command=="Set_Sphere" && Slot->Functions.Set_Sphere) Slot->Functions.Set_Sphere(Entity,X);
        else throw std::runtime_error("Physics plugin does not support "+Command);
    }
    else if(Command == "Set_Box" || Command == "Set_Capsule" || Command == "Set_Circle")
    {
        std::string Entity; float X,Y=0; *Data >> Entity; *Data >> X;
        if(Command!="Set_Circle") *Data >> Y;
        auto* Slot=Entity_Slot(Entity);
        if(!Slot) throw std::runtime_error("Physics body not attached: "+Entity);
        auto& F=Slot->Functions;
        if(Command=="Set_Circle" && F.Set_Circle) F.Set_Circle(Entity,X);
        else if(Command=="Set_Box" && F.Set_Box) F.Set_Box(Entity,X,Y);
        else if(Command=="Set_Capsule" && F.Set_Capsule) F.Set_Capsule(Entity,X,Y);
        else throw std::runtime_error("Physics plugin does not support "+Command);
    }
    else if(Command == "Set")
    {
        std::string Entity, Key, Value;
        *Data >> Entity;
        *Data >> Key;
        *Data >> Value;
        Plugin_Slot *Slot = Entity_Slot(Entity);
        if(Slot != nullptr && Slot->Functions.Set != nullptr) Slot->Functions.Set(Entity, Key, Value);
    }
    else if(Command == "Set_Body_Type")
    {
        std::string Entity, Type;
        *Data >> Entity;
        *Data >> Type;
        Plugin_Slot *Slot = Entity_Slot(Entity);
        if(Slot != nullptr && Slot->Functions.Set_Body_Type != nullptr) Slot->Functions.Set_Body_Type(Entity, Type);
    }
    else if(Command == "Set_Position" || Command == "Set_Velocity" || Command == "Apply_Force" || Command == "Apply_Impulse")
    {
        std::string Entity;
        float X, Y, Z;
        *Data >> Entity;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Plugin_Slot *Slot = Entity_Slot(Entity);
        if(Slot == nullptr) return;
        if(Command == "Set_Position" && Slot->Functions.Set_Position != nullptr) Slot->Functions.Set_Position(Entity, X, Y, Z);
        else if(Command == "Set_Velocity" && Slot->Functions.Set_Velocity != nullptr) Slot->Functions.Set_Velocity(Entity, X, Y, Z);
        else if(Command == "Apply_Force" && Slot->Functions.Apply_Force != nullptr) Slot->Functions.Apply_Force(Entity, X, Y, Z);
        else if(Command == "Apply_Impulse" && Slot->Functions.Apply_Impulse != nullptr) Slot->Functions.Apply_Impulse(Entity, X, Y, Z);
    }
    else if(Command == "Set_Gravity")
    {
        float X, Y, Z;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        for(auto &Pair : Plugins)
            if(Pair.second.Functions.Set_Gravity != nullptr) Pair.second.Functions.Set_Gravity(X, Y, Z);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Physics Dictionary" << std::endl;
    }
}

extern "C" void Shutdown()
{
    Entity_Bodies.clear();
    Plugins.clear();
}
