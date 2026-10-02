RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "==============================================================="

cat <<'EOF'
  PROGRAMM OUTPUT                                           
EOF

echo "================================================================"

echo "================================================================"

echo "Program 1 Producer-Consumer Problem"

echo "================================================================\n"
gcc -o run producer_consumer.c
./run


echo "============================================================="

echo "Program 2 Deadlock Detection Algorithm"

echo "=============================================================="
gcc -o run deadlock_detection.c
./run

echo ""
echo ""
echo "Program 3 Synchronization and Deadlock"
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
