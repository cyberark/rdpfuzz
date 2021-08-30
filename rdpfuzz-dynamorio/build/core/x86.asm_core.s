









































































































































































































































































































































































   

























































  



  















































































































































































































































































































































































































































































































































































































































































































































































































































 
 
 
.CODE








































































































































































































































































































EXTERN unexpected_return:PROC

EXTERN get_own_context_integer_control:PROC
EXTERN get_simd_vals:PROC
EXTERN auto_setup:PROC
EXTERN return_from_native:PROC
EXTERN native_module_callout:PROC
EXTERN d_r_dispatch:PROC

EXTERN dr_app_start_helper:PROC

EXTERN dynamo_process_exit:PROC
EXTERN dynamo_thread_exit:PROC
EXTERN dynamo_thread_stack_free_and_exit:PROC
EXTERN dynamorio_app_take_over_helper:PROC
EXTERN found_modified_code:PROC
EXTERN get_cleanup_and_terminate_global_do_syscall_entry:PROC



EXTERN internal_exception_info:PROC
EXTERN is_currently_on_dstack:PROC
EXTERN nt_continue_setup:PROC






EXTERN hashlookup_null_target:PROC












EXTERN dynamorio_earliest_init_takeover_C:PROC
EXTERN os_terminate_wow64_stack:PROC



EXTERN exiting_thread_count:PROC
EXTERN d_r_initstack:PROC
EXTERN initstack_mutex:PROC
EXTERN int_syscall_address:PROC
EXTERN syscalls:PROC
EXTERN sysenter_ret_address:PROC
EXTERN sysenter_tls_offset:PROC

EXTERN wow64_index:PROC

EXTERN syscall_argsz:PROC

EXTERN load_dynamo_failure:PROC

















        dynamo_auto_start PROC

        

        mov      rax, rsp
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call auto_setup 
 lea rsp, [32 + 8*0 + rsp]
        
        jmp      load_dynamo_failure
        dynamo_auto_start ENDP




















        call_switch_stack PROC

        


        mov      rax, rsp
        


        lea      rsp, [-8 + rsp] 







        mov      [1*8 + rax], rcx
        mov      [2*8 + rax], rdx
        mov      [3*8 + rax], r8
        mov      [4*8 + rax], r9




        
        push     rbx
        mov      rbx, rax
        
        push     r12 
        mov      r12, rsp
        
        mov      rdx, [3*8 + rax] 
        mov      rcx, [1*8 + rax] 
        mov      rsp, [2*8 + rax] 
        cmp      qword ptr [4*8 + rax], 0 
        je       call_dispatch_alt_stack_no_free
        mov      rax, [4*8 + rax]
        mov      dword ptr [rax], 0
call_dispatch_alt_stack_no_free:
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rcx 
 call rdx 
 lea rsp, [32 + 8*0 + rsp]
        mov      rsp, r12
        mov      rax, rbx
        cmp      byte ptr [5*8 + rax], 0 
        je       unexpected_return
        pop      r12
        pop      rbx


        mov      rsp, rax






        ret
        call_switch_stack ENDP




















        dr_call_on_clean_stack PROC EXPORT

        







        


        mov      rax, rsp
        


        lea      rsp, [-8 + rsp] 










        mov      [1*8 + rax], rcx
        mov      [2*8 + rax], rdx
        mov      [3*8 + rax], r8
        mov      [4*8 + rax], r9







        
        push     rbx
        push     rbp 

        



        push     rsi
        mov      rsi, gs:[8]

        mov      rbx, rax
        mov      rbp, rsp
        
        mov      r10, [2*8 + rax] 
        mov      r11, [1*8 + rax] 
        mov rsp, qword ptr [((18*8 + 48 + ((6)*(64)) + ((8)*(8)))+(8 + 5 * 8)+3*8) + r11]

        
        mov      gs:[8], rsp

        lea rsp, [-32 - 8*4 + rsp]
        mov      r11, [10*8 + rax]
        mov      qword ptr [7*8 + rsp], r11
        mov      r11, [9*8 + rax]
        mov      qword ptr [6*8 + rsp], r11
        mov      r11, [8*8 + rax]
        mov      qword ptr [5*8 + rsp], r11
        mov      r11, [7*8 + rax]
        mov      qword ptr [4*8 + rsp], r11
        mov      r11, [6*8 + rax]
        mov      r9, r11
        mov      r11, [5*8 + rax]
        mov      r8, r11
        mov      r11, [4*8 + rax]
        mov      rdx, r11
        mov      r11, [3*8 + rax]
        mov      rcx, r11
        call     r10
        
        lea rsp, [32 + 8*4 + rsp]
        mov      rsp, rbp
        mov      rcx, rbx

        
        mov      gs:[8], rsi
        pop      rsi

        pop      rbp
        pop      rbx


        mov      rsp, rcx








        ret
        dr_call_on_clean_stack ENDP








        clone_and_swap_stack PROC

        mov      rax, rcx
        mov      rcx, rdx
        mov      rdx, rsp
        
        push     rsi
        push     rdi
        
        sub      rcx, rdx 
        mov      rsi, rdx 
        mov      rdi, rax 
        sub      rdi, rcx
        sub      rax, rcx 
        
        cld
        rep movsb
        
        pop      rdi
        pop      rsi
        mov      rsp, rax
        ret
        clone_and_swap_stack ENDP





        dr_app_start PROC EXPORT

        sub rsp, 16 - 8

        
        
