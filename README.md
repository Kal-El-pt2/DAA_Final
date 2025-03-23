# 1.Execution instruction

Downloading and Extracting the files :

    - Download the repo "DAA_Assignment" from https://github.com/Kal-El-pt2/DAA_Final
    - Extract the zip file "DAA_Assignment.zip" .

OR

If git is installed run the following in terminal :

    git clone https://github.com/Kal-El-pt2/DAA_Final

make sure you've the following files :
Tomita.cpp
ELS.cpp
Chiba.cpp
as-skitter.txt
email-Enron.txt
wiki-Vote.txt

Ensure that you've a c++ and python compiler.

Compilation :

Before compiling the cpp file, select the dataset(input-file) by changing the name of the txt file you want to test.
By default it is : "email-Enron.txt"

    1: g++ Tomita.cpp -O2 -o Tomita
    2: g++ ELS.cpp -O2 -o ELS
    3: g++ Chiba.cpp -O2 -o Chiba

Executing the files :

    1: ./Tomita
    2: ./ELS
    3: ./Chiba

Plotting Histograms :

    run Process.py

# 2.Dataset Preparation :

Download txt files from the following websites :

    1. https://snap.stanford.edu/data/email-Enron.html
    2. https://snap.stanford.edu/data/as-Skitter.html
    3. https://snap.stanford.edu/data/wiki-Vote.html

//

    run : dataprep.py

It coverts Directed graph to undirected graph.
Uses a zero-based indexing
Ensures that any String or unwanted lines not present.
Converts the dataset into a standard format. (vertex1, vertex2).

# 3. Individual Contributions

The process of exploring and implementing these papers was an exiciting task full of challenges.

We : Dhairya, Aariv, Saumya, Animish and Shreejeet enjoyed the process and sucessfully implemented these papers.

Dhairya(2022A7PS1377H) and Shreejeet(2022A7PS0036H) :

Read and Implemented the "Listing All Maximal Cliques in Sparse Graphs in Near-optimal Time" paper. (ELS.cpp)

And has also analyzed the results of all papers and hosted it in a webpage : https://daa-assignment-website.vercel.app/

Made a dataprep.py for preprocessing the data.

Aariv(2022A7PS0052H) and Animish(2022A7PS1367H) :

    Read and Implemented the "The worst-case time complexity for generating all maximal cliques and computational experiments". (Tomita.cpp)

    And made process.py for plotting histograms and analyzing the data.

    Crucial efforts in making the report.

Saumya(2022A7PS1318H) :

    Read and Implemented the "Arboricity and Subgraph Listing Algorithms" paper. (Chiba.cpp)

    Compiled all the output files and processed all the output files into Histograms using process.py.
