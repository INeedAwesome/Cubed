@echo off

:: This makes the mkdir command not output to console
mkdir 1>NUL 2>NUL "bin" 

call glslangValidator -o bin/basic.vert.spirv basic.vert.glsl -V
call glslangValidator -o bin/basic.frag.spirv basic.frag.glsl -V

pause