lea rsp, [rsp + (- ((6)*(64)) - ((8)*(8)) - 48)] 
 push qword ptr [16 - 8 + rsp - (- ((6)*(64)) - ((8)*(8)) - 48)] 
 pushfq 
 push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi 
 lea rax, [rsp] 
 lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call get_simd_vals 
 lea rsp, [32 + 8*0 + rsp] 
 lea rax, [(18*8 + 48 + ((6)*(64)) + ((8)*(8))) + rsp] 
 mov [(3*8) + rsp], rax 

        
        lea     rax, [rsp] 
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call dr_app_start_helper 
 lea rsp, [32 + 8*0 + rsp]

        
        add      rsp, (18*8 + 48 + ((6)*(64)) + ((8)*(8))) + 16 - 8
        ret
        dr_app_start ENDP






        dr_app_take_over PROC EXPORT

        jmp      dynamorio_app_take_over
        dr_app_take_over ENDP







        dr_app_running_under_dynamorio PROC EXPORT

        mov      eax, 0
        ret
        dr_app_running_under_dynamorio ENDP






        dynamorio_app_take_over PROC EXPORT

        sub rsp, 16 - 8

        
        
lea rsp, [rsp + (- ((6)*(64)) - ((8)*(8)) - 48)] 
 push qword ptr [16 - 8 + rsp - (- ((6)*(64)) - ((8)*(8)) - 48)] 
 pushfq 
 push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi 
 lea rax, [rsp] 
 lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call get_simd_vals 
 lea rsp, [32 + 8*0 + rsp] 
 lea rax, [(18*8 + 48 + ((6)*(64)) + ((8)*(8))) + rsp] 
 mov [(3*8) + rsp], rax 

        
        lea      rax, [rsp] 
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call dynamorio_app_take_over_helper 
 lea rsp, [32 + 8*0 + rsp]

        
        add      rsp, (18*8 + 48 + ((6)*(64)) + ((8)*(8))) + 16 - 8
        ret
        dynamorio_app_take_over ENDP















        cleanup_and_terminate PROC

        




        mov      rbp, rsp
        lea      rsp, [-8 + rsp] 










        mov      [1*8 + rbp], rcx
        mov      [2*8 + rbp], rdx
        mov      [3*8 + rbp], r8
        mov      [4*8 + rbp], r9






        










        lock inc dword ptr [exiting_thread_count] 

        
        
        mov      rbx, qword ptr [1*8 + rbp] 
        mov qword ptr [(((18*8 + 48 + ((6)*(64)) + ((8)*(8)))+(8 + 5 * 8)+3*8)+1*8) + rbx], 1
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rbx 
 call is_currently_on_dstack 
 lea rsp, [32 + 8*0 + rsp] 
        cmp      al, 0
        jnz      cat_save_dstack
        mov      rbx, 0 
        jmp      cat_done_saving_dstack
cat_save_dstack:
        mov rbx, qword ptr [((18*8 + 48 + ((6)*(64)) + ((8)*(8)))+(8 + 5 * 8)+3*8) + rbx]
