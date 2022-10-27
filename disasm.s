in 1
pop rbx
in -5
pop rcx
in 6
pop rdx
push 19
pop rax
call l_20:
hlt
l_20:
push 0
push rbx
je l_98:
push rcx
dup
mul
push 4
push rbx
mul
push rdx
mul
sub
pop rdx
push rdx
push 0
jb l_124:
push rdx
push 0
je l_85:
push -1
push rcx
mul
push rdx
sqrt
add
push 2
push rbx
mul
div
out
push -1
push rcx
mul
push rdx
sub
push 2
push rbx
mul
div
out
ret
l_85:
push -1
push rcx
mul
push 2
push rbx
mul
div
out
ret
l_98:
push 0
push rcx
je l_106:
jmp l_114:
l_106:
push 0
push rdx
je l_128:
jmp l_124:
l_114:
push -1
push rdx
mul
push rcx
div
out
ret
l_124:
push -111
out
ret
l_128:
push -122
out
ret
