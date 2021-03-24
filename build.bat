@echo off

if not exist deps\ mkdir deps

if exist deps\lai\ (
    echo "Updating LAI"
    git -C deps/lai pull
) else (
    echo "Cloning LAI"
    git -C deps clone --depth=1 https://github.com/managarm/lai
)
where /q RBuild.exe
if errorlevel 1 (
    
    if exist deps\RBuild\ (
        echo "Updating RBuild"
        git -C deps/RBuild pull
    ) else (
        echo "Cloning RBuild"
        git -C deps clone --depth=1 https://github.com/RicardoLuis0/RBuild
    )
    
    if not exist .\.RBuild.exe (
        cd deps\RBuild
        build.bat
        copy build\win\release\bin\RBuild.exe ..\..\.RBuild.exe
        ..\..\.RBuild.exe --file=RBuild.json release
        copy build\win\release\bin\RBuild.exe ..\..\.RBuild.exe
        cd ..\..
    ) else (
        cd deps\RBuild
        ..\..\.RBuild.exe --file=RBuild.json release
        copy build\win\release\bin\RBuild.exe ..\..\.RBuild.exe
        cd ..\..
    )
    .\.RBuild.exe --file=build_lai.json
    .\.RBuild.exe --file=build_kernel.json
    
    
) else (
    RBuild --file=build_lai.json
    RBuild --file=build_kernel.json
)
