
# Remove old tree
echo Removing old tree image
rm tree.png

# Convert Dot file
echo Converting Dot file...
dot -Tpng MiniJava_syntax.dot > tree.png
