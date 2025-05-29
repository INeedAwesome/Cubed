@echo off

call glslangValidator -o bin/basic.vert.spirv basic.vert.glsl -V
call glslangValidator -o bin/basic.frag.spirv basic.frag.glsl -V

pause