cat_done_saving_dstack:
        


        

        lea rsp, [-32 - 8*0 + rsp] 
 call get_cleanup_and_terminate_global_do_syscall_entry 
 lea rsp, [32 + 8*0 + rsp]



        push     rbx 
        push     rax
        
        movzx    esi, byte ptr [5*8 + rbp] 
        cmp      esi, 0
        jz       cat_thread_only
        lea rsp, [-32 - 8*0 + rsp] 
 call dynamo_process_exit 
 lea rsp, [32 + 8*0 + rsp]
        jmp      cat_no_thread
cat_thread_only:
        lea rsp, [-32 - 8*0 + rsp] 
 call dynamo_thread_exit 
 lea rsp, [32 + 8*0 + rsp]
cat_no_thread:
        


        mov      ecx, 1




cat_spin:



        xchg     dword ptr [initstack_mutex], ecx 

        jecxz    cat_have_lock
        
        pause    
        jmp      cat_spin
cat_have_lock:
        

        





        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, -1 
 call os_terminate_wow64_stack 
 lea rsp, [32 + 8*0 + rsp]
        mov      rdi, rax    

        mov      rsi, [2*8 + rbp]  
        pop      rax             
        pop      rcx             



        mov      rbx, rbp 
        





        mov      rsp, qword ptr [d_r_initstack] 

        












        push     rdi   

        push     qword ptr [4*8 + rbx] 
        push     qword ptr [3*8 + rbx] 
        push     rax   
        push     rsi   



        
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rcx 
 call dynamo_thread_stack_free_and_exit 
 lea rsp, [32 + 8*0 + rsp] 



        
        pop      rax   

        
        pop      r10       




        pop      rcx   
        pop      rdx   


















        pop      rsp    

        







        mov      dword ptr [initstack_mutex], 0 

        






        lock dec dword ptr [exiting_thread_count] 


        jmp      r10      



        cleanup_and_terminate ENDP









        global_do_syscall_int PROC


        int      02eh













        global_do_syscall_int ENDP



        global_do_syscall_sygate_int PROC

        


        jmp      qword ptr [int_syscall_address]
        global_do_syscall_sygate_int ENDP











        global_do_syscall_sysenter PROC

        DB 00fh 
 DB 034h 
 






        global_do_syscall_sysenter ENDP









        global_do_syscall_sygate_sysenter PROC

        mov      rsp, rdx
        

        pop      qword ptr [rsp]
        push     qword ptr [sysenter_ret_address]

        syscall  









        global_do_syscall_sygate_sysenter ENDP










       global_do_syscall_syscall PROC

        mov      r10, rcx
        syscall







        global_do_syscall_syscall ENDP











       global_do_syscall_wow64 PROC

        call     qword ptr fs:[00c0h]



        global_do_syscall_wow64 ENDP




        global_do_syscall_wow64_index0 PROC

        xor      ecx, ecx
        call     qword ptr fs:[00c0h]



        global_do_syscall_wow64_index0 ENDP

















































        dynamorio_syscall_int2e PROC

        mov      eax, [4 + esp]
        lea      edx, [8 + esp]
        int      02eh
        ret
        dynamorio_syscall_int2e ENDP

        dynamorio_syscall_sygate_int2e PROC

        mov      eax, [4 + esp]
        lea      edx, [8 + esp]
        call     qword ptr [int_syscall_address]
        ret
        dynamorio_syscall_sygate_int2e ENDP

        dynamorio_syscall_sysenter PROC

        




        mov      eax, [4 + esp]
        mov      rdx, rsp

        syscall  



        


        ret
        dynamorio_syscall_sysenter ENDP

        PUBLIC dynamorio_mach_syscall_fixup
        dynamorio_syscall_sygate_sysenter PROC

        





























        pop      rdx
        mov      eax, dword ptr [sysenter_tls_offset]
        mov      fs:[eax], edx
        pop      rax

        
        mov      rcx, dynamorio_mach_syscall_fixup
        push     rcx



        push     qword ptr [sysenter_ret_address]
        mov      rdx, rsp

        syscall  



