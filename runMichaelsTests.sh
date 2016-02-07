
# Compile
echo "******************Compiling******************"
gcc -ansi lc3bsim2.c -o program.out
echo "*********************************************"
echo

# Assemble
echo "******************Assembling*****************"
#./assembler.linux sourcecode.asm objectfile.obj
./assembler.linux michaelsTests.asm michaelsobjectfile.obj
echo "*********************************************"
echo

# Run
echo "*******************Running*******************"
./program.out michaelsobjectfile.obj
echo "*********************************************"
echo

