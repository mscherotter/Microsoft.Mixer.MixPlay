@echo Building NuGet Package for Microsoft.Mixer

c:\dev\nuget pack Microsoft.Mixer.nuspec

copy /Y *.nupkg "c:\dev\nuget packages"

pause