dynamorio_mach_syscall_fixup::
        
        push     rax
        
        mov      edx, dword ptr [sysenter_tls_offset]
        push     qword ptr fs:[edx]
        ret
        dynamorio_syscall_sygate_sysenter ENDP








        dynamorio_syscall_syscall PROC

        mov      rax, qword ptr [syscalls]
        
        mov      eax, dword ptr [rax + rcx*4] 
        mov      r10, syscall_argsz
        
        mov      r10d, dword ptr [r10 + rcx*4] 
        cmp      r10, 0
        je       dynamorio_syscall_syscall_ready
        cmp      r10, 1
        je       dynamorio_syscall_syscall_1arg
        cmp      r10, 2
        je       dynamorio_syscall_syscall_2arg
        cmp      r10, 3
        je       dynamorio_syscall_syscall_3arg
        
        mov      rcx, qword ptr [rsp + r10*8 + 8]
        jmp      dynamorio_syscall_syscall_ready
dynamorio_syscall_syscall_1arg:
        mov      rcx, rdx
        jmp      dynamorio_syscall_syscall_ready
dynamorio_syscall_syscall_2arg:
        mov      rcx, r8
        jmp      dynamorio_syscall_syscall_ready
dynamorio_syscall_syscall_3arg:
        mov      rcx, r9
        
dynamorio_syscall_syscall_ready:
        mov      r10, rcx 
        syscall
        ret
        dynamorio_syscall_syscall ENDP



















        dynamorio_syscall_wow64 PROC

        mov      eax, [4 + esp]
        mov      edx, dword ptr [wow64_index]
        mov      ecx, [edx + eax*4]
        mov      edx, dword ptr [syscalls]
        mov      eax, [edx + eax*4]
        lea      edx, [8 + esp]
        call     qword ptr fs:[00c0h]
        ret
        dynamorio_syscall_wow64 ENDP






        dynamorio_syscall_wow64_noedx PROC

        mov      eax, [4 + esp]
        mov      ecx, dword ptr [syscalls]
        mov      eax, [ecx + eax*4]
        mov      ecx, [esp]
        mov      [esp + 4], ecx
        lea      esp, [esp + 4]
        call     qword ptr fs:[00c0h]
        
        mov      ecx, [esp]
        mov      [esp - 4], ecx
        lea      esp, [esp - 4]
        ret
        dynamorio_syscall_wow64_noedx ENDP













































































































































































































































































































































































































































































































































        nt_continue_dynamo_start PROC

        


        
        lea rsp, [rsp + (- ((6)*(64)) - ((8)*(8)) - 48)] 
 push 0 
 pushfq 
 push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi 
 lea rax, [rsp] 
 lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call get_simd_vals 
 lea rsp, [32 + 8*0 + rsp] 
 lea rax, [(18*8 + 48 + ((6)*(64)) + ((8)*(8))) + rsp] 
 mov [(3*8) + rsp], rax
        lea      rax, [rsp] 

        



        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call nt_continue_setup 
 lea rsp, [32 + 8*0 + rsp]
        
        jmp      unexpected_return
        nt_continue_dynamo_start ENDP











        back_from_native_retstubs PROC





        DB 06ah 
 DB 00h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 01h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 02h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 03h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 04h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 05h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 06h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 07h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 08h 
 
        jmp      short back_from_native
        DB 06ah 
 DB 09h 
 
        jmp      short back_from_native
PUBLIC back_from_native_retstubs_end



