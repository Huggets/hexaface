@echo off
set arg="%1%"
set command=cmake -B build -D VALIDATION_LAYERS=%1
if %arg%=="true" (
    echo Enabling validation layers
    %command%
    goto end
)
if %arg%=="false" (
    echo Disabling validation layers
    %command%
    goto end
)

:error
echo Wrong argument given
echo You must give "true" or "false"

:end