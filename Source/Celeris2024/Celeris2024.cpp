#include "Celeris2024.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

class FCeleris2024Module : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FString PluginShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Source/Celeris2024/Shaders"));
        AddShaderSourceDirectoryMapping(TEXT("/Celeris2024"), PluginShaderDir);
    }

    virtual void ShutdownModule() override
    {
        // Clean up module resources if needed
    }

    virtual bool IsGameModule() const override
    {
        return true;
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FCeleris2024Module, Celeris2024, "Celeris2024");