back_from_native_retstubs_end::
        back_from_native_retstubs ENDP






        back_from_native PROC





        


        
        lea rsp, [rsp + (- ((6)*(64)) - ((8)*(8)) - 48)] 
 push 0 
 pushfq 
 push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi 
 lea rax, [rsp] 
 lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call get_simd_vals 
 lea rsp, [32 + 8*0 + rsp] 
 lea rax, [(18*8 + 48 + ((6)*(64)) + ((8)*(8))) + rsp] 
 mov [(3*8) + rsp], rax
        lea      rax, [rsp] 

        




        and      rsp, -16  

        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call return_from_native 
 lea rsp, [32 + 8*0 + rsp]
        
        jmp      unexpected_return
        back_from_native ENDP











































        dr_try_start PROC EXPORT

        add      rcx, 0
        jmp      dr_setjmp
        dr_try_start ENDP




        dr_setjmp PROC














        mov      rdx, rcx

        mov      [       0 + rdx], rbx
        mov      [  8 + rdx], rcx
        mov      [2*8 + rdx], rdi
        mov      [3*8 + rdx], rsi
        mov      [4*8 + rdx], rbp
        mov      [5*8 + rdx], rsp
        mov      rax, [rsp]
        mov      [6*8 + rdx], rax

        mov      [ 7*8 + rdx], r8
        mov      [ 8*8 + rdx], r9
        mov      [ 9*8 + rdx], r10
        mov      [10*8 + rdx], r11
        mov      [11*8 + rdx], r12
        mov      [12*8 + rdx], r13
        mov      [13*8 + rdx], r14
        mov      [14*8 + rdx], r15

        xor      eax, eax
        ret
        dr_setjmp ENDP



        dr_longjmp PROC

        mov      rax, rdx
        mov      rdx, rcx

        mov      rbx, [       0 + rdx]
        mov      rdi, [2*8 + rdx]
        mov      rsi, [3*8 + rdx]
        mov      rbp, [4*8 + rdx]
        mov      rsp, [5*8 + rdx] 
        mov      rcx, [6*8 + rdx]
        mov      [rsp], rcx    
        mov      rcx, [  8 + rdx]

        mov      r8,  [ 7*8 + rdx]
        mov      r9,  [ 8*8 + rdx]
        mov      r10, [ 9*8 + rdx]
        mov      r11, [10*8 + rdx]
        mov      r12, [11*8 + rdx]
        mov      r13, [12*8 + rdx]
        mov      r14, [13*8 + rdx]
        mov      r15, [14*8 + rdx]

        ret
        dr_longjmp ENDP












        atomic_swap PROC

        mov      rax, rdx
        mov      rcx, rcx 
        xchg     [rcx], eax
        ret
        atomic_swap ENDP




        cpuid_supported PROC

        pushfq
        pop      rax
        mov      ecx, eax      
        xor      eax, 0200000h 
        push     rax
        popfq
        pushfq
        pop      rax
        cmp      ecx, eax
        mov      eax, 0        
        setne    al
        push     rcx         
        popfq
        ret
        cpuid_supported ENDP





        our_cpuid PROC

        mov      rax, rcx
        


        mov      rcx, r8
        mov      rdx, rdx
        push     rbx 
        push     rdi 
        
        mov      rdi, rax
        mov      rax, rdx
        cpuid
        mov      [ 0 + rdi], eax
        mov      [ 4 + rdi], ebx
        mov      [ 8 + rdi], ecx
        mov      [12 + rdi], edx
        pop      rdi 
        pop      rbx 
        ret
        our_cpuid ENDP




        dr_stmxcsr PROC

        mov      rax, rcx
        stmxcsr  [rax]
        ret
        dr_stmxcsr ENDP




        dr_xgetbv PROC

        mov      rax, rcx
        mov      rdx, rdx
        push     rax               
        push     rdx               
        mov      ecx, 0
        
        DB 00fh 
 DB 001h 
 DB 0d0h 
    
        pop      rcx
        mov      dword ptr [rcx], eax  
        pop      rcx
        mov      dword ptr [rcx], edx  
        ret
        dr_xgetbv ENDP




        dr_fxsave PROC

        mov      rax, rcx

        
        DB 048h 
 DB 00fh 
 DB 0aeh 
 DB 000h 
 



        fnclex
        finit
        ret
        dr_fxsave ENDP




        dr_fnsave PROC

        mov      rax, rcx
        
        fnsave   [rax]
        fwait
        ret
        dr_fnsave ENDP




        dr_fxrstor PROC

        mov      rax, rcx

        
        DB 048h 
 DB 00fh 
 DB 0aeh 
 DB 008h 
 



        ret
        dr_fxrstor ENDP




        dr_frstor PROC

        mov      rax, rcx
        frstor   [rax]
        ret
        dr_frstor ENDP





        dr_fxsave32 PROC

        mov      rax, rcx
        fxsave   [rax]
        fnclex
        finit
        ret
        dr_fxsave32 ENDP




        dr_fxrstor32 PROC

        mov      rax, rcx
        fxrstor  [rax]
        ret
        dr_fxrstor32 ENDP




















        call_modcode_alt_stack PROC

        mov      rax, rcx 
        mov      rbx, rdx
        mov      rdi, r8
        mov      rsi, r9
        mov      rdx, qword ptr [5*8 + rsp]
        mov      rcx, qword ptr [7*8 + rsp]
        
        cmp      qword ptr [6*8 + rsp], 0
        je       call_modcode_alt_stack_no_free
        mov      dword ptr [initstack_mutex], 0 
