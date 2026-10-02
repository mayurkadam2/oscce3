cat <<'EOF'
\|/          (__)    
     `\------(oo)
       ||    (__)
       ||w--||     \|/
   \|/

EOF

cat <<'EOF'
  PROGRAMM OUTPUT                                           
EOF

echo "\nProgram 1 Producer-Consumer Problem\n"
gcc -o run producer_consumer.c
./run

echo "\nProgram 2 Deadlock Detection Algorithm\n"
gcc -o run deadlock_detection.c
./run

echo "\nProgram 3 Synchronization and Deadlock\n"
gcc -o run synchronization_deadlock.c
./run

cat <<'EOF'
                                                                                                 
   -           __
 --          ~( @\   \
---   _________]_[__/_>________
     /  ____ \ <>     |  ____  \
    =\_/ __ \_\_______|_/ __ \__D
________(__)_____________(__)____

                                                      
EOF
