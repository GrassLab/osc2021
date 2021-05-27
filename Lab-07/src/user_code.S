.section ".text.user"
.global user_process
user_process:
    .incbin "../src/user_program" 