call_modcode_alt_stack_no_free:
        mov rsp, qword ptr [((18*8 + 48 + ((6)*(64)) + ((8)*(8)))+(8 + 5 * 8)+3*8) + rax]
        lea rsp, [-32 - 8*2 + rsp] 
 mov qword ptr [5*8 + rsp], rcx 
 mov qword ptr [4*8 + rsp], rdx 
 mov r9, rsi 
 mov r8, rdi 
 mov rdx, rbx 
 mov rcx, rax 
 call found_modified_code 
 lea rsp, [32 + 8*2 + rsp]
        
        jmp      unexpected_return
        ret
        call_modcode_alt_stack ENDP




















        call_intr_excpt_alt_stack PROC

        mov      rax, rcx
        mov      rbx, rdx
        mov      rdi, r8
        mov      rbp, qword ptr [5*8 + rsp]
        mov      rsi, rsp
        mov      rsp, r9

        

        push     rsi       
        




lea rsp, [-32 - 8*1 + rsp] 
 mov qword ptr [4*8 + rsp], rbp 
 mov r9, 1 
 mov r8, rdi 
 mov rdx, rbx 
 mov rcx, rax 
 call internal_exception_info 
 lea rsp, [32 + 8*1 + rsp]
        pop      rsp
        ret
        call_intr_excpt_alt_stack ENDP






















        get_segments_defg PROC

        xor      eax, eax           
        mov      rcx, rcx
        mov      ax, ds
        mov      [rcx], ax
        mov      rcx, rdx
        mov      ax, es
        mov      [rcx], ax
        mov      rcx, r8
        mov      ax, fs
        mov      [rcx], ax
        mov      rcx, r9
        mov      ax, gs
        mov      [rcx], ax
        ret
        get_segments_defg ENDP


        get_segments_cs_ss PROC

        xor      eax, eax           
        mov      rcx, rcx
        mov      ax, cs
        mov      [rcx], ax
        mov      rcx, rdx
        mov      ax, ss
        mov      [rcx], ax
        ret
        get_segments_cs_ss ENDP







        get_own_context_helper PROC

        
        push     rbx
        push     rsi
        push     rdi

        
        
        mov rdi, rcx


        
        
        lea rsp, [rsp + (- ((6)*(64)) - ((8)*(8)) - 48)] 
 push [(3 * 8) + rsp - (- ((6)*(64)) - ((8)*(8)) - 48)] 
 pushfq 
 push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi 
 lea rax, [rsp] 
 lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call get_simd_vals 
 lea rsp, [32 + 8*0 + rsp] 
 lea rax, [(18*8 + 48 + ((6)*(64)) + ((8)*(8))) + rsp] 
 mov [(3*8) + rsp], rax
        


        lea      rsi, [rsp] 

        mov      rax, rdi




        xor      edi, edi
        mov      di, ss
        xor      ebx, ebx
        mov      bx, cs
        lea rsp, [-32 - 8*0 + rsp] 
 mov r9, rsi 
 mov r8, rdi 
 mov rdx, rbx 
 mov rcx, rax 
 call get_own_context_integer_control 
 lea rsp, [32 + 8*0 + rsp]
        add      rsp, (18*8 + 48 + ((6)*(64)) + ((8)*(8)))
        pop      rdi
        pop      rsi
        pop      rbx
        ret
        get_own_context_helper ENDP












        get_xmm_caller_saved PROC

        mov      rax, rcx
        movups   [rax + 0*64], xmm0
        movups   [rax + 1*64], xmm1
        movups   [rax + 2*64], xmm2
        movups   [rax + 3*64], xmm3
        movups   [rax + 4*64], xmm4
        movups   [rax + 5*64], xmm5














        ret
        get_xmm_caller_saved ENDP







        get_ymm_caller_saved PROC

        mov      rax, rcx
       











        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 000h 

        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 048h 
 DB 040h 

        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 090h 
 DB 080h 
 DB 000h 
 DB 000h 
 DB 000h 

        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 098h 
 DB 0c0h 
 DB 000h 
 DB 000h 
 DB 000h 

        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 0a0h 
 DB 000h 
 DB 001h 
 DB 000h 
 DB 000h 

        DB 0c5h 
 DB 0feh 
 DB 07fh 
 DB 0a8h 
 DB 040h 
 DB 001h 
 DB 000h 
 DB 000h 





























        ret
        get_ymm_caller_saved ENDP






        get_zmm_caller_saved PROC

        mov      rax, rcx
       














        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 000h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 048h 
 DB 001h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 050h 
 DB 002h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 058h 
 DB 003h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 060h 
 DB 004h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 068h 
 DB 005h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 070h 
 DB 006h 

        DB 062h 
 DB 0f1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 078h 
 DB 007h 


       
























        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 040h 
 DB 008h 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 048h 
 DB 009h 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 050h 
 DB 00ah 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 058h 
 DB 00bh 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 060h 
 DB 00ch 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 068h 
 DB 00dh 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 070h 
 DB 00eh 

        DB 062h 
 DB 071h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 078h 
 DB 00fh 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 040h 
 DB 010h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 048h 
 DB 011h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 050h 
 DB 012h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 058h 
 DB 013h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 060h 
 DB 014h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 068h 
 DB 015h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 070h 
 DB 016h 

        DB 062h 
 DB 0e1h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 078h 
 DB 017h 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 040h 
 DB 018h 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 048h 
 DB 019h 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 050h 
 DB 01ah 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 058h 
 DB 01bh 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 060h 
 DB 01ch 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 068h 
 DB 01dh 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 070h 
 DB 01eh 

        DB 062h 
 DB 061h 
 DB 0feh 
 DB 048h 
 DB 07fh 
 DB 078h 
 DB 01fh 


        ret
        get_zmm_caller_saved ENDP






        get_opmask_caller_saved PROC

        mov      rax, rcx
       









        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 000h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 048h 
 DB 008h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 050h 
 DB 010h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 058h 
 DB 018h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 060h 
 DB 020h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 068h 
 DB 028h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 070h 
 DB 030h 

        DB 0c5h 
 DB 0f8h 
 DB 091h 
 DB 078h 
 DB 038h 

        ret
        get_opmask_caller_saved ENDP





        hashlookup_null_handler PROC










        jmp      qword ptr [hashlookup_null_target] 

        hashlookup_null_handler ENDP





