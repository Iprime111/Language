cd ../build
./bin/frontend $1
./bin/middleend ./SyntaxTree.tmp ./NameTables.tmp
./bin/backend ./SyntaxTree.tmp ./NameTables.tmp > out.asm

dot -Tpng TreeDumpFrontend.dot > TreeDumpFrontend.png
dot -Tpng TreeDumpBackend.dot  > TreeDumpBackend.png

~/Code/Processor/build/bin/Assembler -s ./out.asm -o $2
~/Code/Processor/build/bin/SoftProcessor -b $2
cd ../tests
