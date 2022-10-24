push 1
pop rbx
in 5
push 1
pop rdx
call l_13:
l_13:
push rdx
push rcx
ja l_35:
push rdx
push rbx
mul
pop rbx
push 1
push rdx
add
pop rdx
call l_13:
l_35:
push rbx
out
hlt