PUBLIC safe_read_asm_pre
PUBLIC safe_read_asm_mid
PUBLIC safe_read_asm_post
PUBLIC safe_read_asm_recover













        safe_read_asm PROC

        push rdi 
 push rsi 
 mov rdi, rcx 
 mov rsi, rdx 
 mov rdx, r8           
        
        mov rcx, rsi 
 and rcx, (8 - 1) 
 jz safe_read_asm_aligned 
 neg rcx 
 add rcx, 8 
 cmp rdx, rcx 
 cmovb rcx, rdx 
 sub rdx, rcx 
safe_read_asm_pre:: 
 rep movsb 
safe_read_asm_aligned: 
 
 mov rcx, rdx 
 shr rcx, 3 
safe_read_asm_mid:: 
 rep movsq 
 
 mov rcx, rdx 
 and rcx, (8 - 1) 
safe_read_asm_post:: 
 rep movsb
safe_read_asm_recover::
        mov      rax, rsi        
        pop rsi 
 pop rdi
        ret
        safe_read_asm ENDP



























































































































































































































        dynamorio_earliest_init_takeover PROC EXPORT

        push r15 
 push r14 
 push r13 
 push r12 
 push r11 
 push r10 
 push r9 
 push r8 
 push rax 
 push rcx 
 push rdx 
 push rbx 
 
 push rsp 
 push rbp 
 push rsi 
 push rdi














        
        lea rsp, [-32 - 8*0 + rsp] 
 mov rcx, rax 
 call dynamorio_earliest_init_takeover_C 
 lea rsp, [32 + 8*0 + rsp]
        

        
        pop rdi 
 pop rsi 
 pop rbp 
 pop rbx 
 pop rbx 
 pop rdx 
 pop rcx 
 pop rax 
 pop r8 
 pop r9 
 pop r10 
 pop r11 
 pop r12 
 pop r13 
 pop r14 
 pop r15 

        ret
        dynamorio_earliest_init_takeover ENDP


END
