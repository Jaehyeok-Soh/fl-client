
xcopy			/y			.\Engine\Bin\Debug\Engine.dll			.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Release\Engine.dll			.\Client\Bin\Release\
xcopy			/y			.\Engine\Bin\Debug\Engine.dll			.\Tool\Bin\Debug\
xcopy			/y			.\Engine\Bin\Release\Engine.dll			.\Tool\Bin\Release\

xcopy			/y			.\Engine\Bin\Debug\Engine.lib			.\EngineSDK\Library\Debug\
xcopy			/y			.\Engine\Bin\Release\Engine.lib			.\EngineSDK\Library\Release\
xcopy			/y			.\Engine\Bin\Debug\Shader_Deffered.cso	.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Release\Shader_Deffered.cso	.\Client\Bin\Release\

xcopy			/y			.\Engine\Public\*.*						.\EngineSDK\Include\